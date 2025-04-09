/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/ui/render_preview_thread.hpp"

#include "core/logging.hpp"
#include "okapi/rendering/rendering.hpp"
#include "okapi/rendering/sensor/sensor.hpp"
#include "okapi/ui/render_state.hpp"
#include "imaging/io/io.hpp"

RenderPreviewThread::RenderPreviewThread(RenderSceneThread* scene_render_thread)  : render_scene_thread_(scene_render_thread) {
    using namespace de_vertexwahn;

    image_ = make_reference_counted<Image4b>(100, 100, ColorRGBA4b{0, 0, 0, 255});
    qimage_ = new QImage(image_->data(), image_->width(), image_->height(), QImage::Format_RGBX8888);

    for(int y = 0; y < image_->height(); ++y) {
        for(int x = 0; x < image_->width(); ++x) {
            ColorRGB3f color = ColorRGB3f{0.24f};
            ColorRGBA4b color8Bit{std::uint8_t(color.red() * 255), std::uint8_t(color.green() * 255), std::uint8_t(color.blue() * 255), 0};
            image_->set_pixel(x, y, color8Bit);
        }
    }

    // create OIDN device
    device_ = oidn::newDevice();

    // check for errors
    char const* error_message = nullptr;
    if (device_.getError(error_message) != oidn::Error::None) {
        LOG_WARNING_WITH_LOCATION("OIDN error: {}", error_message);
    }

    device_.set("verbose", 2);
    device_.commit();
    filter_ = device_.newFilter("RT"); // generic ray tracing filter

    if (device_.getError(error_message) != oidn::Error::None) {
        LOG_WARNING_WITH_LOCATION("OIDN error: {}", error_message);
    }

    filter_.set("hdr", true); // beauty image is HDR
}

RenderPreviewThread::~RenderPreviewThread() {
    delete qimage_;
}

void RenderPreviewThread::run() {
    using namespace de_vertexwahn;

    while(true) {
        if(stop_update_) {
            break;
        }

        QThread::msleep(100);
        if(render_scene_thread_->render_state_ == RenderState::Rendering ||
           render_scene_thread_->render_state_ == RenderState::StoreResults ||
           render_scene_thread_->render_state_ == RenderState::Done) {
            ReferenceCounted<Image3f> image = render_scene_thread_->scene->sensor()->film()->image(0);

            if(denoise_image_) {
                // Create a filter for denoising a beauty (color) image using optional auxiliary images too
                ReferenceCounted<Image3f> denoised_image = make_reference_counted<Image3f>(image->width(), image->height());
                float* output_ptr = denoised_image->data();
                float* color_ptr = image->data();
                filter_.setImage("color", color_ptr, oidn::Format::Float3, image->width(), image->height()); // beauty
                filter_.setImage("output", output_ptr, oidn::Format::Float3, image->width(), image->height()); // denoised beauty
                filter_.commit();

                filter_.execute();

                const char* error_message;
                if (device_.getError(error_message) != oidn::Error::None) {
                    LOG_WARNING_WITH_LOCATION("OIDN error: {}", error_message);
                }

                LOG_INFO_WITH_LOCATION("Denoised image with OIDN. ( {} x {} )", image->width(), image->height());
                image = denoised_image;
            }

            // Update image view
            if(image_->width() != image->width() ||
               image_->height() != image->height()) {
                delete qimage_;

                image_ = make_reference_counted<Image4b>(image->width(), image->height(), ColorRGBA4b{0, 0, 0, 255});
                qimage_ = new QImage(image_->data(), image_->width(), image_->height(), QImage::Format_RGBX8888);
            }

            for(int y = 0; y < image->height(); ++y) {
                for(int x = 0; x < image->width(); ++x) {
                    ColorRGB3f color = image->get_pixel(Point2i(x, y)) * scale_;
                    color = ColorRGB3f{to_srgb(color.red()), to_srgb(color.green()), to_srgb(color.blue())};
                    color.clamp();

                    ColorRGBA4b color8Bit{std::uint8_t(color.red() * 255), std::uint8_t(color.green() * 255), std::uint8_t(color.blue() * 255), 255};
                    image_->set_pixel(x, y, color8Bit);
                }
            }
        }
    }
}

// stolen from Nori: https://github.com/wjakob/nori/blob/f9f33a09856f2a00493c80430f03122502809808/src/gui.cpp#L69
float RenderPreviewThread::to_srgb(float value) {
    if (value < 0.0031308)
        return 12.92 * value;
    return 1.055 * pow(value, 0.41666) - 0.055;
}
