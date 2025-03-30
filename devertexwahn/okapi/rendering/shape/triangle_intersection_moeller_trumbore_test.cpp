/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/shape/triangle_mesh.h"

#include "math/transform.hpp"

#include "gmock/gmock.h"

using namespace de_vertexwahn;

TEST(intersect_triangle_moeller_trumbore, trivial_hit) {
    // Arrange

    // setup ray
    Point3f ray_origin{0.f, 0.f, 100.f};
    Vector3f ray_direction{0.f, 0.f, -1.f};
    Ray3f ray{ray_origin, ray_direction, 0.f, 1000.f};

    // counter clockwise is front facing
    std::vector<Point3f> vertices = {
            {-1.f, -1.f, 0.f},
            {1.f,  -1.f, 0.f},
            {0.f,  1.f,  0.f},
    };

    float t = -1.f;
    float u = -1.f;
    float v = -1.f;

    // Act
    bool hit = intersect_triangle_moeller_trumbore(ray, vertices[0], vertices[1], vertices[2], t, u, v);

    // Assert
    EXPECT_THAT(hit, true);
    EXPECT_THAT(t, 100.f);
    EXPECT_THAT(u, .25f);
    EXPECT_THAT(v, .5f);
}

TEST(intersect_triangle_moeller_trumbore, trivial_hit2) {
    // Arrange

    // setup ray
    Point3f ray_origin{0.f, 0.f, -100.f};
    Vector3f ray_direction{0.f, 0.f, 1.f};
    Ray3f ray{ray_origin, ray_direction, 0.f, 1000.f};

    // counter clockwise is front facing
    std::vector<Point3f> vertices = {
            {-1.f, -1.f, -1.f},
            {1.f,  -1.f, -1.f},
            {0.f,  1.f,  -1.f},
    };

    float t = -1.f;
    float u = -1.f;
    float v = -1.f;

    // Act
    bool hit = intersect_triangle_moeller_trumbore(ray, vertices[0], vertices[1], vertices[2], t, u, v);

    // Assert
    EXPECT_THAT(hit, true);
    EXPECT_THAT(t, 99.f);
    EXPECT_THAT(u, .25f);
    EXPECT_THAT(v, .5f);
}

TEST(intersect_triangle_moeller_trumbore, hit_even_it_is_a_miss) {
    // Arrange

    // setup ray
    Point3f ray_origin{0.f, 0.f, 100.f};
    Vector3f ray_direction{0.f, 0.f, 1.f};
    Ray3f ray{ray_origin, ray_direction, 0.f, 1000.f};

    // counter clockwise is front facing
    std::vector<Point3f> vertices = {
            {-1.f, -1.f, 0.f},
            {1.f,  -1.f, 0.f},
            {0.f,  1.f,  0.f},
    };

    float t = -1.f;
    float u = -1.f;
    float v = -1.f;

    // Act
    bool hit = intersect_triangle_moeller_trumbore(ray, vertices[0], vertices[1], vertices[2], t, u, v);

    // Assert
    EXPECT_THAT(hit, true);
    EXPECT_THAT(t, -100.f);
    EXPECT_THAT(u, .25f);
    EXPECT_THAT(v, .5f);
}

TEST(intersect_triangle_moeller_trumbore, miss) {
    // Arrange

    // setup ray
    Point3f ray_origin{100.f, 0.f, 100.f};
    Vector3f ray_direction{0.f, 0.f, 1.f};
    Ray3f ray{ray_origin, ray_direction, 0.f, 1000.f};

    // counter clockwise is front facing
    std::vector<Point3f> vertices = {
            {-1.f, -1.f, 0.f},
            {1.f,  -1.f, 0.f},
            {0.f,  1.f,  0.f},
    };

    float t = -1.f;
    float u = -1.f;
    float v = -1.f;

    // Act
    bool hit = intersect_triangle_moeller_trumbore(ray, vertices[0], vertices[1], vertices[2], t, u, v);

    // Assert
    EXPECT_THAT(hit, false);
    EXPECT_THAT(u, 50.25f);
}
