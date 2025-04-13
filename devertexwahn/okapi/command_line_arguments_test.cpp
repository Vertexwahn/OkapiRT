/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "core/exception.hpp"
#include "okapi/command_line_arguments.hpp"

#include "gmock/gmock.h"

using namespace de_vertexwahn;

TEST(CommandLineArguments, parse_help) {
    CommandLineArguments cla;

    int argc = 2;
    char argv0[] = "okapi";
    char argv1[] = "--help";

    char *argv[] = {argv0,
                    argv1,
                    nullptr};

    testing::internal::CaptureStdout();
    cla.parse_command_line_options(argc, argv);

    EXPECT_TRUE(cla.handle_standard_commands());

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_THAT(output, ::testing::HasSubstr("Options:"));
    EXPECT_THAT(output, ::testing::HasSubstr("--help"));
    EXPECT_THAT(output, ::testing::HasSubstr("--version"));
}

TEST(CommandLineArguments, parse_version) {
    CommandLineArguments cla;

    int argc = 2;
    char argv0[] = "okapi";
    char argv1[] = "--version";

    char *argv[] = {argv0,
                    argv1,
                    nullptr};

    cla.parse_command_line_options(argc, argv);

    testing::internal::CaptureStdout();
    EXPECT_TRUE(cla.handle_standard_commands());

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_THAT(output, ::testing::HasSubstr("Okapi Renderer Alpha 1."));
}

TEST(CommandLineArguments, parse_invalid_argument) {
    CommandLineArguments cla;

    int argc = 2;
    char argv0[] = "okapi";
    char argv1[] = "--invalid_argument";

    char *argv[] = {argv0,
                    argv1,
                    nullptr};

    EXPECT_DEATH(cla.parse_command_line_options(argc, argv), "");
    //EXPECT_THROW(cla.parse_command_line_options(argc, argv), Exception);
}

TEST(CommandLineArguments, get_override_scene_properties_integrator) {
    CommandLineArguments cla;

    int argc = 2;
    char argv0[] = "okapi";
    char argv1[] = "--integrator=normal";

    char *argv[] = {argv0,
                    argv1,
                    nullptr};

    cla.parse_command_line_options(argc, argv);

    auto properties = cla.get_override_scene_properties();
    EXPECT_TRUE(properties.has_property("integrator"));
    EXPECT_THAT(properties.get_property<std::string>("integrator"), std::string("normal"));
}

TEST(CommandLineArguments, get_override_scene_properties_intersector) {
    CommandLineArguments cla;

    int argc = 2;
    char argv0[] = "okapi";
    char argv1[] = "--intersector=octree";

    char *argv[] = {argv0,
                    argv1,
                    nullptr};

    cla.parse_command_line_options(argc, argv);

    auto properties = cla.get_override_scene_properties();
    EXPECT_TRUE(properties.has_property("intersector"));
    EXPECT_THAT(properties.get_property<std::string>("intersector"), std::string("octree"));
}

TEST(CommandLineArguments, get_override_scene_properties_film_filename) {
    CommandLineArguments cla;

    int argc = 2;
    char argv0[] = "okapi";
    char argv1[] = "--film_filename=out.exr";

    char *argv[] = {argv0,
                    argv1,
                    nullptr};

    cla.parse_command_line_options(argc, argv);

    auto properties = cla.get_override_scene_properties();
    EXPECT_TRUE(properties.has_property("film_filename"));
    EXPECT_THAT(properties.get_property<std::string>("film_filename"), std::string("out.exr"));
}

TEST(CommandLineArguments, get_override_scene_properties_samples_per_pixel) {
    CommandLineArguments cla;

    int argc = 2;
    char argv0[] = "okapi";
    char argv1[] = "--samples_per_pixel=128";

    char *argv[] = {argv0,
                    argv1,
                    nullptr};

    cla.parse_command_line_options(argc, argv);

    auto properties = cla.get_override_scene_properties();
    EXPECT_TRUE(properties.has_property("samples_per_pixel"));
    EXPECT_THAT(properties.get_property<int>("samples_per_pixel"), 128);
}
