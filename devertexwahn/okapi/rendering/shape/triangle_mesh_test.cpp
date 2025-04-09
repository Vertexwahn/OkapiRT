/*
 *  SPDX-FileCopyrightText: Copyright 2022-2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/shape/triangle_mesh.h"

#include "math/transform.hpp"

#include "gmock/gmock.h"

using namespace de_vertexwahn;

TEST(TriangleMesh3, GivenEmptyVertexBuffer_WhenCreatingTriangleMesh_ExpectException) {
    // Arrange
    auto transform = translate(0.f, 0.f, 0.f);

    PropertySet ps;
    ps.add_property("transform", transform);

    std::vector<unsigned int> position_indices;
    std::vector<Point3f> positions;

	// Act/Assert
    EXPECT_THROW(TriangleMesh3f(ps, positions, position_indices), std::runtime_error);
}

TEST(TriangleMesh3, GivenEmptyIndexBuffer_WhenCreatingTriangleMesh_ExpectException) {
    // Arrange
    auto transform = translate(0.f, 0.f, 0.f);

    PropertySet ps;
    ps.add_property("transform", transform);

    std::vector<unsigned int> position_indices;
    std::vector<Point3f> positions = {
        {0.0f, 0.0f, 0.0f},
    };

	// Act/Assert
    EXPECT_THROW(TriangleMesh3f(ps, positions, position_indices), std::runtime_error);
}

TEST(TriangleMesh3, ctor) {
    // Arrange
    auto transform = translate(1.f, 2.f, 3.f);

    PropertySet ps;
    ps.add_property("transform", transform);

    std::vector<unsigned int> indices = {0, 1, 2};
    std::vector<Point3f> vertices = {
        {-1.f, -1.f, 0.f},
        {1.f, -1.f, 0.f},
        {1.f, 1.f, 0.f},
    };

	// Act
    TriangleMesh3f mesh{ps, vertices, indices};

	// Assert
	EXPECT_THAT(mesh.transform(), translate(1.f, 2.f, 3.f));
	EXPECT_THAT(mesh.position_count(), 3u);
	EXPECT_NE(mesh.position_index(), nullptr);
}

TEST(TriangleMesh3, ctor_empty_position_buffer) {
    // Arrange
    auto transform = translate(1.f, 2.f, 3.f);

    PropertySet ps;
    ps.add_property("transform", transform);

    std::vector<unsigned int> indices = {0, 1, 2};
    std::vector<Point3f> vertices = {};

    // Act
    EXPECT_THROW(TriangleMesh3f(ps, vertices, indices), std::runtime_error);
}

TEST(TriangleMesh3, ctor_invalid_index_buffer) {
    // Arrange
    auto transform = translate(1.f, 2.f, 3.f);

    PropertySet ps;
    ps.add_property("transform", transform);

    std::vector<unsigned int> indices = {0, 1};
    std::vector<Point3f> vertices = {
            {-1.f, -1.f, 0.f},
            {1.f, -1.f, 0.f},
            {1.f, 1.f, 0.f},
    };

    // Act
    EXPECT_THROW(TriangleMesh3f(ps, vertices, indices), std::runtime_error);
}


TEST(TriangleMesh3, ctor_empty_vertex_buffer) {
    // Arrange
    auto transform = translate(1.f, 2.f, 3.f);

    PropertySet ps;
    ps.add_property("transform", transform);

    std::vector<unsigned int> indices = {0, 1, 2};
    std::vector<Point3f> vertices = {};

    // Act
    EXPECT_THROW(TriangleMesh3f(ps, vertices, indices), std::runtime_error);
}

TEST(TriangleMesh3, TryToLoadNotExistingObj) {
    // Arrange
    auto transform = translate(0.f, 0.f, 0.f);

    PropertySet ps;
    ps.add_property("parent_path", std::string("okapi/scenes/cube"));
    ps.add_property("transform", transform);
    ps.add_property("filename", std::string("not_existing.obj"));

    // Act
    EXPECT_THROW((TriangleMesh3f{ps}),std::runtime_error);

    EXPECT_THAT([&]() { TriangleMesh3f{ps}; },
                testing::Throws<std::runtime_error>(Property(&std::runtime_error::what,
                                                    testing::HasSubstr("Loading Wavefront OBJ (okapi/scenes/cube/not_existing.obj) failed."))));
}

TEST(TriangleMesh3, ctor_LoadObj) {
    // Arrange
    auto transform = translate(0.f, 0.f, 0.f);

    PropertySet ps;
    ps.add_property("parent_path", std::string("okapi/scenes/cube"));
    ps.add_property("transform", transform);
    ps.add_property("filename", std::string("cube.obj"));

    // Act
    TriangleMesh3f mesh{ps};

    // setup ray
    Point3f ray_origin{0.f,0.f,100.f};
    Vector3f ray_direction{0.f,0.f,-1.f};
    Ray3f ray{ray_origin, ray_direction, 0.f, 1000.f};

    MediumEvent3f me;
    bool hit = mesh.intersect(ray, me);
    EXPECT_TRUE(hit);

    // Assert
    EXPECT_THAT(mesh.transform(), identity<float>());

    EXPECT_THAT(mesh.position_count(), 8u);
    EXPECT_THAT(mesh.position_index_count(), 36u);
	EXPECT_THAT(mesh.position_index()[0], 1u);
	EXPECT_THAT(mesh.position_index()[1], 2u);
	EXPECT_THAT(mesh.position_index()[2], 3u);

	EXPECT_THAT(mesh.normal_count(), 6u);
	EXPECT_THAT(mesh.normals()[0].x(), .0f);
	EXPECT_THAT(mesh.normals()[0].y(), -1.f);
	EXPECT_THAT(mesh.normals()[0].z(), .0f);

	EXPECT_THAT(mesh.normal_index_count(), 36u);
    EXPECT_THAT(mesh.normal_index_data()[0], 0u);
    EXPECT_THAT(mesh.normal_index_data()[1], 0u);
    EXPECT_THAT(mesh.normal_index_data()[2], 0u);
}

TEST(TriangleMesh3, GivenATriangle_WhenIntersection_ThenHit) {
    // Arrange

    // setup TriangleMesh
    auto transform = translate(0.f, 0.f, 0.f);

    PropertySet ps;
    ps.add_property("transform", transform);

    std::vector<unsigned int> indices = {0, 1, 2};
    // counter clockwise is front facing
    std::vector<Point3f> vertices = {
        {-1.f, -1.f, 0.f},
        {1.f, -1.f, 0.f},
        {0.f, 1.f, 0.f},
    };

    TriangleMesh3f mesh{ps, vertices, indices};

    // Act
    MediumEvent3f me;

    // setup ray
    Point3f ray_origin{0.f,0.f,100.f};
    Vector3f ray_direction{0.f,0.f,-1.f};
    Ray3f ray{ray_origin, ray_direction, 0.f, 1000.f};

    bool hit = mesh.intersect(ray, me);

    // Assert
    EXPECT_THAT(hit, true);
    EXPECT_THAT(me.t, 100.f);
    EXPECT_THAT(me.p.x(), 0.f);
    EXPECT_THAT(me.p.y(), 0.f);
    EXPECT_THAT(me.p.z(), 0.f);
    EXPECT_THAT(me.geo_frame.n.x(), 0.f);
    EXPECT_THAT(me.geo_frame.n.y(), 0.f);
    EXPECT_THAT(me.geo_frame.n.z(), -1.f);
    EXPECT_THAT(me.sh_frame.n.x(), 0.f);
    EXPECT_THAT(me.sh_frame.n.y(), 0.f);
    EXPECT_THAT(me.sh_frame.n.z(), -1.f);
}

TEST(TriangleMesh3, GivenTriangles_WhenIntersection_ThenHitCloserOne) {
    // Arrange

    // setup ray
    Point3f ray_origin{0.f,0.f,-100.f};
    Vector3f ray_direction{0.f,0.f,1.f};
    Ray3f ray{ray_origin, ray_direction, 0.f, 1000.f};

    // setup TriangleMesh
    auto transform = translate(0.f, 0.f, 0.f);

    PropertySet ps;
    ps.add_property("transform", transform);

    std::vector<unsigned int> indices = {0, 1, 2, 3, 4, 5};
    // counter-clockwise is front facing
    std::vector<Point3f> vertices = {
        {-1.f, -1.f, 0.f},
        {1.f, -1.f, 0.f},
        {0.f, 1.f, 0.f},

        {-1.f, -1.f, -1.f},
        {1.f, -1.f, -1.f},
        {0.f, 1.f, -1.f},
    };

    TriangleMesh3f mesh{ps, vertices, indices};

    // Act
    MediumEvent3f me;
    bool hit = mesh.intersect(ray, me);

    // Assert
    EXPECT_THAT(hit, true);
    EXPECT_THAT(me.t, 99.f);
    EXPECT_THAT(me.p.x(), 0.f);
    EXPECT_THAT(me.p.y(), 0.f);
    EXPECT_THAT(me.p.z(), -1.f);
    EXPECT_THAT(me.geo_frame.n.x(), 0.f);
    EXPECT_THAT(me.geo_frame.n.y(), 0.f);
    EXPECT_THAT(me.geo_frame.n.z(), -1.f);
    EXPECT_THAT(me.sh_frame.n.x(), 0.f);
    EXPECT_THAT(me.sh_frame.n.y(), 0.f);
    EXPECT_THAT(me.sh_frame.n.z(), -1.f);
}

TEST(TriangleMesh3, bounds) {
    // Arrange
    auto transform = translate(0.f, 0.f, 0.f);

    PropertySet ps;
    ps.add_property("transform", transform);

    std::vector<unsigned int> indices = {0, 1, 2};
    std::vector<Point3f> vertices = {
            {-1.f, -1.f, 0.f},
            {1.f, -1.0f, 0.f},
            {1.f, 1.f, 1.f},
    };

    TriangleMesh3f mesh{ps, vertices, indices};

    // Act
    auto x =  mesh.bounds();

    // Assert
    EXPECT_THAT(x, (AxisAlignedBoundingBox3f{{-1.f,-1.f,0.f},{1.f, 1.f, 1.f}}));
}

TEST(TriangleMesh3, CheckIfValidTextureCoordinates) {
    // Arrange
    auto transform = translate(0.f, 0.f, 0.f);

    PropertySet ps;
    ps.add_property("parent_path", std::string("okapi/scenes/cornell_box_texture/meshes"));
    ps.add_property("transform", transform);
    ps.add_property("filename", std::string("cbox_floor2.obj"));

    // Act
    TriangleMesh3f mesh{ps};

    // Assert
    EXPECT_THAT(mesh.uv_count(), 4);
    EXPECT_THAT(mesh.uvs()[0].x(), 10.f);
    EXPECT_THAT(mesh.uvs()[0].y(), -9.f);
    EXPECT_THAT(mesh.uvs()[1].x(), 0.f);
    EXPECT_THAT(mesh.uvs()[1].y(), -9.f);
    EXPECT_THAT(mesh.uvs()[2].x(), 0.f);
    EXPECT_THAT(mesh.uvs()[2].y(), 1.f);
    EXPECT_THAT(mesh.uvs()[3].x(), 10.f);
    EXPECT_THAT(mesh.uvs()[3].y(), 1.f);
}
