/*
 *  SPDX-FileCopyrightText: Copyright 2025 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/ui/camera.hpp"
#include "okapi/ui/viewport_camera_transform.hpp"
#include "okapi/rendering/scene/load_scene.h"
#include "math/quaternion.hpp"

#include "gmock/gmock.h"

using namespace de_vertexwahn;

::testing::AssertionResult Matrix44fNear(const Matrix44f& given, const Matrix44f& expected, float delta) {
    bool within_tolerance = true;
    std::vector<std::pair<int, float>> outsiders;

    for (int index = 0; index < 16; ++index) {
        float diff = std::abs(given(index) - expected(index));
        if (diff > delta) {
            within_tolerance = false;
            outsiders.push_back(std::make_pair(index, diff));
        }
    }

    if (within_tolerance) {
        return ::testing::AssertionSuccess();
    }
    else {
        return ::testing::AssertionFailure() << "Matrix values differ by more than " << delta << std::endl
                                               << "Index " << outsiders[0].first
                                               << " given " << given(outsiders[0].first)
                                               << " expected " << expected(outsiders[0].first);
    }
}
/*
// This test serves to get a basic understanding of the underlying sensor space and the world to sensor space transformation
TEST(Viewport, SensorSpaceStart) {
    auto scene = load_scene3f("okapi/scenes/cornell_box_with_spheres/cornell_box_with_spheres.next.okapi.xml");
    ASSERT_TRUE(scene);

    const auto& scene_camera_transform = scene->sensor()->transform().matrix();

    // these are the values expected from scene file
    EXPECT_NEAR(scene_camera_transform(12), 278.f, 0.01f);
    EXPECT_NEAR(scene_camera_transform(13), -273.f, 0.01f);
    EXPECT_NEAR(scene_camera_transform(14), 800.f, 0.01f);

    Point4f scene_camera_eye_world_space{278.f, 273.f, 800.f, 1.f};
    auto scene_camera_eye_sensor_space = scene_camera_transform * scene_camera_eye_world_space;
    EXPECT_THAT(scene_camera_eye_sensor_space, Point4f(0.f,0.f,0.f, 1.f));

    Point4f origin_world_space{0.f, 0.f, 0.f, 1.f};
    auto origin_scene_space = scene_camera_transform * origin_world_space;
    EXPECT_THAT(origin_scene_space, Point4f(278.f,-273.f,800.f, 1.f));

    Matrix44f expected;
    expected << -1.f,    0.f,    0.f,  278.f,
                 0.f,    1.f,    0.f, -273.f,
                 0.f,    0.f,   -1.f,  800.f,
                 0.f,    0.f,    0.f,    1.f;

    EXPECT_TRUE(Matrix44fNear(scene_camera_transform, expected, 0.1f));
}

TEST(Viewport, SensorSpace_MoveOneUnitForward) {
    auto scene = load_scene3f("okapi/scenes/cornell_box_with_spheres/cornell_box_with_spheres.next.okapi.xml");
    ASSERT_TRUE(scene);

    Point3f eye{278.f, 273.f, 800.f-1.f};
    Point3f target{278.f, 273.f, 799.f-1.f};
    scene->sensor()->set_transform(look_at(eye, target, Vector3f{0.f, 1.f, 0.f}));
    const auto& scene_camera_transform = scene->sensor()->transform().matrix();

    // these are the values expected from scene file
    EXPECT_NEAR(scene_camera_transform(12), 278.f, 0.01f);
    EXPECT_NEAR(scene_camera_transform(13), -273.f, 0.01f);
    EXPECT_NEAR(scene_camera_transform(14), 800.f-1.f, 0.01f);

    Point4f scene_camera_eye_world_space{278.f, 273.f, 800.f-1.f, 1.f};
    auto scene_camera_eye_sensor_space = scene_camera_transform * scene_camera_eye_world_space;
    EXPECT_THAT(scene_camera_eye_sensor_space, Point4f(0.f,0.f,0.f, 1.f));

    Point4f origin_world_space{0.f, 0.f, 0.f, 1.f};
    auto origin_scene_space = scene_camera_transform * origin_world_space;
    EXPECT_THAT(origin_scene_space, Point4f(278.f,-273.f,800.f-1.f, 1.f));

    Matrix44f expected;
    expected << -1.f,    0.f,    0.f,  278.f,
                 0.f,    1.f,    0.f, -273.f,
                 0.f,    0.f,   -1.f,  800.f-1.f,
                 0.f,    0.f,    0.f,    1.f;

    EXPECT_TRUE(Matrix44fNear(scene_camera_transform, expected, 0.1f));
}
*/
// This test serves to get a basic understanding of the underlying camara controller space
TEST(Viewport, CameraControllerSpace_Start) {
    auto camera_ = make_reference_counted<Camera>();
    camera_->frustum() = CameraFrustum(2.f, 2.f, 0.1f, 1000.f, eProjectionType::Perspective);
    camera_->transformation().offset() = 0;
    camera_->transformation().yaw() = pi_over_4f;

    auto camera_controller = make_reference_counted<CameraController>(camera_);
    camera_controller->set_state(CameraController::eState::Free);
    camera_controller->handle_wheel(10.0f);

    Vector3f eye{278.f, 273.f, 800.f};
    Vector3f target{278.f, 273.f, 799.f};
    camera_controller->camera()->transformation().look_at(target, eye);

    Matrix44f expected;
    expected << -1.f,    0.f,    0.f,  278.f,
                 0.f,    1.f,    0.f, -273.f,
                 0.f,    0.f,   -1.f,  800.f,
                 0.f,    0.f,    0.f,    1.f;

    EXPECT_TRUE(Matrix44fNear(camera_controller->camera()->transformation().view_matrix(), expected, 0.1f));
}

TEST(Viewport, CameraControllerSpace_MoveOneUnitForward) {
    auto camera_ = make_reference_counted<Camera>();
    camera_->frustum() = CameraFrustum(2.f, 2.f, 0.1f, 1000.f, eProjectionType::Perspective);
    camera_->transformation().offset() = 0;
    camera_->transformation().yaw() = pi_over_4f;

    auto camera_controller = make_reference_counted<CameraController>(camera_);
    camera_controller->set_state(CameraController::eState::Free);
    camera_controller->handle_wheel(10.0f);

    Vector3f eye{278.f, 273.f, 800.f};
    Vector3f target{278.f, 273.f, 799.f};
    camera_controller->camera()->transformation().look_at(target, eye);

    // Move forward
    camera_controller->handleKeyDown(CameraController::eKey::MoveForward);
    float delta = 0.189492956f;
    camera_controller->tick(delta);
    camera_->tick(delta);
    camera_controller->handleKeyUp(CameraController::eKey::MoveForward);

    Matrix44f expected;
    expected << -1.f,    0.f,    0.f,  278.f,
                 0.f,    1.f,    0.f, -273.f,
                 0.f,    0.f,   -1.f,  790.525f,
                 0.f,    0.f,    0.f,    1.f;

    EXPECT_TRUE(Matrix44fNear(camera_controller->camera()->transformation().view_matrix(), expected, 0.1f));
}

TEST(CameraTransformation, Identity) {
    Matrix44f m = identity_matrix<float>();
    CameraTransformation ct = compute_camera_transform(m);

    Matrix44f expected;
    expected <<  1.f,    0.f,    0.f,    0.f,
                 0.f,    1.f,    0.f,    0.f,
                 0.f,    0.f,    1.f,    0.f,
                 0.f,    0.f,    0.f,    1.f;

    EXPECT_TRUE(Matrix44fNear(ct.view_matrix(), expected, 0.1f));
}

TEST(CameraTransformation, GivenTranslationWhenViewMatrixRequestedThenExpectCorrectValues) {
    /// Arrange
    Matrix44f m = translation_matrix(-1.f, -2.f, -3.f);

    // Expect correct translation matrix
    {
        EXPECT_NEAR(m(0), 1.f, 0.01f);
        EXPECT_NEAR(m(1), 0.f, 0.01f);
        EXPECT_NEAR(m(2), 0.f, 0.01f);
        EXPECT_NEAR(m(3), 0.f, 0.01f);

        EXPECT_NEAR(m(4), 0.f, 0.01f);
        EXPECT_NEAR(m(5), 1.f, 0.01f);
        EXPECT_NEAR(m(6), 0.f, 0.01f);
        EXPECT_NEAR(m(7), 0.f, 0.01f);

        EXPECT_NEAR(m(8), 0.f, 0.01f);
        EXPECT_NEAR(m(9), 0.f, 0.01f);
        EXPECT_NEAR(m(10), 1.f, 0.01f);
        EXPECT_NEAR(m(11), 0.f, 0.01f);

        EXPECT_NEAR(m(12), -1.f, 0.01f);
        EXPECT_NEAR(m(13), -2.f, 0.01f);
        EXPECT_NEAR(m(14), -3.f, 0.01f);
        EXPECT_NEAR(m(15), 1.f, 0.01f);
    }

    CameraTransformation ct = compute_camera_transform(m);

    // Expect correct camera transform
    {
        EXPECT_NEAR(ct.translation().x(), 1.f, 0.01f);
        EXPECT_NEAR(ct.translation().y(), 2.f, 0.01f);
        EXPECT_NEAR(ct.translation().z(), 3.f, 0.01f);
        EXPECT_NEAR(ct.pitch(), 0.f, 0.01f);
        EXPECT_NEAR(ct.roll(), 0.f, 0.01f);
        EXPECT_NEAR(ct.yaw(), 0.f, 0.01f);
        EXPECT_NEAR(ct.offset(), 0.f, 0.01f);
    }

    /// Act & Assert
    Matrix44f expected;
    expected <<  1.f,    0.f,    0.f,    -1.f,
                 0.f,    1.f,    0.f,    -2.f,
                 0.f,    0.f,    1.f,    -3.f,
                 0.f,    0.f,    0.f,    1.f;

    EXPECT_TRUE(Matrix44fNear(ct.view_matrix(), expected, 0.1f));

    // Note: Cannot be "auto result = ..." since this will yield something wired 
    // TODO(Vertexwahn): Find out what and why this happens only in opt mode with VS2022
    // maybe uninitialized memory in the wired result type?
    // bazel --output_base=G:/bazel_output_base test --config=vs2022 --compilation_mode=opt --cache_test_results=no --runs_per_test=20 //okapi/ui:viewport_test
    Point4f result = ct.view_matrix() * Point4f{1.f, 2.f, 3.f, 1.f};
    EXPECT_NEAR(result.x(), 0.f, 0.01f);
    EXPECT_NEAR(result.y(), 0.f, 0.01f);
    EXPECT_NEAR(result.z(), 0.f, 0.01f);
}

// Given CameraTransformation created with standard ctor
// When transformation matrix is computed
// Expect identy matrix
TEST(CameraTransformation, transformation_matrix) {
    CameraTransformation ct;

    Matrix44f expected;
    expected <<
    1.f,    0.f,    0.f,    0.f,
    0.f,    1.f,    0.f,    0.f,
    0.f,    0.f,    1.f,    0.f,
    0.f,    0.f,    0.f,    1.f;

    EXPECT_TRUE(Matrix44fNear(ct.transformation_matrix(), expected, 0.1f));
}

TEST(CameraTransformation, transformation_matrix_translated) {
    CameraTransformation ct;
    ct.x() = 1;
    ct.y() = 2;
    ct.z() = 3;

    Matrix44f expected;
    expected <<
    1.f,    0.f,    0.f,    1.f,
    0.f,    1.f,    0.f,    2.f,
    0.f,    0.f,    1.f,    3.f,
    0.f,    0.f,    0.f,    1.f;

    EXPECT_TRUE(Matrix44fNear(ct.transformation_matrix(), expected, 0.1f));
}

TEST(CameraTransformation, view_matrix) {
    CameraTransformation ct;

    Matrix44f expected;
    expected <<
    1.f,    0.f,    0.f,    0.f,
    0.f,    1.f,    0.f,    0.f,
    0.f,    0.f,    1.f,    0.f,
    0.f,    0.f,    0.f,    1.f;

    EXPECT_TRUE(Matrix44fNear(ct.view_matrix(), expected, 0.1f));
}

TEST(CameraTransformation, view_matrix_translated) {
    CameraTransformation ct;
    ct.x() = 1;
    ct.y() = 2;
    ct.z() = 3;

    Matrix44f expected;
    expected <<
    1.f,    0.f,    0.f,   -1.f,
    0.f,    1.f,    0.f,   -2.f,
    0.f,    0.f,    1.f,   -3.f,
    0.f,    0.f,    0.f,    1.f;

    EXPECT_TRUE(Matrix44fNear(ct.view_matrix(), expected, 0.1f));
}
/*
TEST(Viewport, BunnyScene) {
    auto scene = load_scene3f("okapi/scenes/bunny/bunny.normal.okapi.xml");
    ASSERT_TRUE(scene);
    const auto& scene_camera_transform = scene->sensor()->transform().matrix();
    ASSERT_TRUE(scene_camera_transform.determinant() > 0.f);
    Point3f origin{-0.0315182f, 0.284011f, -0.7331f};

    auto computed_eye_point = compute_eye_point(scene_camera_transform);

    EXPECT_NEAR(computed_eye_point.x(), origin.x(), 0.01f);
    EXPECT_NEAR(computed_eye_point.y(), origin.y(), 0.01f);
    EXPECT_NEAR(computed_eye_point.z(), origin.z(), 0.01f);

    //std::cout << "--------------" << std::endl;
    //std::cout << scene_camera_transform << std::endl;

    auto camera_ = de_vertexwahn::make_reference_counted<Camera>();
    camera_->frustum() = CameraFrustum(640, 480, 0.1f, 1000.f, eProjectionType::Perspective);
    camera_->transformation().offset() = 15;
    camera_->transformation().yaw() = pi_over_4f;

    auto camera_controller_ = make_reference_counted<CameraController>(camera_);
    camera_controller_->set_state(CameraController::eState::Free);
    camera_controller_->handle_wheel(10.0f);

    camera_controller_->camera()->transformation() = compute_camera_transform(scene_camera_transform);

    //std::cout << "--------------" << std::endl;
    //std::cout << camera_controller_->camera()->transformation().view_matrix() << std::endl;

    EXPECT_TRUE(Matrix44fNear(camera_controller_->camera()->transformation().view_matrix(), scene_camera_transform, 0.1f));
}

TEST(Viewport, CornellBox) {
    auto scene = load_scene3f("okapi/ui/scenes/cornell_box/cornell_box_wood_texture.next.okapi.xml");
    ASSERT_TRUE(scene);
    const auto& scene_camera_transform = scene->sensor()->transform().matrix();
    ASSERT_TRUE(scene_camera_transform.determinant() > 0.f);
    Point3f origin{278.f, 273.f, 800.f};

    auto computed_eye_point = compute_eye_point(scene_camera_transform);

    EXPECT_NEAR(computed_eye_point.x(), origin.x(), 0.01f);
    EXPECT_NEAR(computed_eye_point.y(), origin.y(), 0.01f);
    EXPECT_NEAR(computed_eye_point.z(), origin.z(), 0.01f);

    //std::cout << "--------------" << std::endl;
    //std::cout << scene_camera_transform << std::endl;

    auto camera_ = de_vertexwahn::make_reference_counted<Camera>();
    camera_->frustum() = CameraFrustum(640, 480, 0.1f, 1000.f, eProjectionType::Perspective);
    camera_->transformation().offset() = 15;
    camera_->transformation().yaw() = pi_over_4f;

    auto camera_controller_ = make_reference_counted<CameraController>(camera_);
    camera_controller_->set_state(CameraController::eState::Free);
    camera_controller_->handle_wheel(10.0f);

    camera_controller_->camera()->transformation() = compute_camera_transform(scene_camera_transform);

    //std::cout << "--------------" << std::endl;
    //std::cout << camera_controller_->camera()->transformation().view_matrix() << std::endl;

    EXPECT_TRUE(Matrix44fNear(camera_controller_->camera()->transformation().view_matrix(), scene_camera_transform, 0.1f));
}

TEST(Viewport, KitchenScene) {
    //auto scene = load_scene3f("okapi/scenes/kitchen/kitchen.ao.okapi.xml");
    //ASSERT_TRUE(scene);
    Matrix44f scene_camera_transform;
    scene_camera_transform <<
    0.624102,   0.235455,   0.745022,  0.326188,
    -0.33241,   0.942932, -0.0195438,   -1.72863,
    -0.707107,  -0.235455,   0.666754,    2.37338,
    0,         -0,         -0,          1;
    //const auto& scene_camera_transform = scene->sensor()->transform().matrix();

    ASSERT_TRUE(scene_camera_transform.determinant() > 0.f);

    Point3f origin{0.900047714002f, 2.11200457879f, -1.8592650289f};

    auto computed_eye_point = compute_eye_point(scene_camera_transform);

    EXPECT_NEAR(computed_eye_point.x(), origin.x(), 0.01f);
    EXPECT_NEAR(computed_eye_point.y(), origin.y(), 0.01f);
    EXPECT_NEAR(computed_eye_point.z(), origin.z(), 0.01f);

    //std::cout << "--------------" << std::endl;
    //std::cout << scene_camera_transform << std::endl;

    auto camera_ = de_vertexwahn::make_reference_counted<Camera>();
    camera_->frustum() = CameraFrustum(640, 480, 0.1f, 1000.f, eProjectionType::Perspective);
    camera_->transformation().offset() = 15;
    camera_->transformation().yaw() = pi_over_4f;

    auto camera_controller_ = make_reference_counted<CameraController>(camera_);
    camera_controller_->set_state(CameraController::eState::Free);
    camera_controller_->handle_wheel(10.0f);

    camera_controller_->camera()->transformation() = compute_camera_transform(scene_camera_transform);

    //std::cout << "--------------" << std::endl;
    //std::cout << camera_controller_->camera()->transformation().view_matrix() << std::endl;

    EXPECT_TRUE(Matrix44fNear(camera_controller_->camera()->transformation().view_matrix(), scene_camera_transform, 0.1f));
}
*/
