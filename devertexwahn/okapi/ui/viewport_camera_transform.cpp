/*
 *  SPDX-FileCopyrightText: Copyright 2025 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "viewport_camera_transform.hpp"

#include "math/quaternion.hpp"

using namespace de_vertexwahn;

DE_VERTEXWAHN_BEGIN_NAMESPACE

Point3f compute_eye_point(const Matrix44f& scene_senor_world_to_camera) {
    // note do not change this to "auto eye = "
    // gives an error with
    // bazel test --config=gcc13 --compilation_mode=opt --cache_test_results=no //okapi/ui:viewport_test
    // TODO(Vertexwahn): Find out why
    Point4f eye = inverse_matrix<float>(scene_senor_world_to_camera) * Point4f{0.f, 0.f, 0.f, 1.0f};
    return Point3f{eye.topLeftCorner<3,1>()};
}

// there seems to be a problem with eulerAngles
// it goes always for minimized angles -> this
// give maybe the behavior that sometime up and down is inverted
// https://stackoverflow.com/questions/33895970/about-eulerangles-conversion-from-eigen-c-library
// https://gamedev.stackexchange.com/questions/50963/how-to-extract-euler-angles-from-transformation-matrix
// maybe change CameraTranformation to make use of quaternions instead of euler angles would solve some issues...
// https://gamedev.stackexchange.com/questions/50963/how-to-extract-euler-angles-from-transformation-matrix
// https://stackoverflow.com/questions/1996957/conversion-euler-to-matrix-and-matrix-to-euler
// https://www.ogldev.org/www/tutorial13/tutorial13.html
CameraTransformation compute_camera_transform(const Matrix44f& scene_senor_world_to_camera) {
    auto eye = compute_eye_point(scene_senor_world_to_camera);

    Matrix33f rot_part{scene_senor_world_to_camera.inverse().topLeftCorner<3, 3>()};

    auto euler = rot_part.eulerAngles(1, 0, 2);

    CameraTransformation tf;
    tf.x() = eye.x();
    tf.y() = eye.y();
    tf.z() = eye.z();

    tf.yaw() = euler.x();// + pi;
    tf.pitch() = euler.y();// + pi;
    tf.roll() = euler.z();// + pi;

    return tf;
}

DE_VERTEXWAHN_END_NAMESPACE
