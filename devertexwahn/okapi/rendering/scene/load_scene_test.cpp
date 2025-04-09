/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "flatland/rendering/scene/load_scene.hpp"
#include "flatland/rendering/integrator/path_mirror_reflection.hpp"
#include "okapi/rendering/integrator/ambient_occlusion.h"
#include "okapi/rendering/scene/load_scene.h"
#include "okapi/rendering/shape/sphere.h"
#include "okapi/rendering/sensor/sensor.hpp"

#include "pugixml.hpp"

#include "gmock/gmock.h"

#include <filesystem>

using namespace de_vertexwahn;

// Test Scene details
DE_VERTEXWAHN_BEGIN_NAMESPACE
    PropertySet read_all_properties(const pugi::xml_node &node);
DE_VERTEXWAHN_END_NAMESPACE

TEST(load_scene2f, GivenFileWithPathMirrorReflection_WhenLoadingScene_ThenExpectValidPathMirrorReflectionIntegrator) {
    // Act
    auto scene = load_scene2f("flatland/scenes/mirror.flatland.xml");
    ASSERT_TRUE(scene);
    auto integrator = scene->integrator();
    ASSERT_TRUE(integrator);

    // Assert
    ReferenceCounted<PathMirrorReflection2f> pst = std::dynamic_pointer_cast<PathMirrorReflection2f>(integrator);
    EXPECT_TRUE(pst.get());
}

TEST(load_scene3f, GivenFileWithSphere_WhenLoadingScene_ThenExpectValidSphere) {
    // Act
    auto scene = load_scene3f("okapi/scenes/sphere/sphere.ao.okapi.xml");
    ASSERT_TRUE(scene);
    EXPECT_THAT(scene->shape_count(), 1u);

    ReferenceCounted<Sphere3f> sphere = std::dynamic_pointer_cast<Sphere3f>(scene->shapes()[0]);

    Matrix44f expected_matrix;
    expected_matrix << 1.f,0.f,0.f,0.f,
            0.f,1.f,0.f,0.f,
            0.f,0.f,1.f,140.f,
            0.f,0.f,0.f,1.f;

    EXPECT_TRUE(sphere->transform().matrix().isApprox(expected_matrix));
}

TEST(load_scene2f, GivenFileWithAmbientOcclusionIntegrator_WhenLoadingScene_ThenExpectValidAmbientOcclusionIntegrator) {
    // Act
    auto scene = load_scene2f("flatland/scenes/ao.flatland.xml");
    ASSERT_TRUE(scene);
    auto integrator = scene->integrator();
    ASSERT_TRUE(integrator);

    // Assert
    ReferenceCounted<AmbientOcclusion2f> ao = std::dynamic_pointer_cast<AmbientOcclusion2f>(integrator);
    EXPECT_TRUE(ao.get());
}

TEST(load_scene2f, ReadProperties) {
    const auto old_locale=setlocale(LC_NUMERIC, nullptr);

    // force '.' as the radix point
    setlocale(LC_NUMERIC,"C");

    // Arrange
    pugi::xml_document doc;
    auto result = doc.load_file("flatland/tests/scenes/property.test.flatland.xml");
    auto xml_shape = doc.child("scene").child("shape");

    // Act
    auto ps = read_all_properties(xml_shape);

    // Assert
    EXPECT_FLOAT_EQ(ps.get_property<int>("int_property"), 42);
    EXPECT_FLOAT_EQ(ps.get_property<float>("float_property"), 1.f);
    EXPECT_THAT(ps.get_property<bool>("bool_property"), true);
    EXPECT_THAT(ps.get_property<std::string>("string_property"), "This is a string");
    EXPECT_EQ(ps.get_property<Point2f>("point_property"), Point2f(3.f, 4.f));
    EXPECT_EQ(ps.get_property<Vector2f>("vector_property"), Vector2f(1.f, 2.f));
    EXPECT_EQ(ps.get_property<ColorRGB3f>("color_property"), ColorRGB3f(0.f, 0.682f, 0.937f));
    EXPECT_TRUE(result);

    // restore locale setting
    setlocale(LC_NUMERIC, old_locale);
}

TEST(load_scene2f, ReadTransform) {
    // Arrange
    pugi::xml_document doc;
    auto result = doc.load_file("flatland/scenes/disk.flatland.xml");
    auto xmlShape = doc.child("scene").child("shape");

    // Act
    auto transform = read_transform<2>(xmlShape.child("transform"));

    // Assert
    EXPECT_THAT(transform.matrix()(0, 3), 400.f);
    EXPECT_TRUE(result);
}

TEST(load_scene2f, ReadSensorTransform) {
    // Arrange
    pugi::xml_document doc;
    auto result = doc.load_file("flatland/scenes/rectangle.flatland.xml");
    auto xml_sensor = doc.child("scene").child("sensor");

    // Act
    auto transform = read_transform<2>(xml_sensor.child("transform"));

    auto p = transform * Point2f{100.f, 600.f};
    EXPECT_NEAR(p.x(), 0.f, 0.0001f);
    EXPECT_NEAR(p.y(), 0.f, 0.0001f);

    // Assert
    //EXPECT_THAT(transform.getMatrix()(0,3), 100.f);
    EXPECT_TRUE(result);
}

TEST(load_scene2f, ReadPolygonTransform) {
    // Arrange
    pugi::xml_document doc;
    auto result = doc.load_file("flatland/scenes/bunny/bunny.flatland.xml");
    auto xml_shape = doc.child("scene").child("shape");

    // Act
    auto transform = read_transform<2>(xml_shape.child("transform"));

    Matrix44f scaling;
    scaling <<  3.f, 0.f, 0.f, 0.f,
            0.f, 3.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f;

    Matrix44f translation;
    translation <<  1.f, 0.f, 0.f, 400.f,
            0.f, 1.f, 0.f, 300.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f;

    Matrix44f expected_result = translation * scaling;

    // Assert
    EXPECT_THAT(transform.matrix(), expected_result);
    EXPECT_TRUE(result);
}

TEST(load_scene2f, ReadMaterial) {
    // https://stackoverflow.com/questions/12316972/how-to-tweak-stdstod-string-to-double-for-decimal-separator-and-number-of-di
    // save locale setting
    const auto old_locale = setlocale(LC_NUMERIC, nullptr);

    // force '.' as the radix point
    setlocale(LC_NUMERIC,"C");

    // Arrange
    pugi::xml_document doc;
    auto result = doc.load_file("flatland/scenes/disk.flatland.xml");
    auto xml_shape = doc.child("scene").child("shape");

    // Act
    auto material = read_all_properties(xml_shape.child("bsdf"));

    // Assert
    EXPECT_THAT(material.get_property<ColorRGB3f>("stroke_color"), (ColorRGB3f{0.0, 0.682, 0.937}));
    EXPECT_THAT(material.get_property<float>("stroke_width"), 3);
    EXPECT_THAT(material.get_property<ColorRGB3f>("fill_color"), (ColorRGB3f{1.f, 1.f, 1.f}));
    EXPECT_TRUE(result);

    // restore locale setting
    setlocale(LC_NUMERIC, old_locale);
}

TEST(load_scene3f, WhenSceneDoesNotExist_ThenReturnNull) {
    auto scene = load_scene3f("does_not_exist.okapi.xml");
    EXPECT_THAT(scene, nullptr);
}

TEST(load_scene3f, override_samples_per_pixel) {
    PropertySet override_scene_properties;
    override_scene_properties.add_property("samples_per_pixel", 42);
    auto scene = load_scene3f("okapi/scenes/bunny/bunny.normal.okapi.xml", override_scene_properties);
    EXPECT_THAT(scene->sampler()->sample_count(), 42);
}

TEST(load_scene3f, LoadStanfordBunnyScene) {
    // Act
    auto scene = load_scene3f("okapi/scenes/bunny/bunny.normal.okapi.xml");

    // Assert
	ASSERT_TRUE(scene);

	ASSERT_THAT(scene->shape_count(), 1u);

	auto look_at_matrix = scene->sensor()->transform().matrix();

	Matrix44f expected_matrix;
	expected_matrix <<  0.999662f, -0.0127234f, -0.0226716f,  0.0185006f,
	                   0.0175928f,   0.973127f,   0.229598f,  -0.107506f,
                       0.0191411f,   -0.22992f,   0.973021f,   0.779225f,
                       0.f,          0.f,          0.f,          1.f;

    EXPECT_TRUE(look_at_matrix.isApprox(expected_matrix));
}

TEST(load_scene3f, GivenSphereWithoutTransform_ExpectIdentiyTransform) {
    auto scene = load_scene3f("okapi/scenes/bunny/bunny.ao.okapi.xml");

	ASSERT_TRUE(scene);
    ASSERT_THAT(scene->shape_count(), 1u);
	EXPECT_THAT(scene->shapes()[0]->transform(), identity<float>());
	EXPECT_THAT(scene->sampler()->sample_count(), 10);
}

TEST(load_scene3f, GivenAjax_ExpectCorrectSampleCount) {
    auto scene = load_scene3f("okapi/scenes/ajax/ajax.ao.okapi.xml");

    ASSERT_TRUE(scene);
    EXPECT_THAT(scene->sampler()->sample_count(), 512);
}

TEST(load_scene3f, GivenAjax_ExpectCorrectShapeTransform) {
    auto scene = load_scene3f("okapi/scenes/ajax/ajax.ao.okapi.xml");
    ASSERT_TRUE(scene);
    ASSERT_TRUE(scene->shapes().size() == 2u);

    {
        auto ajax_transform_matrix = scene->shapes()[0]->transform().matrix();
        EXPECT_TRUE(ajax_transform_matrix.isApprox(identity<float>().matrix()));
    }

    {
        auto plane_transform_matrix = scene->shapes()[1]->transform().matrix();
        std::cout << plane_transform_matrix << std::endl;
        EXPECT_TRUE(plane_transform_matrix.isApprox(scale<float>(100.f, 1.f, 100.f).matrix()));
    }
}

TEST(load_scene3f, GivenFileWithTentFilter_WhenLoadingScene_ThenTentValidFilter) {
    // Act
    auto scene = load_scene3f("okapi/scenes/sphere/sphere.hit.okapi.xml");
    ASSERT_TRUE(scene);

    auto film = scene->sensor()->film();

    auto filter = film->filter();

    // Assert
    EXPECT_THAT(filter->to_string(), "TentFilter");
    EXPECT_THAT(filter->radius(), (Vector2f{1.f, 1.f}));
}

TEST(load_scene3f, load_cornell_box) {
    // Act
    auto scene = load_scene3f("okapi/scenes/cornell_box/cornell_box.okapi.xml");
    ASSERT_TRUE(scene);

    EXPECT_THAT(scene->shapes()[0]->bsdf(), nullptr);
    EXPECT_THAT(scene->shapes()[0]->is_emitter(), true);
    auto emitter_color = scene->shapes()[0]->emitter()->evaluate();
    EXPECT_THAT(emitter_color.red(), testing::FloatNear(18.387f, .001f));
    EXPECT_THAT(emitter_color.green(), testing::FloatNear(10.9873f, .001f));
    EXPECT_THAT(emitter_color.blue(), testing::FloatNear(2.75357f, .001f));

    ASSERT_TRUE(scene->shapes()[1]->bsdf());
    Vector3f wi, wo;
    BSDFSample3f bsdf_sample;
    bsdf_sample.wi = Vector3f{0.f, 0.f, 1.f};
    bsdf_sample.wo = Vector3f{0.f, 0.f, 1.f};
    ColorRGB3f color = scene->shapes()[1]->bsdf()->evaluate(bsdf_sample);

    float abs_error = .01f;
    EXPECT_THAT(color.red(), testing::FloatNear(0.230775f, abs_error));
    EXPECT_THAT(color.green(), testing::FloatNear(0.226f, abs_error));
    EXPECT_THAT(color.blue(), testing::FloatNear(0.216451f, abs_error));
}

TEST(load_scene3f, GivenFileWithObjThatDoesNotExist_WhenLoadingScene_ThenExcpetionWithUsfullErrorMessage) {
    EXPECT_THROW(load_scene3f("okapi/tests/scenes/ReferencingNonExistingObj.ao.okapi.xml"), std::runtime_error);
}

TEST(load_scene3f, GivenFileWithSimpleIntegraterWithMissingAttribute_WhenLoadingScene_ThenExcpetionWithUsfullErrorMessage) {
    EXPECT_THAT([]() { load_scene3f("okapi/scenes/sphere/sphere.simple.missing_position_attribute.okapi.xml"); },
                ThrowsMessage<PropertyDoesNotExistException>(
                        testing::HasSubstr("Property with name 'position' does not exist")
                )
    );
}
