/*
 *  SPDX-FileCopyrightText: Copyright 2022-2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef Okapi_Rendering_3d345263_28ee_44ea_89b8_4d0008b63a23_h
#define Okapi_Rendering_3d345263_28ee_44ea_89b8_4d0008b63a23_h

#include "core/namespace.hpp"
#include "core/stl/string.hpp"
#include "flatland/rendering/scene/scene.hpp"

#include <atomic>

DE_VERTEXWAHN_BEGIN_NAMESPACE

void render(const Scene3f* scene);

struct RenderDescription {
    int thread_count = 0; // if this is set to 0, the maximum number of threads will be used
    std::atomic<bool> abort = false; // if this is set to true, the rendering will be aborted
};

struct InteractiveRenderDescription : RenderDescription {
    std::atomic<bool> clear_accumulation_buffer = false;
    Vector3f camera_position = Vector3f{0.0f, 0.0f, 0.0f};
    bool query_camera_settings = false;
};

class ProgressReporter {
public:
    virtual ~ProgressReporter() = default;
    virtual void update(const string& message) = 0;
};

/**
 * \brief Renders the scene in a parallel and tile-based way.
 */
void render_parallel(const Scene3f* scene, const RenderDescription& render_description);

/**
 * \brief Renders the scene in a parallel and progressive way.
 */
void render_parallel_progressive(
    const Scene3f* scene,
    const RenderDescription& render_description,
    ProgressReporter* progress_reporter = nullptr);

/**
 * \brief Renders the scene in a parallel, progressive and interactive way.
 */
void render_interactive_parallel_progressive(
    const Scene3f* scene,
    InteractiveRenderDescription& render_description,
    ProgressReporter* progress_reporter = nullptr);

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define Okapi_Rendering_3d345263_28ee_44ea_89b8_4d0008b63a23_h
