/*
 *  SPDX-FileCopyrightText: Copyright 2024-2025 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/ui/viewport.hpp"

#include "imaging/io/io.hpp"
#include "imaging/io/io_openexr.hpp"
#include "imaging/io/io_pfm.hpp"
#include "okapi/rendering/sensor/sensor.hpp"
#include "okapi/ui/viewport_camera_transform.hpp"

#include "QtCore/QTimer"

using namespace de_vertexwahn;

const std::map<int, CameraController::eKey> keyMap = {
{Qt::Key_W, CameraController::eKey::MoveForward}, {Qt::Key_S, CameraController::eKey::MoveBackward}, {Qt::Key_D, CameraController::eKey::MoveRight},
{Qt::Key_A, CameraController::eKey::MoveLeft},    {Qt::Key_Q, CameraController::eKey::MoveUp},       {Qt::Key_E, CameraController::eKey::MoveDown},
{Qt::Key_1, CameraController::eKey::ViewForward}, {Qt::Key_3, CameraController::eKey::ViewBackward}, {Qt::Key_4, CameraController::eKey::ViewRight},
{Qt::Key_6, CameraController::eKey::ViewLeft},    {Qt::Key_7, CameraController::eKey::ViewUp},       {Qt::Key_9, CameraController::eKey::ViewDown},
};

Viewport::Viewport(const CommandLineArguments& cla, QWidget* parent/* = nullptr*/)  :
QWidget(parent), render_scene_thread_(cla), render_preview_thread_(&render_scene_thread_) {
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    camera_ = de_vertexwahn::make_reference_counted<Camera>();
    camera_->frustum() = CameraFrustum(width(), height(), 0.1f, 1000.f, eProjectionType::Perspective);
    camera_->transformation().offset() = 15;
    camera_->transformation().yaw() = pi_over_4f;

    camera_controller_ = make_reference_counted<CameraController>(camera_);
    camera_controller_->set_state(CameraController::eState::Free);
    camera_controller_->handle_wheel(10.0f);

    draw_next_frame();

    setFixedSize(100,100);

    timer_ = new QTimer();
    timer_->setInterval(16);
    timer_->setSingleShot(false);
    connect(timer_, SIGNAL(timeout()), this, SLOT(tick()));

    last_tick_ = std::chrono::high_resolution_clock::now().time_since_epoch();
    timer_->start();
}

Viewport::~Viewport() {
    disconnect(timer_, SIGNAL(timeout()), this, SLOT(tick()));
    delete timer_;
    timer_ = nullptr;

    if(render_preview_thread_.isRunning()) {
        render_preview_thread_.quit();
    }
    if(render_scene_thread_.isRunning()) {
        render_scene_thread_.quit();
    }
}

void Viewport::tick() {
    std::chrono::nanoseconds current = std::chrono::high_resolution_clock::now().time_since_epoch();
    float delta = (float)((current - last_tick_).count() * std::chrono::nanoseconds::period::num / (double)std::chrono::nanoseconds::period::den);

    camera_controller_->tick(delta);
    camera_->tick(delta);

    //repaint();

    last_tick_ = current;
}

void Viewport::wheelEvent(QWheelEvent* event) {
    //repositionCamera();

    float delta = event->angleDelta().y();
    std::cout << delta << std::endl;

    float factor = pow(1.1, delta / -120.f);
    camera_controller_->handle_wheel(factor);

    QWidget::wheelEvent(event);
}

void Viewport::paintEvent(QPaintEvent * e) {
    if(render_preview_thread_.qimage_) {
        QPainter p(this);
        p.drawImage(rect(), *render_preview_thread_.qimage_);

        bool debug_enabled = false;
        if (debug_enabled) {
            float x = camera_controller_->camera()->transformation().x();
            float y = camera_controller_->camera()->transformation().y();
            float z = camera_controller_->camera()->transformation().z();
            std::string text = fmt::format("Camera: {}, {}, {}", x, y, z);
            QColor colorStyle(240,0,0);
            p.setPen(colorStyle);

            QFont font=p.font() ;
            font.setPointSize(18);
            p.setFont(font);
            p.drawText(50,50, text.c_str());
        }
    }
    else {
        LOG_INFO_WITH_LOCATION("Nothing to draw");
    }
}

void Viewport::save_as_ppm(const QString& filename) {
    store_image(filename.toStdString().data(), render_preview_thread_.image_);
}

void Viewport::save_as_png(const QString& filename) {
    try {
        store_image(filename.toStdString().data(), render_preview_thread_.image_);
    }
    catch (std::exception &ex) {
        QMessageBox msgBox;
        msgBox.setText(ex.what());
        msgBox.exec();
    }
}

void Viewport::save_as_webp(const QString& filename) {
    try {
        store_image(filename.toStdString().data(), render_preview_thread_.image_);
    }
    catch (std::exception &ex) {
        QMessageBox msgBox;
        msgBox.setText(ex.what());
        msgBox.exec();
    }
}

void Viewport::save_as_pfm(const QString& filename) {
    if(render_scene_thread_.render_state_ == RenderState::Rendering ||
       render_scene_thread_.render_state_ == RenderState::Done ) {
        auto image = render_scene_thread_.scene->sensor()->film()->image();
        store_pfm(filename.toStdString().data(), *image.get());
       }
    else {
        LOG_WARNING_WITH_LOCATION("No rendering available");
    }
}

void Viewport::save_as_open_exr(const QString& filename) {
    if(render_scene_thread_.render_state_ == RenderState::Rendering ||
        render_scene_thread_.render_state_ == RenderState::Done ) {
        auto image = render_scene_thread_.scene->sensor()->film()->image();
        store_open_exr(filename.toStdString().data(), image);

        //auto normal_image = render_scene_thread_.scene->sensor()->film()->image(3);
        //std::string normal_image_name = "/home/vertexwahn/Desktop/normal.exr";
        //store_open_exr(normal_image_name.data(), normal_image);

        //auto albedo_image = render_scene_thread_.scene->sensor()->film()->image(6);
        //std::string albedo_image_name = "/home/vertexwahn/Desktop/albedo.exr";
        //store_open_exr(albedo_image_name.data(), albedo_image);
    }
    else {
        LOG_WARNING_WITH_LOCATION("No rendering available");
    }
}

void Viewport::save_as_jpeg(const QString& filename) {
    store_image(filename.toStdString().data(), render_preview_thread_.image_);
}

void Viewport::draw_next_frame() {
    if(render_scene_thread_.render_state_ == RenderState::Rendering ||
       render_scene_thread_.render_state_ == RenderState::Done) {
        static Vector2i old_size = Vector2i(-1,-1);
        Vector2i size = render_scene_thread_.scene->sensor()->film()->size();

        if(old_size != size) {
            this->setFixedSize(size.x(), size.y());
            old_size = size;
        }
       }

    update_camera();
    repaint();

    QTimer::singleShot(1000, this, SLOT(draw_next_frame()));
}

void Viewport::update_camera() {
    if (render_scene_thread_.render_state_ != RenderState::Rendering) {
        return; // noop when rendering is not started yet
    }

    static bool camera_initialized = false;

    if (!camera_initialized) {
        /*
        // Set position for Cornell box
        Vector3f eye{278.f, 273.f, 800.f};
        Vector3f target{278.f, 273.f, 799.f};
        camera_controller_->camera()->transformation().look_at(target, eye);
        */

        /*
        // Set position for Kitchen scene
        Vector3f eye{0.900047714002f, 2.11200457879f, -1.8592650289f};
        Vector3f target{-5.76749146572f, -1.21783664471f, 4.80827415083f};
        camera_controller_->camera()->transformation().look_at(target, eye);
        */

        auto const& scene_camera_transform = render_scene_thread_.scene->sensor()->transform().matrix();
        camera_controller_->camera()->transformation() = compute_camera_transform(scene_camera_transform);

        //std::cout << camera_controller_->camera()->transformation().view_matrix() << std::endl;
        //std::cout << camera_controller_->camera()->transformation().view_matrix() << std::endl;

        camera_initialized = true;
    }

    // find out if camera changed
    static Matrix44f last_transform = camera_controller_->camera()->transformation().view_matrix();
    Matrix44f current_transform = camera_controller_->camera()->transformation().view_matrix();

    if (last_transform != current_transform) {
        last_transform = current_transform;

        // camera has changed
        render_scene_thread_.render_desc_.clear_accumulation_buffer = true;

        render_scene_thread_.scene->sensor()->set_transform(
            camera_controller_->camera()->transformation().view_matrix()
        );
    }
}

void Viewport::mousePressEvent(QMouseEvent* event) {
    if (event->buttons()) {
        Vector2f mouse(event->x(), event->y());
        last_mouse_position_ = mouse;
        //viewCube_->mousePress();
        //repositionCamera();
    }

    // To avoid continuous rendering.
    repaint();
}

void Viewport::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons()) {

        Vector2f mouse(event->position().x(), event->position().y());

        Vector2f m = Vector2f(mouse.x() / width() * 2 - 1, 1 - mouse.y() / height() * 2);

        Vector2f l = Vector2f(last_mouse_position_.x() / width() * 2 - 1, 1 - last_mouse_position_.y() / height() * 2);

        Vector2f delta = l - m;

        bool rotateCameraIfLeftMouseButtonDownAndGhostModeEnabled = (camera_controller_->state() == CameraController::eState::Free) & Qt::LeftButton;
        bool rotateCameraIfLeftMouseButtonDownAndAltKeyPressed = event->buttons() & Qt::LeftButton && event->modifiers() & Qt::ALT;
        bool rotateCamera = rotateCameraIfLeftMouseButtonDownAndGhostModeEnabled || rotateCameraIfLeftMouseButtonDownAndAltKeyPressed;

        if (rotateCamera) {
            camera_->rotate_camera(Vector3f(-delta.x(), delta.y(), 0));
        } else if (event->buttons() & Qt::RightButton) {
            float d = 1;
            //if (lastMousePos_.x() >= 0 && lastMousePos_.x() < width() && lastMousePos_.y() >= 0 && lastMousePos_.y() < height()) {
            //    auto color = depthBuffer_.getPixelColor(lastMousePos_.x(), lastMousePos_.y());
            //    buw::convertD24ToF32(&color, d);
            //}

            if (d < 1) {
                Vector3f screenPos(l.x(), l.y(), d);
                camera_->pan_camera_screen(delta, screenPos);
            } else
                camera_->pan_camera_world(delta);
        }

        last_mouse_position_ = mouse;
    }

    // To avoid continuous rendering.
    if(!camera_controller_->is_camera_moving()) {
        repaint();
    }
}

void Viewport::keyPressEvent(QKeyEvent* event) {
    if (!event->isAutoRepeat()) {
        if (keyMap.find(event->key()) != keyMap.end()) {
            CameraController::eKey key = keyMap.at(event->key());
            camera_controller_->handleKeyDown(key);
            update_camera();
        }
    }

    // To avoid continuous rendering.
    //repaint();
}
void Viewport::keyReleaseEvent(QKeyEvent* event) {
    if (!event->isAutoRepeat()) {
        if (keyMap.find(event->key()) != keyMap.end()) {
            CameraController::eKey key = keyMap.at(event->key());
            camera_controller_->handleKeyUp(key);
            update_camera();
        }
    }

    // To avoid continuous rendering.
    //repaint();
}
