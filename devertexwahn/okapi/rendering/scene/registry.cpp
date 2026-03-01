/*
 *  SPDX-FileCopyrightText: Copyright 2026 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/scene/registry.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

// Samplers (from flatland)
void register_ConstantSampler();
void register_IndependentSampler();

// Intersectors
void register_BruteForceIntersector3f();
void register_EmbreeIntersector();

// Integrators
void register_NormalIntegrator3f();

// Shapes
void register_SerializedMesh3f();
void register_Sphere3f();
void register_TriangleMesh3f();

// BSDFs
void register_Diffuse();

// Emitters

// Textures

// Filters
void register_BoxFilter();
void register_GaussianFilter();
void register_TentFilter();

void register_okapi_plugins() {
    // Samplers
    register_ConstantSampler();
    register_IndependentSampler();

    // Intersectors
    register_BruteForceIntersector3f();
    register_EmbreeIntersector();

    // Integrators
    register_NormalIntegrator3f();

    // Shapes
    register_SerializedMesh3f();
    register_Sphere3f();
    register_TriangleMesh3f();

    // BSDFs

    // Emitters

    // Textures

    // Filters
    register_BoxFilter();
    register_GaussianFilter();
    register_TentFilter();
}

DE_VERTEXWAHN_END_NAMESPACE
