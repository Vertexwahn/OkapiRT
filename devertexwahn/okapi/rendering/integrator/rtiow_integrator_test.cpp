/*
*  SPDX-FileCopyrightText: Copyright 2025 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/integrator/rtiow_integrator.hpp"

using namespace de_vertexwahn;

#include "gmock/gmock.h"

using namespace testing;

/*
class MockHitIntersector : public IntersectorType<float, 3> {
public:
    MockHitIntersector() {}

    MOCK_METHOD(void, build_acceleration_structure, (std::vector<ReferenceCounted<Shape>> shapes), (override));
    MOCK_METHOD(bool, intersect, (const Ray &ray, MediumEvent &me), (const, override));
};
*/

TEST(RtiowIntegrator, Test1) {
    PropertySet ps;
    RtiowIntegrator3f integrator{ps};

    /*
    Scene3<float> scene;
    auto intersector = make_reference_counted<MockHitIntersector>();
    scene.set_intersector(intersector);

    EXPECT_CALL(*intersector.get(), intersect(_, _))
           .Times(1)
           .WillOnce(Return(false));

    // Act
    Ray3f ray{Point3f{0,0,0}, Vector3f{0,0,1}, 0.f, 100.f};
    ColorRGB3f final_color = integrator.trace(&scene, nullptr, ray, 1);

    EXPECT_THAT(final_color, ColorRGB3f(0.f, 0.f, 0.f));
    */
}
