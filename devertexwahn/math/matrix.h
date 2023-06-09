/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef De_Vertexwahn_Math_Matrix_3b0aa566_7e67_433d_8fb6_075c84a4a70e_h
#define De_Vertexwahn_Math_Matrix_3b0aa566_7e67_433d_8fb6_075c84a4a70e_h

#include "core/namespace.h"

#include "Eigen/Core"
#include "Eigen/Dense"

DE_VERTEXWAHN_BEGIN_NAMESPACE

template <typename ScalarType>
using Matrix22 = Eigen::Matrix<ScalarType, 2, 2>;
template <typename ScalarType>
using Matrix33 = Eigen::Matrix<ScalarType, 3, 3>;
template <typename ScalarType>
using Matrix44 = Eigen::Matrix<ScalarType, 4, 4>;

using Matrix22f = Matrix22<float>;
using Matrix22d = Matrix22<double>;
using Matrix33f = Matrix33<float>;
using Matrix33d = Matrix33<double>;
using Matrix44f = Matrix44<float>;
using Matrix44d = Matrix44<double>;

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define De_Vertexwahn_Math_Matrix_3b0aa566_7e67_433d_8fb6_075c84a4a70e_h