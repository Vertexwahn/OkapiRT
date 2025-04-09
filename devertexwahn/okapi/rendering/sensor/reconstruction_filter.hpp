/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef Okapi_ReconstructionFilter_74c44ed7_da01_4d2a_958b_5a98991cb72c_h
#define Okapi_ReconstructionFilter_74c44ed7_da01_4d2a_958b_5a98991cb72c_h

#include "flatland/rendering/property_set.h"

#include "math/constants.hpp"
#include "math/util.hpp"

#include "core/object.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

template <typename ScalarType>
class ReconstructionFilterType : public Object {
public:
	using Point = Point2<ScalarType>;
    using Vector = Point2<ScalarType>;

	ReconstructionFilterType(const PropertySet& ps) {
        radius_ = ps.get_property<Vector2f>("radius", Vector2f{2.f, 2.f});
	}
    virtual ScalarType evaluate(const Point& p) const = 0;

	const Vector& radius() const {
        return radius_;
    }

protected:
    Vector radius_;
};

using ReconstructionFilter = ReconstructionFilterType<float>;

template <typename ScalarType>
class BoxFilterType : public ReconstructionFilter {
public:
    BoxFilterType(const PropertySet& ps) : ReconstructionFilter(ps) {

    }

    float evaluate(const Point2f& p) const override {
        return box1d(p.x()/radius_.x()) * box1d(p.y()/radius_.y()) / (radius_.x() * radius_.y());
    }

private:
    ScalarType box1d(const ScalarType x) const {
        ScalarType abs_x = std::abs(x);
        if(abs_x < 1.f)
            return 1.f;
        else
            return 0.f;
    }
};

using BoxFilter = BoxFilterType<float>;

template <typename ScalarType>
inline constexpr ScalarType Sqr(const ScalarType v) {
    return v * v;
}

template <typename ScalarType>
ScalarType gaussian(const ScalarType x, const ScalarType mu = 0, const ScalarType sigma = 1) {
    return ScalarType{1.0} / std::sqrt(ScalarType{2.0} * pi_v<ScalarType> * sigma * sigma) *
    std::exp(-Sqr(x - mu) / (ScalarType{2.0} * sigma * sigma));
}

template <typename ScalarType>
class GaussianFilterType : public ReconstructionFilter {
public:
    using Vector2 = VectorType<ScalarType, 2>;
    using Point2 = PointType<ScalarType, 2>;

    GaussianFilterType(const PropertySet& ps) : ReconstructionFilter(ps) {
        standard_deviation_ = ps.get_property<ScalarType>("standard_deviation", ScalarType{0.5});
    }

    ScalarType evaluate(const Point2& p) const override {
        if(p.x() > radius_.x() || p.y() > radius_.y())
            return ScalarType{0};

        auto exp_x = gaussian(radius_.x(), ScalarType{0}, standard_deviation_);
        auto exp_y = gaussian(radius_.y(), ScalarType{0}, standard_deviation_);

        return (std::max<ScalarType>(0, gaussian(p.x(), ScalarType{0}, standard_deviation_) - exp_x) *
                std::max<ScalarType>(0, gaussian(p.y(), ScalarType{0}, standard_deviation_) - exp_y));
    }

    std::string to_string() const override {
        std::stringstream ss;
        ss << "GaussianFilter_standard_deviation_" << standard_deviation_ << "_" << "radius_" << radius_.x() << "_" << radius_.y();
        return ss.str();
    }

private:
    ScalarType standard_deviation_ = ScalarType{1.0};
};

using GaussianFilter = GaussianFilterType<float>;

template <typename ScalarType>
class TentFilterType : public ReconstructionFilter {
public:
    TentFilterType(const PropertySet& ps) : ReconstructionFilter(ps) {
	}

    ScalarType evaluate(const Point2f& p) const override {
        return tent1d(p.x()/radius_.x()) * tent1d(p.y()/radius_.y()) / (radius_.x() * radius_.y());
    }

    std::string to_string() const override {
        return "TentFilter";
    }

private:
    ScalarType tent1d(const ScalarType x) const {
        ScalarType abs_x = std::abs(x);
		if(abs_x < 1.f)
		    return 1.f - abs_x;
		else
            return 0.f;
	}
};

using TentFilter = TentFilterType<float>;

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define Okapi_ReconstructionFilter_74c44ed7_da01_4d2a_958b_5a98991cb72c_h
