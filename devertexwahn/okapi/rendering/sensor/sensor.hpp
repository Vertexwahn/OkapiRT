/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef Okapi_Sensor_14b27f8d_8dca_49d6_8087_3d67c31d29ec_h
#define Okapi_Sensor_14b27f8d_8dca_49d6_8087_3d67c31d29ec_h

#include "flatland/rendering/sensor/sensor.hpp"
#include "math/transform.hpp"
#include "flatland/rendering/property_set.h"
#include "okapi/rendering/sensor/film.hpp"
#include "core/exception.hpp"

#include <iostream>
#include <string>

DE_VERTEXWAHN_BEGIN_NAMESPACE

template <typename ScalarType>
class SensorType<ScalarType, 3> : public SensorTypeBase<ScalarType, 3> {
public:
    using Point = PointType<ScalarType, 3>;
    using Vector = VectorType<ScalarType, 3>;
    using Ray = RayType<ScalarType, 3>;
    using Scalar = ScalarType;
    using Transform = Transform44Type<ScalarType>;

    SensorType(const PropertySet& ps) : SensorTypeBase<ScalarType, 3>(ps) {
        film_ = std::dynamic_pointer_cast<Film>(ps.get_property<ReferenceCounted<Object>>("film"));
        assert(film_);

        horizontal_fov_ = degree_to_radian(ps.get_property<float>("fov", 30.0f));
        near_clip_plane_distance_ = ps.get_property<float>("near_clip", 0.1f);
        far_clip_plane_distance_ = ps.get_property<float>("far_clip", 500.0f);
        // TODO: does mitsuba use focus_distance_?
        focus_distance_ = ps.get_property("focus_distance", static_cast<float>(far_clip_plane_distance_));

        if(near_clip_plane_distance_ <= Scalar{0}) {
            throw Exception("The 'near_clip' parameter must be greater than zero!");
        }

        if(near_clip_plane_distance_ >= far_clip_plane_distance_) {
            throw Exception("Invalid parameters for near and far clip plane distance defined");
        }

        perspective_ = perspective(horizontal_fov_, near_clip_plane_distance_, far_clip_plane_distance_);

        Vector2<ScalarType> film_size_f = Vector2f(SensorType<ScalarType, 3>::film_->size().x(),
                                                   SensorType<ScalarType, 3>::film_->size().y());

        auto rasterSpaceToNDC = raster_space_to_ndc(film_size_f);

        Scalar aspect = film_size_f.x() / film_size_f.y();
/*
        std::string fov_axis = ps.get_property<std::string>("fov_axis", "x");
        if (fov_axis == "smaller")
            fov_axis = aspect > 1 ? "y" : "x";

        if (fov_axis == "y") {
            result = dr::rad_to_deg(
                    2.0 * dr::atan(dr::tan(0.5 * dr::deg_to_rad(fov)) * aspect));
        }

        */
        raster_space_to_camera_ = perspective_.inverse() * scale(1.f, 1.f / aspect, 1.f) * rasterSpaceToNDC;
    }

    virtual ~SensorType() {}

    Ray generate_ray(const Point2f& raster_position) const override {
        Point point_on_near_clipping_plane = raster_space_to_camera_ * Point3f(raster_position.x(), raster_position.y(), 0.f);

        Vector d = point_on_near_clipping_plane.normalized();
        Scalar invZ = Scalar{1} / d.z();

        auto origin = Point{Scalar{0.0}, Scalar{0.0}, Scalar{0.0}};
        auto direction = d;
        auto min_t = near_clip_plane_distance_ * invZ;
        auto max_t = far_clip_plane_distance_ * invZ;
        return SensorType<ScalarType, 3>::world_to_sensor_.inverse() * Ray{origin, direction, min_t, max_t};
    }

    ReferenceCounted<Film> film() {
        return film_;
    }

protected:
    ReferenceCounted<Film> film_;

    ScalarType horizontal_fov_;
    ScalarType near_clip_plane_distance_;
    ScalarType far_clip_plane_distance_;
    ScalarType focus_distance_;
    Transform perspective_;
    Transform raster_space_to_camera_;
};

using Sensor3f = SensorType<float, 3>;
using Sensor3d = SensorType<double, 3>;

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define Okapi_Sensor_14b27f8d_8dca_49d6_8087_3d67c31d29ec_h
