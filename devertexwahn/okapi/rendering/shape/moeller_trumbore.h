/*
 *  SPDX-FileCopyrightText: Copyright 2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef De_Vertexwahn_Core_moeller_trumbore_577aedd3_f2ec_4c36_a786_828bce947c14_h
#define De_Vertexwahn_Core_moeller_trumbore_577aedd3_f2ec_4c36_a786_828bce947c14_h

#include "math/ray.hpp"
#include "math/point.hpp"

#include "core/namespace.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

// Möller–Trumbore intersection algorithm
// Fast Minimum Storage Ray/Triangle Intersection
// http://www.graphics.cornell.edu/pubs/1997/MT97.pdf
template <typename ScalarType>
bool intersect_triangle_moeller_trumbore(const RayType<ScalarType, 3>& ray,
                                         const Point3<ScalarType>& v0,
                                         const Point3<ScalarType>& v1,
                                         const Point3<ScalarType>& v2,
                                         ScalarType& t,
                                         ScalarType& u,
                                         ScalarType& v) {
    const ScalarType EPSILON = ScalarType{0.000001};

    // find vectors for two edges sharing vert0
    auto edge1 = v1 - v0;
    auto edge2 = v2 - v0;

    // begin calculating determinant - also used to calculate U parameter
    auto pvec = ray.direction.cross(edge2);

    // if determinant is near zero, ray lies in plane of triangle
    auto det = edge1.dot(pvec);

    if (det > -EPSILON && det < EPSILON)
        return false;
    auto inv_det = ScalarType{1} / det;

    // calculate distance from vert to ray origin
    auto tvec = ray.origin - v0;

    // calculate U parameter and test bounds
    u = tvec.dot(pvec) * inv_det;
    if(u < ScalarType{0} || u > ScalarType{1}) {
        return false;
    }

    // prepare to test V parameter
    auto qvec = tvec.cross(edge1);

    // calculate V parameter and test bounds
    v = ray.direction.dot(qvec) * inv_det;
    if(v < ScalarType{0.0} || u + v > ScalarType{1}) {
        return false;
    }

    // calculate t, ray intersects triangle
    t = edge2.dot(qvec) * inv_det;

    return true;
}

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define De_Vertexwahn_Core_moeller_trumbore_577aedd3_f2ec_4c36_a786_828bce947c14_h
