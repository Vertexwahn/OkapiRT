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

#include "boost/algorithm/string.hpp"

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

    ScalarType parse_fov(const PropertySet &props, double aspect) {
        if (props.has_property("fov") && props.has_property("focal_length"))
            throw Exception("Please specify either a focal length ('focal_length') or a "
                "field of view ('fov')!");

        double fov;
        std::string fov_axis;

        if (props.has_property("fov")) {
            fov = props.get_property<float>("fov");

            fov_axis = boost::algorithm::to_lower_copy(props.get_property<std::string>("fov_axis", "x"));

            if (fov_axis == "smaller")
                fov_axis = aspect > 1 ? "y" : "x";
            else if (fov_axis == "larger")
                fov_axis = aspect > 1 ? "x" : "y";
        }

        ScalarType result;

        if (fov_axis == "x") {
            result = fov;
        } else if(fov_axis == "y") {
            result = radian_to_degree((
                2.0 * std::atan(std::tan(0.5 * degree_to_radian(fov)) * aspect)));
        }
        else {
            throw std::runtime_error("Unsupported fov axis");
        }

        if (result <= 0.0 || result >= 180.0)
            throw Exception("The horizontal field of view must be in the range [0, 180]!");

        return degree_to_radian(result);
    }

    SensorType(const PropertySet& ps) : SensorTypeBase<ScalarType, 3>(ps) {
        film_ = std::dynamic_pointer_cast<Film>(ps.get_property<ReferenceCounted<Object>>("film"));
        assert(film_);

        //horizontal_fov_ = degree_to_radian(ps.get_property<float>("fov", 30.0f));
        Vector2<ScalarType> film_size_f = Vector2f(SensorType<ScalarType, 3>::film_->size().x(),
                                                   SensorType<ScalarType, 3>::film_->size().y());
        Scalar aspect = film_size_f.x() / film_size_f.y();
        horizontal_fov_ = parse_fov(ps, aspect);

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

        auto rasterSpaceToNDC = raster_space_to_ndc(film_size_f);


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
