/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef De_Vertexwahn_Okapi_RenderPreviewThread_399c9784_2a43_4d23_b6bb_ecd3b49b77dd_hpp
#define De_Vertexwahn_Okapi_RenderPreviewThread_399c9784_2a43_4d23_b6bb_ecd3b49b77dd_hpp

#include "okapi/ui/render_scene_thread.hpp"

#include "core/timer.hpp"
#include "imaging/image.hpp"

#include "QtCore/QThread"
#include "QtGui/QImage"

#include "OpenImageDenoise/oidn.hpp"

class RenderPreviewThread : public QThread {
    Q_OBJECT

public:
    RenderPreviewThread(RenderSceneThread* scene_render_thread);
    ~RenderPreviewThread();

public:
    // need to run always since user can always change gamma value
    void run() override;

private:
    float to_srgb(float value);

public:
    bool stop_update_ = false;
    de_vertexwahn::ReferenceCounted<de_vertexwahn::Image4b> image_;

    QImage* qimage_ = nullptr;
    float scale_ = 1.0f;

    // Denosing
    bool denoise_image_ = false;
    oidn::DeviceRef device_{};
    oidn::FilterRef filter_{};

private:
    RenderSceneThread* render_scene_thread_;
};

#endif // end define De_Vertexwahn_Okapi_RenderPreviewThread_399c9784_2a43_4d23_b6bb_ecd3b49b77dd_hpp
