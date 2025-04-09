/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "core/exception.hpp"
#include "core/stl/string.hpp"
#include "okapi/command_line_arguments.hpp"

using namespace boost::program_options;

DE_VERTEXWAHN_BEGIN_NAMESPACE

CommandLineArguments::CommandLineArguments() {
    desc.add_options()
        ("film_filename", value<std::string>(), "Filename under which the final rendered image should be stored")
        ("help,h", "Help screen")
        ("integrator", value<std::string>(), "Defines which integrator should be used.")
        ("intersector", value<std::string>(), "Defines which intersection should be used.")
        ("render_mode", value<std::string>()->default_value("progressive"), "Defines which render mode should be used.")
        ("samples_per_pixel", value<int>()->default_value(0), "Samples per pixel")
        ("scene_filename", value<std::string>()->default_value(""), "Okapi scene filename use as input for rendering")
        ("store_benchmark_json_data", value<bool>()->default_value(false), "Determines if benchmark data such as render time should be uploaded.")
        ("thread_count", value<int>()->default_value(0), "Number of thread that should be used for rendering")
        ("upload_benchmark_data", value<bool>()->default_value(false), "Determines if benchmark data such as render time should be uploaded.")
        ("show_maximized", value<bool>()->default_value(false), "Specifiy if render windows should be shown maximaized" )
        ("version,v", "print version string");
}

void CommandLineArguments::parse_command_line_options(int argc, char **argv) {
    try {
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);
    } catch (const error &ex) {
        LOG_ERROR_WITH_LOCATION("Error parsing command line arguments: {}", ex.what());
    }
}

bool CommandLineArguments::handle_help() const {
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return true;
    }

    return false;
}

bool CommandLineArguments::handle_version() const {
    if(vm.count("version")) {
        std::cout << "Okapi Renderer Alpha 1.0.7" << std::endl;
        return true;
    }

    return false;
}

bool CommandLineArguments::handle_standard_commands() const {
    if(handle_help()) {
        return true;
    }

    if(handle_version()) {
        return true;
    }

    return false;
}

PropertySet CommandLineArguments::get_override_scene_properties() const {
    PropertySet override_scene_properties;

    if(vm.count("integrator")) {
        std::string integrator_type = vm["integrator"].as<std::string>();
        override_scene_properties.add_property("integrator",  integrator_type);
        LOG_INFO_WITH_LOCATION("Override integrator with {}", integrator_type);
    }

    if(vm.count("intersector")) {
        std::string intersector_type = vm["intersector"].as<std::string>();
        override_scene_properties.add_property("intersector",  intersector_type);
        LOG_INFO_WITH_LOCATION("Override intersector with {}", intersector_type);
    }

    if(vm.count("film_filename")) {
        std::string film_filename =  vm["film_filename"].as<std::string>();
        override_scene_properties.add_property("film_filename",  film_filename);
        LOG_INFO_WITH_LOCATION("Override film filename with {}", film_filename);
    }

    int samples_per_pixel = vm["samples_per_pixel"].as<int>();

    if(samples_per_pixel > 0) {
        override_scene_properties.add_property("samples_per_pixel", samples_per_pixel);
        LOG_INFO_WITH_LOCATION("Override sample count ({})", samples_per_pixel);
    }

    return override_scene_properties;
}

bool CommandLineArguments::upload_benchmark_data() const {
    return vm["upload_benchmark_data"].as<bool>();
}

bool CommandLineArguments::store_benchmark_json_data() const {
    return vm["store_benchmark_json_data"].as<bool>();
}

int CommandLineArguments::thread_count() const {
    return vm["thread_count"].as<int>();
}

bool CommandLineArguments::show_maximized() const {
    return vm["show_maximized"].as<bool>();;
}

RenderMode CommandLineArguments::render_mode() const {
    string render_mode = vm["render_mode"].as<std::string>();

    if(render_mode == "progressive") {
        return RenderMode::ParallelProgressive;
    }

    if(render_mode == "tile_based") {
        return RenderMode::ParallelTileBased;
    }

    if(render_mode == "single_threaded_tile_based") {
        return RenderMode::SingleThreadedTileBased;
    }

    LOG_ERROR_WITH_LOCATION("Unknown render mode: {}", render_mode);

    return RenderMode::Unknown;
}

std::string CommandLineArguments::filename() const {
    return vm["scene_filename"].as<std::string>();
}

DE_VERTEXWAHN_END_NAMESPACE
