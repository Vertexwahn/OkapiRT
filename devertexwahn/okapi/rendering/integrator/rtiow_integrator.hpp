/*
 *  SPDX-FileCopyrightText: Copyright 2025 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef De_Vertexwahn_Okapi_RtiowIntegrator_c701459e_0c0c_431b_a078_706fd32d55a8_hpp
#define De_Vertexwahn_Okapi_RtiowIntegrator_c701459e_0c0c_431b_a078_706fd32d55a8_hpp

#include "flatland/rendering/integrator/integrator.hpp"
#include "flatland/rendering/integrator/integrator.hpp"
#include "flatland/rendering/sampler.hpp"
#include "flatland/rendering/scene/scene.hpp"

#include "imaging/color.hpp"

#include "math/constants.hpp"

#include "core/logging.hpp"
#include "core/namespace.hpp"
#include "core/object.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

template <typename ScalarType>
class RtiowIntegrator : public IntegratorType<ScalarType, 3> {
public:
    using Base = IntegratorType<ScalarType, 3>;
    using Color = ColorTypeRGB<ScalarType, 3>;
    using Scalar = ScalarType;
    using Vector = VectorType<ScalarType, 3>;
    using Point = PointType<ScalarType, 3>;
    using Ray = RayType<ScalarType, 3>;
    using MediumEvent = MediumEventType<ScalarType, 3>;
    using Sampler = SamplerType<ScalarType>;

    RtiowIntegrator(const PropertySet &ps) : IntegratorType<ScalarType, 3>(ps) {
    }

    virtual ~RtiowIntegrator() = default;

    Color trace(
            const SceneType<ScalarType, 3> *scene,
            Sampler* sampler,
            Ray &ray,
            const int /*depth*/,
            ScalarType *aovs = nullptr) const override {
        return ColorRGB<ScalarType>{1., 0., 0.};
    }
};

using RtiowIntegrator3f = RtiowIntegrator<float>;
using RtiowIntegrator3d = RtiowIntegrator<double>;

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define De_Vertexwahn_Okapi_RtiowIntegrator_c701459e_0c0c_431b_a078_706fd32d55a8_hpp
