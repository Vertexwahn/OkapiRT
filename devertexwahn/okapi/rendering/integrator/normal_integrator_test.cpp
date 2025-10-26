/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/scene/load_scene.hpp"
#include "okapi/rendering/sensor/sensor.hpp"
#include "okapi/rendering/integrator/normal_integrator.hpp"

#include "gmock/gmock.h"

using namespace de_vertexwahn;

TEST(NormalIntegrator, Test1) {
    // Arrange
    auto scene = load_scene3f("okapi/scenes/sphere/sphere.normal.okapi.xml");
    ASSERT_TRUE(scene);
    ASSERT_THAT(scene->shape_count(), 1u);

	const auto& film = scene->sensor()->film();
    auto canvas = make_reference_counted<SvgCanvas2f>(film->size());
    auto integrator = scene->integrator();
    ASSERT_TRUE(integrator);

    // Act
	Point2f sample_position{film->width() / 2.f, film->height() / 2.f};
	auto sensor = scene->sensor();
	Ray3f ray = sensor->generate_ray(sample_position);
    ColorRGB3f color = integrator->trace(scene.get(), scene->sampler().get(), ray, 1);

    // Assert
	float abs_error = .01f;
    EXPECT_THAT(color.red(), testing::FloatNear(0.f, abs_error));
    EXPECT_THAT(color.green(), testing::FloatNear(0.f, abs_error));
    EXPECT_THAT(color.blue(), testing::FloatNear(1.f, abs_error));
}


class MockNormalIntersector : public IntersectorType<float, 3> {
public:
    MockNormalIntersector() {}

    void build_acceleration_structure(std::vector<ReferenceCounted<Shape>> shapes) override {}

    bool intersect(const Ray &ray, MediumEvent &me) const override {
        me.sh_frame.n = Normal3f{1.f, 0.f, 0.f};
        return true;
    }
};

TEST(NormalIntegrator, AbsNormals) {
    PropertySet ps{};
    ps.add_property("abs_normals", false);
    NormalIntegrator3f integrator{ps};

    Scene3f scene;
    auto intersector = make_reference_counted<MockNormalIntersector>();
    scene.set_intersector(intersector);

    // Act
    Ray3f ray{Point3f{0.f,0.f,0.f}, Vector3f{0.f,0.f,1.f}, 0.f, 100.f};
    ColorRGB3f final_color = integrator.trace(&scene, nullptr, ray, 0);

    EXPECT_THAT(final_color, ColorRGB3f(1.f, .5f, .5f));
}
