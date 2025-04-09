/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/intersector/embree_intersector.hpp"
#include "okapi/rendering/shape/triangle_mesh.h"

#include "gmock/gmock.h"

using namespace de_vertexwahn;

TEST(EmbreeIntersector, IntersectTriangle) {
	// Arrange
    auto transform = translate(0.f, 0.f, 0.f);

    PropertySet ps;
    ps.add_property("transform", transform);

    std::vector<unsigned int> indices = {0, 1, 2};
    std::vector<Point3f> positions = {
        {-1.f-.5f, -1.f, 10.f},
        { 1.f-.5f, -1.f, 10.f},
        { 1.f-.5f,  1.f, 10.f},
    };

    ReferenceCounted<TriangleMesh3f> mesh =
            make_reference_counted<TriangleMesh3f>(ps, positions, indices);

	EXPECT_THAT(mesh->position_index()[0], 0);
	EXPECT_THAT(mesh->position_index()[1], 1);
	EXPECT_THAT(mesh->position_index()[2], 2);

    const unsigned int *indexPtr = mesh->position_index();

    EXPECT_THAT(indexPtr[0], 0);
    EXPECT_THAT(indexPtr[1], 1);
    EXPECT_THAT(indexPtr[2], 2);

    EmbreeIntersector intersector{PropertySet{}};

    std::vector<ReferenceCounted<Shape3f>> shapes;
	shapes.push_back(mesh);

    Point3f ray_origin{0.f,0.f, -100.f};
    Vector3f ray_direction{0.f, 0.f, 1.f};
    Ray3f ray{ray_origin, ray_direction, 0.f, 1000.f};

	// Act
	intersector.build_acceleration_structure(shapes);

    MediumEvent3f me;
	bool hit = intersector.intersect(ray, me);

    // Assert
    ASSERT_TRUE(hit);

    EXPECT_THAT(me.p.x(), testing::FloatNear(0.f, .00001f));
    EXPECT_THAT(me.p.y(), testing::FloatNear(0.f, .00001f));
    EXPECT_THAT(me.p.z(), testing::FloatNear(10.f, .00001f));

    EXPECT_THAT(me.geo_frame.n.x(), testing::FloatNear(0.f, .00001f));
    EXPECT_THAT(me.geo_frame.n.y(), testing::FloatNear(0.f, .00001f));
    EXPECT_THAT(me.geo_frame.n.z(), testing::FloatNear(-1.f, .00001f));

	auto n = me.geo_frame.s.cross(me.geo_frame.t);
	EXPECT_THAT(n.x(), testing::FloatNear(0.f, .00001f));
	EXPECT_THAT(n.y(), testing::FloatNear(0.f, .00001f));
	EXPECT_THAT(n.z(), testing::FloatNear(-1.f, .00001f));
}

TEST(EmbreeIntersector, IntersectTranslatedTriangle) {
    // Arrange
    auto transform = translate(0.f, 0.f, 10.f);

    PropertySet ps;
    ps.add_property("transform", transform);

    std::vector<unsigned int> indices = {0, 1, 2};
    std::vector<Point3f> positions = {
        {-1.f-.5f, -1.f, 0.f},
        { 1.f-.5f, -1.f, 0.f},
        { 1.f-.5f,  1.f, 0.f},
    };

    ReferenceCounted<TriangleMesh3f> mesh =
            make_reference_counted<TriangleMesh3f>(ps, positions, indices);

    EXPECT_THAT(mesh->position_index()[0], 0);
    EXPECT_THAT(mesh->position_index()[1], 1);
    EXPECT_THAT(mesh->position_index()[2], 2);

    const unsigned int *index_ptr = mesh->position_index();

    EXPECT_THAT(index_ptr[0], 0);
    EXPECT_THAT(index_ptr[1], 1);
    EXPECT_THAT(index_ptr[2], 2);

    EmbreeIntersector intersector{PropertySet{}};

    std::vector<ReferenceCounted<Shape3f>> shapes;
    shapes.push_back(mesh);

    Point3f ray_origin{0.f, 0.f, -100.f};
    Vector3f ray_direction{0.f,0.f,1.f};
    Ray3f ray{ray_origin, ray_direction, 0.f, 1000.f};

    // Act
    intersector.build_acceleration_structure(shapes);

    MediumEvent3f me;
    bool hit = intersector.intersect(ray, me);

    // Assert
    ASSERT_TRUE(hit);

    EXPECT_THAT(me.p.x(), testing::FloatNear(0.f, .00001f));
    EXPECT_THAT(me.p.y(), testing::FloatNear(0.f, .00001f));
    EXPECT_THAT(me.p.z(), testing::FloatNear(10.f, .00001f));

    EXPECT_THAT(me.geo_frame.n.x(), testing::FloatNear(0.f, .00001f));
    EXPECT_THAT(me.geo_frame.n.y(), testing::FloatNear(0.f, .00001f));
    EXPECT_THAT(me.geo_frame.n.z(), testing::FloatNear(-1.f, .00001f));

    auto n = me.geo_frame.s.cross(me.geo_frame.t);
    EXPECT_THAT(n.x(), testing::FloatNear(0.f, .00001f));
    EXPECT_THAT(n.y(), testing::FloatNear(0.f, .00001f));
    EXPECT_THAT(n.z(), testing::FloatNear(-1.f, .00001f));
}

TEST(EmbreeIntersector, IntersectTriangle2) {
    // Arrange
    auto transform = identity<float>();

    PropertySet ps;
    ps.add_property("transform", transform);

    std::vector<unsigned int> indices = {0, 1, 2};
    std::vector<Point3f> positions = {
      {-1.f-.5f, -1.f, 10.f},
      { 1.f-.5f, -1.f, 10.f},
      { 1.f-.5f,  1.f, 10.f},
    };

    ReferenceCounted<TriangleMesh3f> mesh =
            make_reference_counted<TriangleMesh3f>(ps, positions, indices);

    EXPECT_THAT(mesh->position_index()[0], 0);
    EXPECT_THAT(mesh->position_index()[1], 1);
    EXPECT_THAT(mesh->position_index()[2], 2);

    const unsigned int *indexPtr = mesh->position_index();

    EXPECT_THAT(indexPtr[0], 0);
    EXPECT_THAT(indexPtr[1], 1);
    EXPECT_THAT(indexPtr[2], 2);

    EmbreeIntersector intersector{PropertySet{}};

    std::vector<ReferenceCounted<Shape3f>> shapes;
    shapes.push_back(mesh);

    Point3f ray_origin{0.f, 0.f, -100.f};
    Vector3f ray_direction{0.f, 0.f, 1.f};
    Ray3f ray{ray_origin, ray_direction, 0.f, 1000.f};

    // Act
    intersector.build_acceleration_structure(shapes);

    MediumEvent3f me;
    bool hit = intersector.intersect(ray, me);

    // Assert
    EXPECT_THAT(hit, true);

    EXPECT_THAT(me.p.x(), testing::FloatNear(0.f, .00001f));
    EXPECT_THAT(me.p.y(), testing::FloatNear(0.f, .00001f));
    EXPECT_THAT(me.p.z(), testing::FloatNear(10.f, .00001f));

    EXPECT_THAT(me.geo_frame.n.x(), testing::FloatNear(0.f, .00001f));
    EXPECT_THAT(me.geo_frame.n.y(), testing::FloatNear(0.f, .00001f));
    EXPECT_THAT(me.geo_frame.n.z(), testing::FloatNear(-1.f, .00001f));

    auto n = me.geo_frame.s.cross(me.geo_frame.t);
    EXPECT_THAT(n.x(), testing::FloatNear(0.f, .00001f));
    EXPECT_THAT(n.y(), testing::FloatNear(0.f, .00001f));
    EXPECT_THAT(n.z(), testing::FloatNear(-1.f, .00001f));
}

TEST(EmbreeIntersector, to_string) {
    EmbreeIntersector intersector{PropertySet{}};
    EXPECT_THAT(intersector.to_string(), "EmbreeIntersector");
}
