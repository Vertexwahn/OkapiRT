/*
 *  SPDX-FileCopyrightText: Copyright 2024-2025 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/ui/camera.hpp"

#include "gmock/gmock.h"

using namespace de_vertexwahn;

TEST(CameraTransformation, basic_opterations) {
    Vector3f translation = Vector3f(1.f, 2.f, 3.f);
    Vector3f rotation = Vector3f(0.f, 0.f, 0.f);
    CameraTransformation camera_transformation{translation, rotation, 0.f};

    EXPECT_EQ(camera_transformation.translation(), Vector3f(1.f, 2.f, 3.f));
    EXPECT_EQ(camera_transformation.rotation(), Vector3f(0.f, 0.f, 0.f));

    camera_transformation.rotation() = Vector3f{0.f, 0.f, 0.f};
    EXPECT_EQ(camera_transformation.rotation(), Vector3f(0.f, 0.f, 0.f));

    EXPECT_EQ(camera_transformation.x(), 1.f);
    EXPECT_EQ(camera_transformation.y(), 2.f);
    EXPECT_EQ(camera_transformation.z(), 3.f);

    camera_transformation.x() = 4.f;
    camera_transformation.y() = 5.f;
    camera_transformation.z() = 6.f;

    EXPECT_EQ(camera_transformation.x(), 4.f);
    EXPECT_EQ(camera_transformation.y(), 5.f);
    EXPECT_EQ(camera_transformation.z(), 6.f);

    camera_transformation.translation() = Vector3f(7.f, 8.f, 9.f);
    EXPECT_EQ(camera_transformation.translation(), Vector3f(7.f, 8.f, 9.f));

    EXPECT_EQ(camera_transformation.yaw(), 0.f);
    EXPECT_EQ(camera_transformation.pitch(), 0.f);
    EXPECT_EQ(camera_transformation.roll(), 0.f);
}


TEST(CameraTransformation, binary_operations) {
    CameraTransformation a{{1.f,2.f,3.f}, {0.f,0.f,0.f}, 0.f};
    CameraTransformation b{{1.f,2.f,3.f}, {0.f,0.f,0.f}, 0.f};

    auto r1 = a + b;
    auto r2 = a - b;
    auto r3 = a * 2.f;
    auto r4 = a / 2.f;

    EXPECT_THAT(r1.translation(), Vector3f(2.f, 4.f, 6.f));
    EXPECT_THAT(r2.translation(), Vector3f(0.f, 0.f, 0.f));
    EXPECT_THAT(r3.translation(), Vector3f(2.f, 4.f, 6.f));
    EXPECT_THAT(r4.translation(), Vector3f(0.5f, 1.f, 1.5f));

    CameraTransformation r5{{1.f,2.f,3.f}, {0.f,0.f,0.f}, 0.f};
    CameraTransformation r6{{1.f,2.f,3.f}, {0.f,0.f,0.f}, 0.f};

    r5 += r6;

    EXPECT_THAT(r5.translation(), Vector3f(2.f, 4.f, 6.f));

    r5 -= r6;

    EXPECT_THAT(r5.translation(), Vector3f(1.f, 2.f, 3.f));

    r5 *= 2.f;

    EXPECT_THAT(r5.translation(), Vector3f(2.f, 4.f, 6.f));

    r5 /= 2.f;

    EXPECT_THAT(r5.translation(), Vector3f(1.f, 2.f, 3.f));
}

TEST(CameraTransformation, lookAt) {
    CameraTransformation ct{{1.f,2.f,3.f}, {0.f,0.f,0.f}, 0.f};
    ct.look_at({0.f, 0.f, 0.f}, {0.f, 1.f, 0.f});

    EXPECT_THAT(ct.translation(), Vector3f(0.f, 0.f, 0.f));
}

TEST(CameraTransformation, length) {
    CameraTransformation ct{{1.f,2.f,3.f}, {0.f,0.f,0.f}, 0.f};

    EXPECT_THAT(ct.length(), 3.7416575f);
}

// Fails on Windows
/*
TEST(CameraFrustum, ttt) {
    CameraFrustum frustum(640, 480, 0.1f, 1000.f, eProjectionType::Perspective);

    EXPECT_THAT(frustum.depth(), 999.9f);   
    EXPECT_THAT(frustum.width(), 640.0f);
    EXPECT_THAT(frustum.height(), 480.0f);

    frustum.width(650.0f);
    frustum.height(490.0f);

    EXPECT_THAT(frustum.width(), 650.0f);
    EXPECT_THAT(frustum.height(), 490.0f);
   
}
*/

TEST(Camera, test2) {
    CameraFrustum frustum2(-1.f, 1.f, -1.f, 1.f, 0.1f, 1000.f, eProjectionType::Orthographic);
    EXPECT_THAT(frustum2.width(), 2.f);
}

TEST(CameraController, ctor) {
    CameraController cc;

    cc.set_view_direction({0.f, 0.f, 1.f});

    EXPECT_THAT(cc.get_view_direction_vector(eViewDirection::Top), Vector3f(0.f, 1.f, 0.f));
}

TEST(CameraController, basic_ops) {
    CameraController cc;

    EXPECT_NE(cc.camera(), nullptr);
    EXPECT_NE(cc.is_camera_moving(), true);
}

TEST(CameraController, fitToView) {
    CameraController cc;
    cc.fit_to_view({0.f, 0.f, 0.f}, {1.f, 1.f, 1.f});
}

TEST(CameraController, moveCameraGlobal) {
    Camera camera;
    camera.move_camera_global({1.f, 2.f, 3.f});

    EXPECT_THAT(camera.transformation().translation(), Vector3f(1.f, 2.f, 3.f));

    camera.move_camera_local({1.f, 2.f, 3.f});

    EXPECT_THAT(camera.transformation().translation(), Vector3f(2.f, 4.f, 6.f));

    EXPECT_THAT(camera.frustum().width(), 16.f);
}

TEST(CameraController, handleKeyDown) {
    CameraController cc;
    cc.handleKeyDown(CameraController::eKey::MoveDown);

    EXPECT_THAT(cc.camera()->transformation().translation(), Vector3f(0.f, 0.f, 0.f));
}

// fake test
TEST(CameraController, panCameraWorld) {
    Camera camera;
    camera.pan_camera_world(Vector2f{0.f, 0.f}, Vector3f{0.f, 0.f, 0.f});
}
