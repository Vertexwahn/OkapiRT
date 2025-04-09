/*
 *  Note: Contains derived and modified code from BlueFramework which is under Apache-2.0 license and
 *  copyrighted by Technical University of Munich, Chair of Computational Modeling and Simulation
 */

#include "okapi/ui/camera.hpp"

#include "core/logging.hpp"

#include <map>

DE_VERTEXWAHN_BEGIN_NAMESPACE

CameraTransformation::CameraTransformation() : translation_(0, 0, 0), rotation_(0, 0, 0), offset_(0) {
}

Vector3f CameraTransformation::view_direction_to_rotation(const Vector3f &dir) {
    float l = Vector2f(dir.x(), dir.z()).norm();

    float yaw = 0;
    float pitch = (float)(pif / 2.0) * (dir.y() >= 0 ? 1 : -1);
    if (l > 0) {
        yaw = atan2(dir.z(), dir.x());
        pitch = atan(dir.y() / l);
    }

    return Vector3f{yaw, pitch, 0.f};
}

CameraTransformation::CameraTransformation(const Vector3f &translation, const Vector3f &rotation, const float offset)
        : translation_(translation), rotation_(rotation), offset_(offset) {
}

const Vector3f &CameraTransformation::translation() const {
    return translation_;
}
Vector3f &CameraTransformation::translation() {
    return translation_;
}

const Vector3f &CameraTransformation::rotation() const {
    return rotation_;
}
Vector3f &CameraTransformation::rotation() {
    return rotation_;
}

const float &CameraTransformation::offset() const {
    return offset_;
}
float &CameraTransformation::offset() {
    return offset_;
}

const float &CameraTransformation::x() const {
    return translation_.x();
}
float &CameraTransformation::x() {
    return translation_.x();
}

const float &CameraTransformation::y() const {
    return translation_.y();
}
float &CameraTransformation::y() {
    return translation_.y();
}

const float &CameraTransformation::z() const {
    return translation_.z();
}
float &CameraTransformation::z() {
    return translation_.z();
}

const float &CameraTransformation::yaw() const {
    return rotation_.x();
}
float &CameraTransformation::yaw() {
    return rotation_.x();
}

const float &CameraTransformation::pitch() const {
    return rotation_.y();
}
float &CameraTransformation::pitch() {
    return rotation_.y();
}

const float &CameraTransformation::roll() const {
    return rotation_.z();
}
float &CameraTransformation::roll() {
    return rotation_.z();
}

CameraTransformation CameraTransformation::operator+(const CameraTransformation &rhs) const {
    return CameraTransformation(translation_ + rhs.translation_, rotation_ + rhs.rotation_, offset_ + rhs.offset_);
}

float mod(const float &a, const float &n) {
    return a - floor(a / n) * n;
}

float angleDiff(const float &a, const float &b) {
    float c = a - b;
    c = mod(c + pif, 2 * pif) - pif;
    return c;
}

CameraTransformation CameraTransformation::operator-(const CameraTransformation &rhs) const {
    Vector3f rot;
    rot.x() = angleDiff(rotation_.x(), rhs.rotation_.x());
    rot.y() = angleDiff(rotation_.y(), rhs.rotation_.y());
    rot.z() = angleDiff(rotation_.z(), rhs.rotation_.z());

    return CameraTransformation(translation_ - rhs.translation_, rot, offset_ - rhs.offset_);
}
CameraTransformation CameraTransformation::operator*(const float rhs) const {
    return CameraTransformation(translation_ * rhs, rotation_ * rhs, offset_ * rhs);
}
CameraTransformation CameraTransformation::operator/(const float rhs) const {
    return CameraTransformation(translation_ / rhs, rotation_ / rhs, offset_ / rhs);
}

CameraTransformation &CameraTransformation::operator+=(const CameraTransformation &rhs) {
    translation_ += rhs.translation_;
    rotation_ += rhs.rotation_;
    offset_ += rhs.offset_;

    return *this;
}
CameraTransformation &CameraTransformation::operator-=(const CameraTransformation &rhs) {
    Vector3f rot;
    rot.x() = angleDiff(rotation_.x(), rhs.rotation_.x());
    rot.y() = angleDiff(rotation_.y(), rhs.rotation_.y());
    rot.z() = angleDiff(rotation_.z(), rhs.rotation_.z());

    translation_ -= rhs.translation_;
    rotation_ = rot;
    offset_ -= rhs.offset_;

    return *this;
}
CameraTransformation &CameraTransformation::operator*=(const float rhs) {
    translation_ *= rhs;
    rotation_ *= rhs;
    offset_ *= rhs;

    return *this;
}
CameraTransformation &CameraTransformation::operator/=(const float rhs) {
    translation_ /= rhs;
    rotation_ /= rhs;
    offset_ /= rhs;

    return *this;
}

float CameraTransformation::length() const {
    VectorType<float, 7> trafo;
    trafo.segment<3>(0) = translation_;
    trafo.segment<3>(3) = rotation_;
    trafo[6] = offset_;

    return trafo.norm();
}
void CameraTransformation::normalize() {
    operator/=(length());
}

Matrix44f CameraTransformation::rotation_matrix() const {
    return rotation_matrix_y(yaw()) * rotation_matrix_x(pitch()) * rotation_matrix_z(roll());
}
Matrix44f CameraTransformation::transformation_matrix() const {
    return translation_matrix(translation()) * rotation_matrix() * translation_matrix(Vector3f(0.f, 0.f, -offset()));
}
Matrix44f CameraTransformation::view_matrix() const {
    return transformation_matrix().inverse();
}

void CameraTransformation::set_view_direction(const Vector3f &dir) {
    /*float l = Vector2f(dir.x(), dir.z()).norm();

    float pitch = atan(dir.y() / l);
    float yaw = camera_->transformation().yaw();
    if (l > 0)
    yaw = atan2(dir.z(), dir.x()) + (float)(M_PI / 2.0);

    auto target = camera_->transformation();
    target.yaw() = yaw;
    target.pitch() = pitch;
    target.roll() = 0;*/
    rotation_ = view_direction_to_rotation(dir);
}

void CameraTransformation::fit_to_view(const Vector3f &min, const Vector3f &max, float field_of_view) {
    Vector3f center = (min + max) / 2.f;

    Vector3f d1, d2;
    d1 = max - min;
    d2 = Vector3f(max.x(), min.y(), min.z()) - Vector3f(min.x(), max.y(), max.z());

    Vector3f view = Vector3f(1, 1, 1);
    Vector3f rot = view_direction_to_rotation(view.normalized());

    float offset = std::max(0.01f, d1.norm() / 2.f / std::tan(field_of_view / 2.f));

    translation_ = center;
    rotation_ = rot;
    offset_ = offset;
}

void CameraTransformation::look_at(const Vector3f &target, const Vector3f &origin) {
    float yaw = 0;
    float pitch = 0;
    float roll = 0;

    Vector3f v = target - origin;

    if (v.norm() > 0) {
        Vector3f v1(v.x(), 0, v.z()), v2;
        if (v1.norm() > 0) {
            v1 = v1.normalized();
            v2 = v.normalized();

            yaw = pi_over_2 - atan2(v1.z(), v1.x());

            if (v.y() != 0)
                pitch = acos(std::max(-1.f, std::min(v1.dot(v2), 1.f)));
        } else
            pitch = pi_over_2;

        if (v.y() > 0)
            pitch *= -1;
    }

    translation_ = target; // ??? origin?
    rotation_.x() = yaw;
    rotation_.y() = pitch;
    rotation_.z() = roll;
    offset_ = v.norm();
}

Camera::Camera() : frustum_(16, 9, 0.1f, 1000, eProjectionType::Perspective) {
}

const CameraTransformation &Camera::transformation() const {
    return transformation_;
}
CameraTransformation &Camera::transformation() {
    return transformation_;
}

const CameraTransformation &Camera::velocity() const {
    return velocity_;
}
CameraTransformation &Camera::velocity() {
    return velocity_;
}

const CameraFrustum &Camera::frustum() const {
    return frustum_;
}
CameraFrustum &Camera::frustum() {
    return frustum_;
}

Matrix44f Camera::view_projection_matrix() const {
    return frustum_.projection_matrix() * transformation_.view_matrix();
}

Vector4f Camera::project(const Vector4f &worldSpace) const {
    Vector4f screenSpace = view_projection_matrix() * worldSpace;
    screenSpace /= screenSpace.w();
    return screenSpace;
}
Vector4f Camera::deproject(const Vector3f &screenSpace) const {
    Vector4f world_space;

    float d1 = frustum().projection_matrix()(2, 2);
    float d2 = frustum().projection_matrix()(2, 3);

    float z = d2 / (screenSpace.z() - d1);

    world_space = Vector4f(screenSpace.x() * z, screenSpace.y() * z, d1 * z + d2, z);

    world_space = view_projection_matrix().inverse() * world_space;
    return world_space;
}

void Camera::pan_camera_world(const Vector2f &screen_space_delta) {
    pan_camera_world(screen_space_delta, transformation().translation());
}

void Camera::pan_camera_world(const Vector2f &screen_space_delta, const Vector3f &world_space_reference) {
    Vector4f screen_space_reference, world_space_delta;

    Matrix44f projection_matrix = frustum().projection_matrix();
    Matrix44f view_matrix = transformation().view_matrix();

    screen_space_reference = Vector4f(world_space_reference.x(), world_space_reference.y(), world_space_reference.z(), 1);

    screen_space_reference = view_matrix * screen_space_reference;

    screen_space_reference = projection_matrix * screen_space_reference;

    float w = screen_space_reference.w();
    screen_space_reference /= w;

    world_space_delta = Vector4f(screen_space_delta.x(), screen_space_delta.y(), world_space_reference.z(), w);

    world_space_delta *= w;

    world_space_delta = projection_matrix.inverse() * world_space_delta;
    world_space_delta.z() = 0;
    world_space_delta.w() = 0;
    world_space_delta = view_matrix.inverse() * world_space_delta;

    transformation().translation() += world_space_delta.segment<3>(0);
}

void Camera::pan_camera_screen(const Vector2f &screen_space_delta, const Vector3f &screen_space_reference) {
    Vector4f w1 = deproject(screen_space_reference);
    Vector4f w2 = deproject(screen_space_reference + Vector3f(screen_space_delta.x(), screen_space_delta.y(), 0));

    Vector4f world_space_delta = w2 - w1;

    transformation().translation() += world_space_delta.segment<3>(0);
}

void Camera::move_camera_global(const Vector3f &worldSpaceDelta) {
    transformation().translation() += worldSpaceDelta;
}
void Camera::move_camera_local(const Vector3f &localDelta) {
    transformation().translation() += transformation().rotation_matrix().block<3, 3>(0, 0) * localDelta;
}

void Camera::orbit_camera(float yaw) {
    transformation().yaw() += yaw;
}
void Camera::pitch_camera(float pitch) {
    transformation().pitch() += pitch;
}
void Camera::roll_camera(float roll) {
    transformation().roll() += roll;
}

void Camera::rotate_camera(const Vector3f &rotation) {
    transformation().rotation() += rotation;
}

void Camera::pull_camera_out(float delta) {
    transformation().offset() *= delta;
}
void Camera::pull_camera_in(float delta) {
    transformation().offset() /= delta;
}

void Camera::set_offset(float offset) {
    Vector3f oldPos = (transformation().transformation_matrix() * Vector4f(0, 0, 0, 1)).block<3, 1>(0, 0);
    Vector3f newCenter = deproject(Vector3f(0, 0, offset)).block<3, 1>(0, 0);

    transformation().translation() = newCenter;
    transformation().offset() = (oldPos - newCenter).norm();
}

void Camera::set_view_direction(const Vector3f &dir) {
    transformation().set_view_direction(dir);
}

void Camera::fit_to_view(const Vector3f &min, const Vector3f &max) {
    transformation().fit_to_view(min, max, frustum().field_of_view());
}

void Camera::look_at(const Vector3f &target, const Vector3f &origin) {
    transformation().look_at(target, origin);
}

void Camera::tick(const float delta) {
    transformation_ += velocity_ * delta;
}

CameraFrustum::CameraFrustum(float left,
                             float right,
                             float bottom,
                             float top,
                             float near,
                             float far,
                             eProjectionType projectionType,
                             float fieldOfView /*= (float)M_PI / 4.f*/,
                             float scaling /*= 0*/)
        : left_plane_(left)
        , right_plane_(right)
        , bottom_plane_(bottom)
        , top_plane_(top)
        , near_plane_(near)
        , far_plane_(far)
        , projection_type_(projectionType)
        , field_of_view_(fieldOfView)
        , scaling_(scaling) {
}

CameraFrustum::CameraFrustum(float width, float height, float near, float far, eProjectionType projectionType, float fieldOfView /*= (float)M_PI / 4.f*/, float scaling /*= 0*/)
        : left_plane_(-width / 2.f)
        , right_plane_(width / 2.f)
        , bottom_plane_(-height / 2.f)
        , top_plane_(height / 2.f)
        , near_plane_(near)
        , far_plane_(far)
        , projection_type_(projectionType)
        , field_of_view_(fieldOfView)
        , scaling_(scaling) {
}

float CameraFrustum::width() const {
    return right_plane_ - left_plane_;
}
float CameraFrustum::height() const {
    return top_plane_ - bottom_plane_;
}
float CameraFrustum::depth() const {
    return far_plane_ - near_plane_;
}

void CameraFrustum::width(float w) {
    left_plane_ = -w / 2.f;
    right_plane_ = w / 2.f;
}
void CameraFrustum::height(float h) {
    bottom_plane_ = -h / 2.f;
    top_plane_ = h / 2.f;
}
void CameraFrustum::depth(float d) {
    near_plane_ = 0.001f;
    far_plane_ = d - near_plane_;
}

float CameraFrustum::field_of_view() const {
    return field_of_view_;
}
void CameraFrustum::field_of_view(float f) {
    field_of_view_ = f;
};

Matrix44f CameraFrustum::projection_matrix() const {
    switch (projection_type_) {
        case eProjectionType::Perspective: return perspective_projection_matrix_RH(field_of_view_, width() / height(), near_plane_, far_plane_); break;
        case eProjectionType::Orthographic:
        default: return orthographic_projection_matrix_RH(width() / scaling_, height() / scaling_, -far_plane_, far_plane_);
    }
}

CameraController::CameraController() : state_(eState::Orbiting), interpolating_(false), moveDirection_(0, 0, 0), velocity_(5) {
    camera_ = make_reference_counted<Camera>();
}
CameraController::CameraController(ReferenceCounted<Camera> camera)
        : camera_(camera), state_(eState::Orbiting), interpolating_(false), moveDirection_(0, 0, 0), velocity_(5) {
}

void CameraController::set_state(eState state) {
    if (state_ == eState::Orbiting && state == eState::Free)
        camera_->set_offset(0);
    else if (state_ == eState::Free && state == eState::Orbiting)
        camera_->set_offset(0.1f);

    moveDirection_ *= 0;
    state_ = state;
}

CameraController::eState CameraController::state() {
    return state_;
}

void CameraController::tick(const float delta) {
    if (interpolating_) {
        auto diff = target_ - camera_->transformation();
        if (diff.length() < camera_->velocity().length() * delta) {
            camera_->transformation() = target_;
            camera_->velocity() *= 0;
            interpolating_ = false;
        } else {
            start_interpolation(std::max(duration_ - delta, delta), target_);
        }
    } else {
        Vector3f vel(0, 0, 0);
        if (velocity_ > 0 && moveDirection_.norm() > 0)
            vel = moveDirection_.normalized() * velocity_;
        vel = camera_->transformation().rotation_matrix().block<3, 3>(0, 0) * vel;
        camera_->velocity().translation() = vel;
    }
}

void CameraController::set_view_direction(const Vector3f &dir) {
    set_view_direction(dir, 0.5f);
}

void CameraController::set_view_direction(const Vector3f &dir, const float duration) {
    auto target = camera_->transformation();
    target.set_view_direction(dir);

    start_interpolation(duration, target);
}

Vector3f CameraController::get_view_direction_vector(const eViewDirection &direction) {
    Vector3f viewDirection;
    viewDirection[1] = (direction & eViewDirection::Top) > 0 ? 1.0f : (direction & eViewDirection::Bottom) > 0 ? -1.0f : 0.0f;
    viewDirection[2] = (direction & eViewDirection::Right) > 0 ? -1.0f : (direction & eViewDirection::Left) > 0 ? 1.0f : 0.0f;
    viewDirection[0] = (direction & eViewDirection::Front) > 0 ? 1.0f : (direction & eViewDirection::Back) > 0 ? -1.0f : 0.0f;
    return viewDirection;
}


void CameraController::fit_to_view(const Vector3f &min, const Vector3f &max, const float duration /*= 1.0f*/) {
    CameraTransformation target = camera_->transformation();
    target.fit_to_view(min, max, camera_->frustum().field_of_view());

    state_ = eState::Orbiting;
    start_interpolation(duration, target);
}

void CameraController::look_at(const Vector3f &target, const Vector3f &origin, const float duration /*= 1.0f*/) {
    CameraTransformation targetTransformation = camera_->transformation();
    targetTransformation.look_at(target, origin);

    state_ = eState::Orbiting;
    start_interpolation(duration, targetTransformation);
}

void CameraController::start_interpolation(float duration, const CameraTransformation &target) {
    duration_ = duration;
    target_ = target;
    auto diff = target_ - camera_->transformation();

    camera_->velocity() = diff / duration;
    interpolating_ = true;
}

const std::map<CameraController::eKey, Vector3f> key2MoveDirection = {
        {CameraController::eKey::MoveForward, Vector3f(0, 0, 1)}, {CameraController::eKey::MoveBackward, Vector3f(0, 0, -1)},
        {CameraController::eKey::MoveRight, Vector3f(1, 0, 0)},   {CameraController::eKey::MoveLeft, Vector3f(-1, 0, 0)},
        {CameraController::eKey::MoveUp, Vector3f(0, 1, 0)},      {CameraController::eKey::MoveDown, Vector3f(0, -1, 0)},
};

const std::map<CameraController::eKey, Vector3f> key2ViewDirection = {
        {CameraController::eKey::ViewForward, Vector3f(0, 0, 1)}, {CameraController::eKey::ViewBackward, Vector3f(0, 0, -1)},
        {CameraController::eKey::ViewRight, Vector3f(1, 0, 0)},   {CameraController::eKey::ViewLeft, Vector3f(-1, 0, 0)},
        {CameraController::eKey::ViewUp, Vector3f(0, 1, 0)},      {CameraController::eKey::ViewDown, Vector3f(0, -1, 0)},
};

void CameraController::handleKeyDown(eKey key) {
    if (state_ == eState::Free && key2MoveDirection.find(key) != key2MoveDirection.end()) {
        moveDirection_ += key2MoveDirection.at(key);
    } else if (state_ == eState::Orbiting && key2ViewDirection.find(key) != key2ViewDirection.end()) {
        set_view_direction(key2ViewDirection.at(key));
    }
}

void CameraController::handleKeyUp(eKey key) {
    if (state_ == eState::Free && key2MoveDirection.find(key) != key2MoveDirection.end()) {
        moveDirection_ -= key2MoveDirection.at(key);
    }
}

void CameraController::handle_wheel(float factor) {
    if (state_ == eState::Free)
        velocity_ *= factor;
    else if (state_ == eState::Orbiting)
        camera_->pull_camera_out(factor);
}

ReferenceCounted<Camera> CameraController::camera() {
    return camera_;
}

const bool CameraController::is_camera_moving() const {
    return camera_->velocity().length() > 0.0f;
}

DE_VERTEXWAHN_END_NAMESPACE
