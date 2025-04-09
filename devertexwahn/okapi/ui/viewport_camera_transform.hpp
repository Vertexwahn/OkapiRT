/*
 *  SPDX-FileCopyrightText: Copyright 2025 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef De_Vertexwahn_Core_ViewportCameraTransform_ca485f18_17f0_4140_8b99_de52c3caa576_hpp
#define De_Vertexwahn_Core_ViewportCameraTransform_ca485f18_17f0_4140_8b99_de52c3caa576_hpp

#include "core/namespace.hpp"
#include "math/matrix.hpp"
#include "math/point.hpp"
#include "okapi/ui/camera.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

Point3f compute_eye_point(const Matrix44f& scene_senor_world_to_camera);
CameraTransformation compute_camera_transform(const Matrix44f& scene_senor_world_to_camera);
DE_VERTEXWAHN_END_NAMESPACE

#endif // end define De_Vertexwahn_Core_ViewportCameraTransform_ca485f18_17f0_4140_8b99_de52c3caa576_hpp
