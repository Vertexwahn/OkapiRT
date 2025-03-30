/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef Okapi_Sphere_e1052595_6050_4a7e_aad5_4d57c2f2bb62_h
#define Okapi_Sphere_e1052595_6050_4a7e_aad5_4d57c2f2bb62_h

#include "flatland/rendering/shape/shape.hpp"

#include "math/frame.hpp"
#include "math/geometry.hpp"
#include "math/intersection.hpp"
#include "math/transform.hpp"
#include "math/util.hpp"

#include "core/namespace.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

template <typename ScalarType>
class Sphere : public Shape3<ScalarType> {
public:
    using Point = Point3<ScalarType>;
    using Vector = Vector3<ScalarType>;
    using Normal = Normal3<ScalarType>;
    using Frame = Frame3<ScalarType>;
    using Scalar = ScalarType;
    using Ray = RayType<ScalarType, 3>;

    Sphere(const PropertySet& ps) : Shape3<Scalar>(ps) {
        radius_ = ps.get_property<Scalar>("radius");
    }
    virtual ~Sphere() {
    }

    // it is assumed that the direction of the ray is normalized
    // this will not be explicitly checked in release mode and will lead to false unreported results
    // if not considered. Theoretically we could normalize the direction here, but this will lead to worse performance
    bool intersect(const Ray &ray, MediumEventType<ScalarType, 3> &me) const override {
        assert(std::abs(ray.direction.norm() - Scalar{1.0}) < Scalar{0.001}); // expect a normalized direction

        Point center{Scalar{0.0},Scalar{0.0},Scalar{0.0}};
        center = Shape3<Scalar>::transform_ * center;

        // since the intersect_ray_n_sphere can not handle ray.min_t directly, we apply it here manually
        bool hit = intersect_ray_n_sphere(ray.origin, ray.direction, center, radius_, me.p, me.t, me.geo_frame.n);

        if(hit) {
            // check if we are within the bounds
            if(me.t < ray.min_t || me.t > ray.max_t) {
                hit = false;
            }
        }

		me.geo_frame = Frame3f{me.geo_frame.n};
        me.sh_frame = me.geo_frame;

        return hit;
    }

    [[nodiscard]]
    AxisAlignedBoundingBox3f bounds() const override {
        throw std::runtime_error("not implemented");
    }

private:
    Scalar radius_;
};

using Sphere3f = Sphere<float>;
using Sphere3d = Sphere<double>;

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define Okapi_Sphere_e1052595_6050_4a7e_aad5_4d57c2f2bb62_h
