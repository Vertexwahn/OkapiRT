/*
 *  SPDX-FileCopyrightText: Copyright 2022-2025 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/shape/triangle_mesh.hpp"

#include "core/object_factory.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

void rply_message_callback(p_ply ply, const char *message) {
    LOG_WARNING("rply: {}", message);
}

/* Callback to handle vertex data from RPly */
int rply_vertex_callback(p_ply_argument argument) {
    float *buffer;
    long index, flags;

    ply_get_argument_user_data(argument, (void **)&buffer, &flags);
    ply_get_argument_element(argument, nullptr, &index);

    int stride = (flags & 0x0F0) >> 4;
    int offset = flags & 0x00F;

    buffer[index * stride + offset] = (float)ply_get_argument_value(argument);

    return 1;
}

/* Callback to handle face data from RPly */
int rply_face_callback(p_ply_argument argument) {
    FaceCallbackContext *context;
    long flags;
    ply_get_argument_user_data(argument, (void **)&context, &flags);

    long length, value_index;
    ply_get_argument_property(argument, nullptr, &length, &value_index);

    if (length != 3 && length != 4) {
        LOG_WARNING("plymesh: Ignoring face with {} vertices (only triangles and quads "
                "are supported!)",
                length);
        return 1;
    } else if (value_index < 0) {
        return 1;
    }

    if (value_index >= 0)
        context->face[value_index] = (int)ply_get_argument_value(argument);

    if (value_index == length - 1) {
        if (length == 3)
            for (int i = 0; i < 3; ++i)
                context->triIndices.push_back(context->face[i]);
        else {
            //CHECK_EQ(length, 4);
            LOG_ERROR("Only triangles are currently supported within PLY files");

            // Note: modify order since we're specifying it as a blp...
            context->quadIndices.push_back(context->face[0]);
            context->quadIndices.push_back(context->face[1]);
            context->quadIndices.push_back(context->face[3]);
            context->quadIndices.push_back(context->face[2]);
        }
    }

    return 1;
}

int rply_faceindex_callback(p_ply_argument argument) {
    std::vector<int> *faceIndices;
    long flags;
    ply_get_argument_user_data(argument, (void **)&faceIndices, &flags);

    faceIndices->push_back((int)ply_get_argument_value(argument));

    return 1;
}

TriMesh* load_trimesh_from_ply(const char* filename)
{
    miniply::PLYReader reader(filename);
    if (!reader.valid()) {
        return nullptr;
    }

    uint32_t indexes[3];
    bool gotVerts = false, gotFaces = false;

    TriMesh* trimesh = new TriMesh();
    while (reader.has_element() && (!gotVerts || !gotFaces)) {
        if (reader.element_is(miniply::kPLYVertexElement) && reader.load_element() && reader.find_pos(indexes)) {
            trimesh->numVerts = reader.num_rows();
            trimesh->pos = new float[trimesh->numVerts * 3];
            reader.extract_properties(indexes, 3, miniply::PLYPropertyType::Float, trimesh->pos);

            if (reader.find_normal(indexes)) {
                trimesh->normal = new float[trimesh->numVerts * 3];
                reader.extract_properties(indexes, 3, miniply::PLYPropertyType::Float, trimesh->normal);
            }

            if (reader.find_texcoord(indexes)) {
                trimesh->uv = new float[trimesh->numVerts * 2];
                reader.extract_properties(indexes, 2, miniply::PLYPropertyType::Float, trimesh->uv);
            }
            if (reader.find_normal(indexes)) {
                LOG_INFO("Found normals in polygon {}", indexes[0]);
            }
            gotVerts = true;
        }
        else if (reader.element_is(miniply::kPLYFaceElement) && reader.load_element() && reader.find_indices(indexes)) {
            bool polys = reader.requires_triangulation(indexes[0]);
            if (polys && !gotVerts) {
                fprintf(stderr, "Error: need vertex positions to triangulate faces.\n");
                break;
            }
            if (polys) {
                trimesh->numIndices = reader.num_triangles(indexes[0]) * 3;
                trimesh->indices = new int[trimesh->numIndices];
                reader.extract_triangles(indexes[0], trimesh->pos, trimesh->numVerts, miniply::PLYPropertyType::Int, trimesh->indices);
            }
            else {
                trimesh->numIndices = reader.num_rows() * 3;
                trimesh->indices = new int[trimesh->numIndices];
                reader.extract_list_property(indexes[0], miniply::PLYPropertyType::Int, trimesh->indices);
            }
            gotFaces = true;
        }
        if (gotVerts && gotFaces) {
            break;
        }
        reader.next_element();
    }

    if (!gotVerts || !gotFaces) {
        delete trimesh;
        return nullptr;
    }

    return trimesh;
}

void register_TriangleMesh3f() {
    ObjectFactory<PropertySet>::instance().register_class<TriangleMesh3f>("obj");
    ObjectFactory<PropertySet>::instance().register_class<TriangleMesh3f>("ply");
}

DE_VERTEXWAHN_END_NAMESPACE
