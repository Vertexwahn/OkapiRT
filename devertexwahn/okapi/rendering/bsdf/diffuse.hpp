/*
 *  SPDX-FileCopyrightText: Copyright 2022-2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef De_Vertexwahn_Okapi_Diffuse_d5a966fc_7d84_46c6_83b3_8ddda6518dcd_h
#define De_Vertexwahn_Okapi_Diffuse_d5a966fc_7d84_46c6_83b3_8ddda6518dcd_h

#include "flatland/rendering/bsdf/bsdf.hpp"

#include "math/frame.hpp"
#include "math/sampling.hpp"
#include "math/util.hpp"

#include "core/exception.hpp"
#include "core/namespace.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

class Diffuse final : public BSDF3f {
public:
    explicit Diffuse(const PropertySet &ps) : BSDF3f(ps) {
    
    }

    [[nodiscard]]
    ColorRGB3f sample(BSDFSample& sample, const Point2& sample_point) const override {
        return ColorRGB3f{0.f};
    }

    // https://github.com/mitsuba-renderer/mitsuba2/blob/62863cb3b58ab81ac5161908f39c2bfbf928cb9d/src/bsdfs/diffuse.cpp#L122
    [[nodiscard]]
    float pdf(const BSDFSample& sample) const override {
        return 0;
    }

    [[nodiscard]]
    ColorRGB3f evaluate(const BSDFSample& sample) const override {
        return ColorRGB3f{0.f};
    }


private:
};

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define De_Vertexwahn_Okapi_Diffuse_d5a966fc_7d84_46c6_83b3_8ddda6518dcd_h
