/*
 *  SPDX-FileCopyrightText: Copyright 2022-2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/rendering.hpp"

#include "okapi/rendering/sensor/film.hpp"
#include "okapi/rendering/sensor/sensor.hpp"
#include "okapi/rendering/sensor/tile_generator.hpp"

#include "core/logging.hpp"
#include "core/stl/format.hpp"
#include "core/timer.hpp"

#include "tbb/global_control.h"
#include "tbb/parallel_for.h"

#include <mutex>

DE_VERTEXWAHN_BEGIN_NAMESPACE

void render(const Scene3f* scene) {
	auto sensor = scene->sensor();
	auto film = sensor->film();
	auto film_size = film->size();
	auto integrator = scene->integrator();
	auto sampler = scene->sampler();
    int spp = sampler->sample_count();

	for(int y = 0; y < film_size.y(); ++y) {
        for(int x = 0; x < film_size.x(); ++x) {
            for(int sample = 0; sample < spp; ++sample) {
                Point2f sample_position = Point2f(x , y) + sampler->next_2d();

                Ray3f ray = sensor->generate_ray(sample_position);
                auto color = integrator->trace(scene, sampler.get(), ray, 0);
                film->add_sample(sample_position, color.data());
            }
        }
    }
}

void render_parallel(const Scene3f* scene, const RenderDescription& render_description) {
    auto sensor = scene->sensor();
    auto film = sensor->film();
    auto film_size = film->size();
    auto integrator = scene->integrator();
    auto filter = sensor->film()->filter();

    int spp = scene->sampler()->sample_count();

    LOG_INFO_WITH_LOCATION("integrator: ", integrator->to_string());
    LOG_INFO_WITH_LOCATION("intersector: {}", scene->intersector()->to_string());
    LOG_INFO_WITH_LOCATION("spp: {}", spp);
    LOG_INFO_WITH_LOCATION("sampler: {}", scene->sampler()->to_string());
    LOG_INFO_WITH_LOCATION("filter: {}", filter->to_string());
    LOG_INFO_WITH_LOCATION("thread_count: {}", render_description.thread_count);

    Vector2i preferred_tile_size{40, 40};
    TileGenerator tg{preferred_tile_size, film_size};
    LOG_INFO_WITH_LOCATION("tile_count: {}", tg.tile_count());
    LOG_INFO_WITH_LOCATION("tile_size: ({}, {})", tg.tile_description(0).size.x(), tg.tile_description(0).size.y());

    std::vector<std::string> aov_names = integrator->aov_names();
    uint32_t channel_count = aov_names.empty() ? 3 : aov_names.size();

    LOG_INFO_WITH_LOCATION("channel count: {}", channel_count);

    if(channel_count != 3u) {
        LOG_INFO_WITH_LOCATION("Allocate AOVs");
        film->allocate_image_tensor_memory(static_cast<int>(channel_count));
    }

    std::mutex mutex;

    const size_t max_parallelism = tbb::global_control::active_value(tbb::global_control::max_allowed_parallelism);
    tbb::global_control c(tbb::global_control::max_allowed_parallelism, render_description.thread_count > 0 ? render_description.thread_count : max_parallelism);

    tbb::parallel_for(0, tg.tile_count(), [&](int index) {
        FilmTileDescription ftd = tg.tile_description(index);

        FilmTile film_tile{ftd.offset, ftd.size, channel_count, filter.get(), film->tile_bounds()}; // where to get the correct channel count?

        auto sampler(scene->sampler()->clone());
        int spp = sampler->sample_count();
        auto tile_size = ftd.size;

        for(int y = 0; y < tile_size.y(); ++y) {
            for(int x = 0; x < tile_size.x(); ++x) {
                for(int sample_index = 0; sample_index < spp; ++sample_index) {
                    // Check if use want to abort rendering?
                    if(render_description.abort) {
                        return;
                    }

                    Point2f sample_position = Point2f(ftd.offset.x() + x, ftd.offset.y() + y) + sampler->next_2d();

                    Ray3f ray = sensor->generate_ray(sample_position);

                    if (channel_count > 3) { // if AOV integrator
                        std::unique_ptr<float[]> aovs(new float[channel_count]);
                        integrator->trace(scene, sampler.get(), ray, 0, aovs.get());
                        film_tile.add_sample(sample_position, aovs.get());
                    }
                    else {
                        auto color = integrator->trace(scene, sampler.get(), ray, 0);
                        film_tile.add_sample(sample_position, color.data());
                    }
                }
            }
        }

        mutex.lock();
        film->add_tile(film_tile);
        mutex.unlock();
    });
}

void render_parallel_progressive(
    const Scene3f* scene,
    const RenderDescription& render_description,
    ProgressReporter* progress_reporter) {
    auto sensor = scene->sensor();
    auto film = sensor->film();
    auto film_size = film->size();
    auto integrator = scene->integrator();
    auto filter = sensor->film()->filter();

    int spp = scene->sampler()->sample_count();

    LOG_INFO_WITH_LOCATION("integrator: ", integrator->to_string());
    LOG_INFO_WITH_LOCATION("intersector: {}", scene->intersector()->to_string());
    LOG_INFO_WITH_LOCATION("spp: {}", spp);
    LOG_INFO_WITH_LOCATION("sampler: {}", scene->sampler()->to_string());
    LOG_INFO_WITH_LOCATION("filter: {}", filter->to_string());
    LOG_INFO_WITH_LOCATION("thread_count: {}", render_description.thread_count);

    Vector2i preferred_tile_size{40, 40};
    TileGenerator tg{preferred_tile_size, film_size};
    LOG_INFO_WITH_LOCATION("tile_count: {}", tg.tile_count());
    LOG_INFO_WITH_LOCATION("tile_size: ({}, {})", tg.tile_description(0).size.x(), tg.tile_description(0).size.y());

    std::vector<std::string> aov_names = integrator->aov_names();
    uint32_t channel_count = aov_names.empty() ? 3 : aov_names.size();

    LOG_INFO_WITH_LOCATION("channel count: {}", channel_count);

    if(channel_count != 3u) {
        LOG_INFO_WITH_LOCATION("Allocate AOVs");
        film->allocate_image_tensor_memory(static_cast<int>(channel_count));
    }

    std::mutex mutex;

    const size_t max_parallelism = tbb::global_control::active_value(tbb::global_control::max_allowed_parallelism);
    tbb::global_control c(tbb::global_control::max_allowed_parallelism, render_description.thread_count > 0 ? render_description.thread_count : max_parallelism);

    for(int sample_index = 0; sample_index < spp; ++sample_index) {
        if(render_description.abort) {
            return;
        }

        LOG_INFO("Progressive rendering: {}/{} SPP", sample_index, spp);

        if (progress_reporter) {
            float progress = static_cast<float>(sample_index) / spp;
            progress_reporter->update(fmt::format("Progressive rendering: {0}/{1} SPP ({2:.2f} %), Camera pos: ", sample_index, spp, progress));
        }

        tbb::parallel_for(0, tg.tile_count(), [&](int index) {
            FilmTileDescription ftd = tg.tile_description(index);

            FilmTile film_tile{ftd.offset, ftd.size, channel_count, filter.get(), film->tile_bounds()}; // where to get the correct channel count?

            auto sampler(scene->sampler()->clone());
            auto tile_size = ftd.size;

            for(int y = 0; y < tile_size.y(); ++y) {
                for(int x = 0; x < tile_size.x(); ++x) {
                    // Check if use want to abort rendering?
                    if(render_description.abort) {
                        return;
                    }

                    Point2f sample_position = Point2f(ftd.offset.x() + x, ftd.offset.y() + y) + sampler->next_2d();

                    Ray3f ray = sensor->generate_ray(sample_position);

                    if (channel_count > 3) { // if AOV integrator
                        std::unique_ptr<float[]> aovs(new float[channel_count]);
                        integrator->trace(scene, sampler.get(), ray, 0, aovs.get());
                        film_tile.add_sample(sample_position, aovs.get());
                    }
                    else {
                        auto color = integrator->trace(scene, sampler.get(), ray, 0);
                        film_tile.add_sample(sample_position, color.data());
                    }
                }
            }

            mutex.lock();
            film->add_tile(film_tile);
            mutex.unlock();
        });
    }
}

void render_interactive_parallel_progressive(
    const Scene3f* scene,
    InteractiveRenderDescription& render_description,
    ProgressReporter* progress_reporter) {
    auto sensor = scene->sensor();
    auto film = sensor->film();
    auto film_size = film->size();
    auto integrator = scene->integrator();
    auto filter = sensor->film()->filter();

    int spp = scene->sampler()->sample_count();

    LOG_INFO_WITH_LOCATION("integrator: ", integrator->to_string());
    LOG_INFO_WITH_LOCATION("intersector: {}", scene->intersector()->to_string());
    LOG_INFO_WITH_LOCATION("spp: {}", spp);
    LOG_INFO_WITH_LOCATION("sampler: {}", scene->sampler()->to_string());
    LOG_INFO_WITH_LOCATION("filter: {}", filter->to_string());
    LOG_INFO_WITH_LOCATION("thread_count: {}", render_description.thread_count);

    Vector2i preferred_tile_size{40, 40};
    TileGenerator tg{preferred_tile_size, film_size};
    LOG_INFO_WITH_LOCATION("tile_count: {}", tg.tile_count());
    LOG_INFO_WITH_LOCATION("tile_size: ({}, {})", tg.tile_description(0).size.x(), tg.tile_description(0).size.y());

    std::vector<std::string> aov_names = integrator->aov_names();
    uint32_t channel_count = aov_names.empty() ? 3 : aov_names.size();

    LOG_INFO_WITH_LOCATION("channel count: {}", channel_count);

    if(channel_count != 3u) {
        LOG_INFO_WITH_LOCATION("Allocate AOVs");
        film->allocate_image_tensor_memory(static_cast<int>(channel_count));
    }

    std::mutex mutex;

    const size_t max_parallelism = tbb::global_control::active_value(tbb::global_control::max_allowed_parallelism);
    tbb::global_control c(tbb::global_control::max_allowed_parallelism, render_description.thread_count > 0 ? render_description.thread_count : max_parallelism);

    for(int sample_index = 0; sample_index < spp; ++sample_index) {
        if(render_description.abort) {
            return;
        }

        if (render_description.clear_accumulation_buffer) {
            film->clear();
            sample_index = 0;
            render_description.clear_accumulation_buffer = false;
        }

        //LOG_INFO("Progressive rendering: {}/{} SPP", sample_index, spp);

        if (progress_reporter) {
            float progress = static_cast<float>(sample_index) / spp;
            progress_reporter->update(fmt::format("Progressive rendering: {0}/{1} SPP ({2:.2f} %)", sample_index, spp, progress));
        }

        tbb::parallel_for(0, tg.tile_count(), [&](int index) {
            FilmTileDescription ftd = tg.tile_description(index);

            FilmTile film_tile{ftd.offset, ftd.size, channel_count, filter.get(), film->tile_bounds()}; // where to get the correct channel count?

            auto sampler(scene->sampler()->clone());
            auto tile_size = ftd.size;

            for(int y = 0; y < tile_size.y(); ++y) {
                for(int x = 0; x < tile_size.x(); ++x) {
                    // Check if use want to abort rendering?
                    if(render_description.abort) {
                        return;
                    }

                    Point2f sample_position = Point2f(ftd.offset.x() + x, ftd.offset.y() + y) + sampler->next_2d();

                    Ray3f ray = sensor->generate_ray(sample_position);

                    if (channel_count > 3) { // if AOV integrator
                        std::unique_ptr<float[]> aovs(new float[channel_count]);
                        integrator->trace(scene, sampler.get(), ray, 0, aovs.get());
                        film_tile.add_sample(sample_position, aovs.get());
                    }
                    else {
                        auto color = integrator->trace(scene, sampler.get(), ray, 0);
                        film_tile.add_sample(sample_position, color.data());
                    }
                }
            }

            mutex.lock();
            film->add_tile(film_tile);
            mutex.unlock();
        });
    }
}

DE_VERTEXWAHN_END_NAMESPACE
