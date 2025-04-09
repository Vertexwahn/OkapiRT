/*
 *  SPDX-FileCopyrightText: Copyright 2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "math/transform.hpp"
#include "okapi/rendering/shape/serialized_mesh.h"

#include "gmock/gmock.h"

using namespace de_vertexwahn;

TEST(ZStream, read) {
    /// Arrange
    // Create a dummy file
    {
        std::ofstream file;
        file.open("example.txt");
        file << "Writing this to a file.\n";
        file.close();
    }

    // Open dummy file
    std::ifstream file{"example.txt"};

    uint32_t flags = 0;
    ZStream zstream;

    /// Act & Assert
    EXPECT_THROW(zstream.read(file, &flags, sizeof(flags)), std::runtime_error);
}

TEST(SerializedMesh3f, ctor) {
    // Arrange
    PropertySet ps;
    ps.add_property("transform", translate(0.f, 0.f, 0.f));
    ps.add_property("filename", std::string{"okapi/scenes/matpreview/matpreview.serialized"});
    ps.add_property("shape_index", 1);
    SerializedMesh3f sm{ps};
}

TEST(SerializedMesh3f, InitializeWithNegativeShapeIndex) {
    // Arrange
    PropertySet ps;
    ps.add_property("transform", translate(0.f, 0.f, 0.f));
    ps.add_property("filename", std::string{"okapi/scenes/matpreview/matpreview.serialized"});
    ps.add_property("shape_index", -1);

    EXPECT_THROW(SerializedMesh3f{ps}, std::runtime_error);
}

TEST(SerializedMesh3f, InitializeWithInvalidShapeIndex) {
    // Arrange
    PropertySet ps;
    ps.add_property("transform", translate(0.f, 0.f, 0.f));
    ps.add_property("filename", std::string{"okapi/scenes/matpreview/matpreview.serialized"});
    ps.add_property("shape_index", 1000);

    EXPECT_THROW(SerializedMesh3f{ps}, std::runtime_error);
}

TEST(SerializedMesh3f, TryToLoadNotExistingFile) {
    // Arrange
    PropertySet ps;
    ps.add_property("transform", translate(0.f, 0.f, 0.f));
    ps.add_property("filename", std::string{"does_not_exist.serialized"});
    EXPECT_THROW(SerializedMesh3f{ps}, std::runtime_error);
}
