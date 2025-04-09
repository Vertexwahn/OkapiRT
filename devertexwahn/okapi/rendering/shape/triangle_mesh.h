/*
 *  SPDX-FileCopyrightText: Copyright 2022-2024 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef De_Vertexwahn_Okapi_Rendering_TriangleMesh_9e88f244_71b6_4144_a77f_ba9de4f776c4_h
#define De_Vertexwahn_Okapi_Rendering_TriangleMesh_9e88f244_71b6_4144_a77f_ba9de4f776c4_h

#include "okapi/rendering/shape/moeller_trumbore.h"

#include "flatland/rendering/shape/shape.hpp"

#include "core/logging.hpp"
#include "core/namespace.hpp"

#include "math/geometry.hpp"
#include "math/transform.hpp"
#include "math/util.hpp"

#include "tiny_obj_loader.h"

DE_VERTEXWAHN_BEGIN_NAMESPACE

// TODO(Vertexwahn): rename to OBJ mesh?
template <typename ScalarType>
class TriangleMesh : public Shape3<ScalarType> {
public:
    using Base = Shape3<ScalarType>;
    using Frame = Frame3<ScalarType>;
    using Normal = Normal3<ScalarType>;
    using Point = Point3<ScalarType>;
    using Ray = RayType<ScalarType, 3>;
    using Scalar = ScalarType;
    using Vector = Vector3<ScalarType>;

    // TODO(Vertexwahn): Refactor this: TriangleMesh should maybe be the base class for SerializedMesh and a ObjMesh class
    TriangleMesh(const PropertySet& ps, bool serialized) : Shape3<Scalar>{ps} {}

    TriangleMesh(const PropertySet& ps) : Shape3<Scalar>{ps} {
		auto filename = ps.get_property<std::string>("filename");

        LOG_INFO_WITH_LOCATION("Loading mesh from {}", filename);

        filename = fmt::format("{}/{}", ps.get_property<std::string>("parent_path"), filename);

        tinyobj::attrib_t attribute;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warnings;
        std::string errors;

        bool load_result = tinyobj::LoadObj(&attribute, &shapes, &materials, &warnings, &errors, filename.c_str());

		if(!load_result) {
            std::string error_message = fmt::format("Loading Wavefront OBJ ({}) failed.", filename);
            throw std::runtime_error(error_message);
		}

		for(size_t i = 0; i < attribute.vertices.size() / 3; ++i)  {
            tinyobj::real_t x = attribute.vertices[3 * i + 0];
            tinyobj::real_t y = attribute.vertices[3 * i + 1];
            tinyobj::real_t z = attribute.vertices[3 * i + 2];

            // Flip z, since obj uses a right-handed coordinate system and Ether uses a left-handed
            // coordinate system
			positions_.push_back(Point3f{x,y,-z});
		}

		for (size_t i = 0; i < attribute.normals.size() / 3; ++i) {
            tinyobj::real_t x = attribute.normals[3 * i + 0];
            tinyobj::real_t y = attribute.normals[3 * i + 1];
            tinyobj::real_t z = attribute.normals[3 * i + 2];

            // Flip z, since obj uses a right-handed coordinate system and Ether uses a left-handed
            // coordinate system
			normals_.push_back(Normal3f{x, y, -z});
		}

        bool flip_tex_coords = ps.get_property<bool>("flip_tex_coords", true);

        for(size_t i = 0; i < attribute.texcoords.size() / 2; ++i) {
            tinyobj::real_t tx = attribute.texcoords[2 * i + 0];
            tinyobj::real_t ty = attribute.texcoords[2 * i + 1];

            if(flip_tex_coords) {
                ty = 1.0f - ty;
            }

            uvs_.push_back(Vector2f{tx, ty});
        }

        for(size_t shape_index = 0; shape_index < shapes.size(); ++shape_index) {
            for(size_t i = 0; i < shapes[shape_index].mesh.indices.size(); ++i) {
                position_indices_.push_back(shapes[shape_index].mesh.indices[i].vertex_index);
                normal_indices_.push_back(shapes[shape_index].mesh.indices[i].normal_index);
                if(shapes[shape_index].mesh.indices[i].texcoord_index != -1) {
                    uv_indices_.push_back(shapes[shape_index].mesh.indices[i].texcoord_index);
                }
            }
        }

        check_position_buffer();
        check_position_index_buffer();
	}

    TriangleMesh(
            const PropertySet& ps,
            const std::vector<Point3f>& positions,
            const std::vector<unsigned int>& position_indices) : Shape3<Scalar>(ps),
      positions_(positions), position_indices_(position_indices) {
        check_position_buffer();
        check_position_index_buffer();
    }
    virtual ~TriangleMesh() = default;

    bool intersect(const Ray &ray, MediumEventType<ScalarType, 3> &me) const override {
        bool triangle_mesh_hit = false;
        me.t = ray.max_t;

        // for each triangle
        const size_t triangle_count = position_indices_.size() / 3;
        for(size_t i = 0; i < triangle_count; ++i) {
            uint32_t f = 0;
            f = i;

            const Point3f* V = positions();
            const uint32_t* position_indices = position_index();

            const Normal3f* N = normals();
            const uint32_t* normal_indices = normal_index_data();

            // Vertex indices of the triangle
            Point3f p0 = Base::transform() * V[position_indices[f * 3 + 0]];
            Point3f p1 = Base::transform() * V[position_indices[f * 3 + 1]];
            Point3f p2 = Base::transform() * V[position_indices[f * 3 + 2]];

            float t = -1.0f;
            float u = -1.0f;
            float v = -1.0f;
            bool hit = intersect_triangle_moeller_trumbore(ray, p0, p1, p2, t, u, v);

            if(hit && t <= me.t) {
                Point3f intersection_point = (1-u-v)*p0 + u * p1 + v * p2;
                me.p = intersection_point;
                me.t = t;
                me.geo_frame = Frame3f{(p2 - p0).cross(p1 - p0).normalized()};
                me.shape = this;

                if(/*normal_indices.size() > 0 &&*/ N) {
                    Normal3f n0 = Base::transform() * N[normal_indices[f * 3 + 0]];
                    Normal3f n1 = Base::transform() * N[normal_indices[f * 3 + 1]];
                    Normal3f n2 = Base::transform() * N[normal_indices[f * 3 + 2]];

                    me.sh_frame = Frame3f{((1-u-v)*n0 + u * n1 + v * n2).normalized()};
                }
                else {
                    me.sh_frame = me.geo_frame;
                }

                triangle_mesh_hit = true;
            }
        }

        return triangle_mesh_hit;
    }

    // Positions
    [[nodiscard]]
    size_t position_index_count() const {
	    return position_indices_.size();
	}
    [[nodiscard]]
    const uint32_t* position_index() const {
		return position_indices_.data();
	}

    [[nodiscard]]
    size_t position_count() const {
		return positions_.size();
	}
    [[nodiscard]]
    const Point3f* positions() const {
		return positions_.data();
	}

    // Normals
    [[nodiscard]]
    size_t normal_count() const {
		return normals_.size();
	}

    [[nodiscard]]
	const Normal3f* normals() const {
		return normals_.data();
	}
    [[nodiscard]]
	size_t normal_index_count() {
		return normal_indices_.size();
	}
    [[nodiscard]]
    const uint32_t* normal_index_data() const {
		return normal_indices_.data();
	}

    // UVs
    [[nodiscard]]
    size_t uv_count() const {
        return uvs_.size();
    }

    [[nodiscard]]
    const Point2f* uvs() const {
        return uvs_.data();
    }
    [[nodiscard]]
    size_t uv_index_count() {
        return uv_indices_.size();
    }
    [[nodiscard]]
    const uint32_t* uv_index_data() const {
        return uv_indices_.data();
    }

    [[nodiscard]]
    AxisAlignedBoundingBox3f bounds() const override {
        if (position_count() == 0) {
            return AxisAlignedBoundingBox3f{{0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}};
        }

        auto current_min = positions_[position_indices_[0]];
        auto current_max = positions_[position_indices_[0]];

        for (size_t i = 1; i < position_index_count(); ++i) {
            auto position = positions_[position_indices_[i]];

            if(position.x() < current_min.x()) {
                current_min.x() = position.x();
            }
            if(position.y() < current_min.y()) {
                current_min.y() = position.y();
            }
            if(position.z() < current_min.z()) {
                current_min.z() = position.z();
            }

            if(position.x() > current_max.x()) {
                current_max.x() = position.x();
            }
            if(position.y() > current_max.y()) {
                current_max.y() = position.y();
            }
            if(position.z() > current_max.z()) {
                current_max.z() = position.z();
            }
        }

        auto transform = Shape3<ScalarType>::transform();

        return AxisAlignedBoundingBox3f{transform * current_min, transform * current_max};
    }

protected:
	void check_position_buffer() {
        if(positions_.empty()) {
            throw std::runtime_error("No vertices provided");
        }

        if(positions_.size() <= 2u) {
            throw std::runtime_error("To less vertices are provided");
        }
	}

	void check_position_index_buffer() {
        if(position_indices_.empty()) {
            throw std::runtime_error("Empty index buffer");
        }

        if(position_indices_.size() % 3 != 0u) {
            throw std::runtime_error("Wrong number of provided sub_shape_ids");
        }
	}

protected:
    std::vector<Point3f> positions_;
    std::vector<Normal3f> normals_;
    std::vector<Point2f> uvs_;

    std::vector<uint32_t> position_indices_;
    std::vector<uint32_t> normal_indices_;
    std::vector<uint32_t> uv_indices_;
};

using TriangleMesh3f = TriangleMesh<float>;

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define De_Vertexwahn_Okapi_Rendering_TriangleMesh_9e88f244_71b6_4144_a77f_ba9de4f776c4_h
