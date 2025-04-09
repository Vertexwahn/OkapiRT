/*
 *  SPDX-FileCopyrightText: Copyright 2022-2025 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef De_Vertexwahn_Core_CommandLineArguments_974ec29b_9d0c_477c_939c_06b87ff109d9_h
#define De_Vertexwahn_Core_CommandLineArguments_974ec29b_9d0c_477c_939c_06b87ff109d9_h

#include "core/logging.hpp"
#include "core/namespace.hpp"
#include "flatland/rendering/property_set.h"

#include "boost/program_options.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

enum class RenderMode {
    ParallelProgressive,
    ParallelTileBased,
    SingleThreadedProgressive,
    SingleThreadedTileBased,
    Unknown
};

class CommandLineArguments {
public:
    CommandLineArguments();

    void parse_command_line_options(int argc, char **argv);

    bool handle_help() const;

    bool handle_version() const;

    bool handle_standard_commands() const;

    bool show_maximized() const;

    PropertySet get_override_scene_properties() const;

    bool upload_benchmark_data() const;

    bool store_benchmark_json_data() const;

    int thread_count() const;

    RenderMode render_mode() const;

    std::string filename() const;

public:
    boost::program_options::variables_map vm;
    boost::program_options::options_description desc{"Options"};
};

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define De_Vertexwahn_Core_CommandLineArguments_974ec29b_9d0c_477c_939c_06b87ff109d9_h
