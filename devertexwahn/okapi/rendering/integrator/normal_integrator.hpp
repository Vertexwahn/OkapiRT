/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef Okapi_NormalIntegrator_6535510e_7052_4770_ac7a_20366e169595_h
#define Okapi_NormalIntegrator_6535510e_7052_4770_ac7a_20366e169595_h

#include "flatland/rendering/canvas/svg_canvas.hpp"
#include "core/logging.hpp"
#include "core/namespace.hpp"
#include "core/object.hpp"
#include "imaging/color.hpp"
#include "flatland/rendering/integrator/integrator.hpp"
#include "flatland/rendering/sampler.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

template <typename ScalarType>
class NormalIntegrator : public IntegratorType<ScalarType, 3> {
public:
	using Base = IntegratorType<ScalarType, 3>;
	using Color = ColorTypeRGB<ScalarType, 3>;
	using Scalar = ScalarType;
	using Vector = VectorType<ScalarType, 3>;
    using Normal = NormalType<ScalarType, 3>;
	using Point = PointType<ScalarType, 3>;
	using Ray = RayType<ScalarType, 3>;
	using MediumEvent = MediumEventType<ScalarType, 3>;
	using Scene = SceneType<ScalarType, 3>;
    using Sampler = SamplerType<ScalarType>;

	NormalIntegrator(const PropertySet &ps) : IntegratorType<ScalarType, 3>(ps) {
        abs_normals_ = ps.get_property<bool>("abs_normals", true);
	}

    ColorRGB<ScalarType> trace(
      const Scene *scene,
      Sampler* sampler,
      Ray &ray,
      const int depth,
      ScalarType *aovs = nullptr) const override {
        MediumEvent me;

        if (scene->intersect(ray, me)) {
            Normal normal = me.sh_frame.n; // world space normal

            if(abs_normals_) {
                return Color{std::abs(normal.x()),
                             std::abs(normal.y()),
                             std::abs(normal.z())};
            }
            else {
                return Color{Scalar{0.5} + normal.x() * Scalar{0.5},
                             Scalar{0.5} + normal.y() * Scalar{0.5},
                             Scalar{0.5} + normal.z() * Scalar{0.5}};
            }
        } else {
			return background_color_;
        }
	}

private:
    bool abs_normals_ = true;
    Color background_color_{Scalar{.0}, Scalar{.0}, Scalar{.0}};
};

using NormalIntegrator3f = NormalIntegrator<float>;
using NormalIntegrator3d = NormalIntegrator<double>;

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define Okapi_NormalIntegrator_6535510e_7052_4770_ac7a_20366e169595_h
