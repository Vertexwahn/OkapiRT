/*
 *  SPDX-FileCopyrightText: Copyright 2024-2025 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/ui/render_scene_thread.hpp"

#include "core/logging.hpp"
#include "core/stl/filesystem.hpp"
#include "core/stl/string.hpp"
#include "core/timer.hpp"
#include "imaging/io/io.hpp"
#include "okapi/rendering/scene/load_scene.hpp"
#include "okapi/rendering/sensor/sensor.hpp"

using namespace de_vertexwahn;

class RTProgressReporter : public de_vertexwahn::ProgressReporter {
public:
    RTProgressReporter(RenderSceneThread *rt) : rt_{rt} {}

    void update(const std::string& message) override {
        rt_->update(QString::fromStdString(message));
    }

    RenderSceneThread *rt_;
};

RenderSceneThread::RenderSceneThread(const de_vertexwahn::CommandLineArguments& cla) : cla_{cla} {}

void RenderSceneThread::run() {
    Timer timer;

    RTProgressReporter progress_reporter{this};

    render_state_ = RenderState::Loading;

    string filename = cla_.filename();

    if(cla_.filename().empty()) {
        LOG_INFO_WITH_LOCATION("No scene provided via arguments. Loading default scene.");
        //filename = string("okapi/ui/scenes/logo/logo.okapi.xml");
        //filename = string("okapi/ui/scenes/cornell_box/cornell_box.naive.diffuse.box_filter.okapi.xml");
        //filename = string("okapi/ui/scenes/cornell_box_with_spheres/cornell_box_with_spheres.next.okapi_v3.xml");
        filename = string("okapi/ui/scenes/cornell_box/cornell_box_wood_texture.next.okapi.xml");
    }

    if(!filesystem::exists(filename)) {
        LOG_ERROR_WITH_LOCATION("File {} does not exist", filename);
    }

    LOG_INFO_WITH_LOCATION("Loading scene {}.", std::filesystem::path(filename).string());

    emit update(QString("Loading scene %1").arg(filename.data()) );

    scene = load_scene3f(filename, cla_.get_override_scene_properties());

    auto sensor = scene->sensor();

    if(!scene->sensor()) {
        LOG_ERROR_WITH_LOCATION("Sensor is missing in scene description");
        throw Exception("Sensor is missing in scene description");
    }

    render_state_ = RenderState::Rendering;

    LOG_INFO_WITH_LOCATION("Begin rendering.");
    emit update(QString("Begin rendering."));

    auto render_thread_count = cla_.thread_count();
    LOG_INFO_WITH_LOCATION("Render thread count: {}", render_thread_count);

    render_desc_.thread_count = render_thread_count;

    switch (cla_.render_mode()) {
        case RenderMode::ParallelProgressive:
            LOG_INFO_WITH_LOCATION("Parallel progressive rendering.");
            render_interactive_parallel_progressive(scene.get(), render_desc_, &progress_reporter);
            break;
        case RenderMode::ParallelTileBased:
            LOG_INFO_WITH_LOCATION("Parallel tile-based rendering.");
            render_parallel(scene.get(), render_desc_);
            break;
        case RenderMode::SingleThreadedTileBased:
            LOG_INFO_WITH_LOCATION("Single thread tile-based rendering.");
            render(scene.get());
            break;
        default:
            LOG_INFO_WITH_LOCATION("Parallel progressive rendering.");
            render_parallel_progressive(scene.get(), render_desc_, &progress_reporter);
            break;
    }

    render_state_ = RenderState::StoreResults;

    QString result;
    emit resultReady(result);

    // determine out path
    std::filesystem::path p(filename);
    std::stringstream ss;
    ss << p.parent_path().string() << "/" << scene->sensor()->film()->filename();
    std::filesystem::path outPath = ss.str();

    LOG_INFO_WITH_LOCATION("Store EXR to {}.", outPath.string());

    store_image(outPath.string(), scene->sensor()->film()->image());

    render_state_ = RenderState::Done;

    LOG_INFO_WITH_LOCATION("Done.");
    LOG_INFO_WITH_LOCATION("Shutting down now.");
    LOG_INFO_WITH_LOCATION("Render time: {} seconds", timer.elapsed_seconds());

    emit update(QString("RenderTime was: %1 seconds").arg(timer.elapsed_seconds()));
}
