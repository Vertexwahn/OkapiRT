/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "flatland/rendering/scene/load_scene.hpp"
#include "flatland/rendering/shape/medium_event.hpp"
#include "flatland/rendering/shape/shape.hpp"

#include "gmock/gmock.h"

using namespace de_vertexwahn;

TEST(MediumEvent, GivenAShapeWithAMaterial_WhenRayIntersectsDisk_ThenMediumEventWithRedMaterial) {
    // Arrange
    auto scene = load_scene2f("flatland/scenes/disk.flatland.xml");

    // Act
    Ray2f ray = scene->sensor()->generate_ray(Point2f{0.f, 0.f});
    MediumEvent2f me;
    bool hit = scene->intersect(ray, me);

    EXPECT_TRUE(hit);

    ASSERT_NE(me.shape, nullptr);
    //float index_of_refraction = me.shape->bsdf()->refraction_index();

    // Assert
    //EXPECT_THAT(index_of_refraction, 1.f);
    //EXPECT_THAT(me.shape->bsdf()->interface_interaction_type(), InterfaceInteraction::SpecularTransmission);
}

TEST(MediumEvent, toWorld1) {
    // Arrange
    MediumEvent2f me;
    me.geo_frame.t = Vector2f{1.f, 0.f};
    me.geo_frame.n = Vector2f{0.f, 1.f};

    Vector2f v{1.f, 1.f};

    // Act
    Vector2f vt = me.geo_frame.to_world(v);

    // Assert
    EXPECT_THAT(vt.x(), testing::FloatEq(1.f));
    EXPECT_THAT(vt.y(), testing::FloatEq(1.f));
}

TEST(MediumEvent, toWorld2) {
    // Arrange
    MediumEvent2f me;
    me.geo_frame.t = Vector2f{0.f, 1.f};
    me.geo_frame.n = Vector2f{-1.f, 0.f};

    Vector2f v{1.f, 1.f};

    // Act
    Vector2f vt = me.geo_frame.to_world(v);

    // Assert
    EXPECT_THAT(vt.x(), testing::FloatEq(-1.f));
    EXPECT_THAT(vt.y(), testing::FloatEq(1.f));
}
