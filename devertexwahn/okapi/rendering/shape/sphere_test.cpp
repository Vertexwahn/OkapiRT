/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "flatland/rendering/shape/shape.hpp"
#include "math/transform.hpp"
#include "okapi/rendering/scene/load_scene.h"
#include "okapi/rendering/shape/sphere.h"

#include "gmock/gmock.h"

using namespace de_vertexwahn;

TEST(Sphere, intersection_with_min_t_no_hit) {
    // Arrange
    PropertySet ps {
        {"radius", 10.f},
        {"transform", translate(Vector3f{0.f, 0.f, 100.f})}
    };
    Sphere3f sphere{ps};

    Ray3f ray{
        {0.f,0.f,90.f},
        {0.f,0.f,-1.f},
        1.f,
        1000.f
    };

    // Act
    MediumEvent3f me;
    bool hit = sphere.intersect(ray, me);

    // Assert
    EXPECT_FALSE(hit);
}

TEST(Sphere, intersection_with_max_t_no_hit) {
    // Arrange
    PropertySet ps {
            {"radius", 10.f},
            {"transform", translate(Vector3f{0.f, 0.f, 100.f})}
    };
    Sphere3f sphere{ps};

    Ray3f ray{
            {0.f,0.f, 0.f},
            {0.f,0.f, 1.f},
            1.f,
            10.f
    };

    // Act
    MediumEvent3f me;
    bool hit = sphere.intersect(ray, me);

    // Assert
    EXPECT_FALSE(hit);
}

TEST(Sphere, convert_to_svg) {
    // Arrange
    auto sphere_radius = 1.f;
    auto sphere_center = Vector3f{0.f, 0.f, 0.f};
    auto transform = translate(sphere_center);

    PropertySet ps;
    ps.add_property("radius", sphere_radius);
    ps.add_property("transform", transform);

    Sphere3f sphere{ps};
}

TEST(Sphere, GivenARayAndSphere_WhenComputingIntersection_ThenExpectValidIntersection_pointAndNormal) {
    // Arrange
    Point3f ray_origin{-100,0,0};
    Vector3f ray_direction{1,0,0};
    Ray3f ray{ray_origin, ray_direction, 0.f, 1000.f};

    auto sphere_radius = 1.f;
    auto sphere_center = Vector3f{0.f, 0.f, 0.f};
    auto transform = translate(sphere_center);

    PropertySet ps;
    ps.add_property("radius", sphere_radius);
    ps.add_property("transform", transform);

    // Act
    Sphere3f sphere{ps};

    MediumEvent3f me;
    bool hit = sphere.intersect(ray, me);

    // Assert
    EXPECT_THAT(hit, true);
    EXPECT_THAT(me.p.x(), -1.f);
    EXPECT_THAT(me.p.y(), 0.f);
    EXPECT_THAT(me.p.z(), 0.f);
    EXPECT_THAT(me.geo_frame.n.x(), -1.f);
    EXPECT_THAT(me.geo_frame.n.y(), 0.f);
    EXPECT_THAT(me.geo_frame.n.z(), 0.f);
}

TEST(Sphere, bounds) {
    auto sphere_radius = 1.f;
    auto sphere_center = Vector3f{0.f, 0.f, 0.f};
    auto transform = translate(sphere_center);

    PropertySet ps;
    ps.add_property("radius", sphere_radius);
    ps.add_property("transform", transform);

    Sphere3f sphere{ps};

    EXPECT_THROW(sphere.bounds(), std::runtime_error);
}
