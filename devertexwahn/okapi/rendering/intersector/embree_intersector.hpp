/*
 *  SPDX-FileCopyrightText: Copyright 2022-2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef Okapi_Embree_591acdef_5111_4707_9a72_853fa2a8808a_h
#define Okapi_Embree_591acdef_5111_4707_9a72_853fa2a8808a_h

#include "okapi/rendering/shape/triangle_mesh.h"

#include "flatland/rendering/intersector/intersector.hpp"
#include "flatland/rendering/shape/shape.hpp"
#include "flatland/rendering/scene/scene.hpp"

#include "math/frame.hpp"

#include "core/logging.hpp"

#include "embree4/rtcore.h"

#include "boost/predef.h"

#if BOOST_ARCH_X86_64
#include <pmmintrin.h>
#include <xmmintrin.h> // SIMD Extensions intrinsics
#endif

DE_VERTEXWAHN_BEGIN_NAMESPACE

class EmbreeIntersector : public IntersectorType<float, 3> {
public:
	explicit EmbreeIntersector(const PropertySet&  /*ps*/) {
        LOG_INFO_WITH_LOCATION("Setup Embree integrator.");
#if BOOST_ARCH_X86_64
        // Setup SIMD Extensions intrinsics for EmbreeIntersector
        _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
        _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
#endif

        // create EmbreeIntersector device at application startup
        device_ = rtcNewDevice(nullptr);
        //device_ = rtcNewDevice("verbose=1"); // #if defined(NDEBUG) ...
        assert(rtcGetDeviceError(device_) == RTC_ERROR_NONE);

        // create scene
        scene_ = rtcNewScene(device_);
        assert(rtcGetDeviceError(device_) == RTC_ERROR_NONE);
        assert(scene_);
    }

    ~EmbreeIntersector() {
        rtcReleaseScene(scene_);
        rtcReleaseDevice(device_);
    }

    // see here: https://github.com/embree/embree/blob/master/tutorials/triangle_geometry/triangle_geometry_device.cpp
    void build_acceleration_structure(std::vector<ReferenceCounted<Shape>> shapes) override {
		shapes_ = shapes;

		for(size_t shape_index = 0; shape_index < shapes.size(); ++shape_index) {
            ReferenceCounted<TriangleMesh3f> tm = std::dynamic_pointer_cast<TriangleMesh3f>(shapes[shape_index]);

            // create a geometry buffer for positions, texture coordinates and normals
            // see https://github.com/embree/embree/blob/ae029e2ff83bebbbe8742c88aba5b0521aba1a23/tutorials/common/tutorial/scene_device.cpp

            RTCGeometry mesh = rtcNewGeometry(device_, RTC_GEOMETRY_TYPE_TRIANGLE);
            assert(rtcGetDeviceError(device_) == RTC_ERROR_NONE);
            assert(mesh);

            struct VertexPosition3f { float x,y,z; };
            VertexPosition3f* vertices = (VertexPosition3f*) rtcSetNewGeometryBuffer(mesh,
                                                                                     RTC_BUFFER_TYPE_VERTEX,
                                                                                     0,
                                                                                     RTC_FORMAT_FLOAT3,
                                                                                     sizeof(VertexPosition3f),
                                                                                     tm->position_count());
            for (size_t i = 0; i < tm->position_count(); ++i) {
                auto p = tm->transform() * tm->positions()[i];
                vertices[i].x = p.x();
                vertices[i].y = p.y();
                vertices[i].z = p.z();
            }
            assert(rtcGetDeviceError(device_) == RTC_ERROR_NONE);

            rtcSetSharedGeometryBuffer(mesh,
                                       RTC_BUFFER_TYPE_INDEX,
                                       0,
                                       RTC_FORMAT_UINT3,
                                       tm->position_index(),
                                       0,
                                       3*sizeof(unsigned int),
                                       tm->position_index_count() / 3);
            assert(rtcGetDeviceError(device_) == RTC_ERROR_NONE);

            rtcCommitGeometry(mesh);

            uint32_t geomId = shape_index;
            rtcAttachGeometryByID(scene_, mesh, geomId);
            assert(rtcGetDeviceError(device_) == RTC_ERROR_NONE);

            rtcReleaseGeometry(mesh);
            assert(rtcGetDeviceError(device_) == RTC_ERROR_NONE);
		}

        rtcCommitScene(scene_);
        assert(rtcGetDeviceError(device_) == RTC_ERROR_NONE);
    }

    bool intersect(const Ray &ray, MediumEvent &me) const override {
        RTCRayHit query;
        query.ray.org_x  = ray.origin.x();
        query.ray.org_y = ray.origin.y();
        query.ray.org_z =  ray.origin.z();
        query.ray.dir_x  = ray.direction.x();
        query.ray.dir_y = ray.direction.y();
        query.ray.dir_z = ray.direction.z();
        query.ray.tnear  = ray.min_t;
        query.ray.tfar   = ray.max_t;
        query.ray.time = 0.f;
        query.ray.mask = -1;
        query.ray.id = 0;
        query.ray.flags = 0;
        query.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        query.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

        // trace ray
        RTCIntersectArguments args;
        rtcInitIntersectArguments(&args);
        args.feature_mask = (RTCFeatureFlags) (RTC_FEATURE_FLAG_TRIANGLE);
        assert(rtcGetDeviceError(device_) == RTC_ERROR_NONE);

        rtcIntersect1(scene_, &query, &args);
        assert(rtcGetDeviceError(device_) == RTC_ERROR_NONE);

        // hit data filled on hit
        if (query.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
            me.t = std::numeric_limits<float>::infinity();
            return false;
		}

        me.shape = get_shape_by_id(query.hit.geomID);

        // hit data filled on hit
        float u = query.hit.u;
        float v = query.hit.v;
        float t = query.ray.tfar;

        me.t = t;
        me.p = Point3f(ray.origin + t * ray.direction);
		//me.geo_frame.tangent = Vector3f(query.hit..Ng_s, query.hit.Ng_s, query.hit.Ng_z);
		//me.geo_frame.bitangent = Vector3f(query.ray..Ns_x, query.hit.Ns_y, query.hit.Ng_z);

		me.geo_frame.n = -Normal3f(query.hit.Ng_x, query.hit.Ng_y, query.hit.Ng_z).normalized();

        revised_onb(me.geo_frame.n, me.geo_frame.s, me.geo_frame.t);

        // alternative approach to compute intersection point and geo_frame
		// https://github.com/Vertexwahn/Ether/blob/master/Core/src/Ether/Core/Scene.cpp

        uint32_t f = 0;
        f = query.hit.primID;

        // Find the barycentric coordinates
		Vector2f uv{u,v};
        me.uv = uv;
        Vector3f bary;
        bary << 1 - uv.sum(), uv;

        /* References to all relevant mesh buffers */
        ReferenceCounted<TriangleMesh3f> mesh = std::dynamic_pointer_cast<TriangleMesh3f>(shapes_[query.hit.geomID]);
        const Point3f* V = mesh->positions();
        const Normal3f* N = mesh->normals();
        const Point2f* UV = mesh->uvs();
        const uint32_t* position_indices = mesh->position_index();
		const uint32_t* normal_indices = mesh->normal_index_data();
        const uint32_t* uv_indices = mesh->uv_index_data();

        /* Vertex indices of the triangle */
        Point3f p0 = mesh->transform() * V[position_indices[f * 3 + 0]];
        Point3f p1 = mesh->transform() * V[position_indices[f * 3 + 1]];
        Point3f p2 = mesh->transform() * V[position_indices[f * 3 + 2]];

        // Compute interpolated UV coordinates
        // TODO(Vertexwahn): Shift the to Embree -> https://github.com/embree/embree/blob/ae029e2ff83bebbbe8742c88aba5b0521aba1a23/tutorials/common/tutorial/scene_device.cpp#L474C1-L484C6
        if(mesh->uv_index_count() > 0) {
            Point2f uv0 = UV[uv_indices[f*3 + 0]];
            Point2f uv1 = UV[uv_indices[f*3 + 1]];
            Point2f uv2 = UV[uv_indices[f*3 + 2]];
            me.uv = bary.x() * uv0 + bary.y() * uv1 + bary.z() * uv2;
        }

        /* Compute the intersection position accurately
         using barycentric coordinates */
        me.p = bary.x() * p0 + bary.y() * p1 + bary.z() * p2;

        /* Compute the geometry frame */
        me.geo_frame = Frame3f{(p2 - p0).cross(p1 - p0).normalized()};

        if (N != nullptr) {
            /* Compute the shading frame. Note that for simplicity,
            the current implementation doesn't attempt to provide
            tangents that are continuous across the surface. That
            means that this code will need to be modified to be able
            use anisotropic BRDFs, which need tangent continuity */

            Normal3f n0 = mesh->transform() * N[normal_indices[f * 3 + 0]];
            Normal3f n1 = mesh->transform() * N[normal_indices[f * 3 + 1]];
            Normal3f n2 = mesh->transform() * N[normal_indices[f * 3 + 2]];

            me.sh_frame = Frame3f{(bary.x() * n0 + bary.y() * n1 + bary.z() * n2).normalized()};
        } else {
            me.sh_frame = me.geo_frame;
        }

        return true;
    }

    std::string to_string() const override {
        return "EmbreeIntersector";
    }

private:
    const Shape* get_shape_by_id(std::uint32_t id) const {
        return shapes_[id].get();
    }

private:
    RTCDevice device_ = nullptr;
    RTCScene scene_ = nullptr;
    std::vector<ReferenceCounted<Shape>> shapes_;
};

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define Okapi_Embree_591acdef_5111_4707_9a72_853fa2a8808a_h
