/*
 *  This file contains code copied from Mitsuba 3
 */

// Mitsuba 3 is available under the following license:

/*
    Copyright (c) 2017 Wenzel Jakob <wenzel.jakob@epfl.ch>, All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    You are under no obligation whatsoever to provide any bug fixes, patches, or
    upgrades to the features, functionality or performance of the source code
    ("Enhancements") to anyone; however, if you choose to make your Enhancements
    available either publicly, or directly to the author of this software, without
    imposing a separate written license agreement for such Enhancements, then you
    hereby grant the following license: a non-exclusive, royalty-free perpetual
    license to install, use, modify, prepare derivative works, incorporate into
    other computer software, distribute, and sublicense such enhancements or
    derivative works thereof, in binary and source code form.
*/


// Links
// https://mitsuba.readthedocs.io/en/latest/src/generated/plugins_shapes.html#serialized-mesh-loader-serialized
// https://github.com/mitsuba-renderer/mitsuba3/blob/3549783b37a9d831997f9c39a2a8d5affcbf2c50/src/shapes/serialized.cpp#L237
// https://github.com/mitsuba-renderer/mitsuba3/blob/master/include/mitsuba/core/zstream.h
// https://github.com/mitsuba-renderer/mitsuba3/blob/master/src/core/zstream.cpp#L6

#pragma once
#ifndef De_Vertexwahn_Core_SerializedMesh_9e6c8d39_75a1_4bb1_9b16_8602afb5a378_h
#define De_Vertexwahn_Core_SerializedMesh_9e6c8d39_75a1_4bb1_9b16_8602afb5a378_h

#include "okapi/rendering/shape/triangle_mesh.h"

#include "core/logging.hpp"
#include "core/namespace.hpp"

#include "zlib.h"

extern "C" {
    struct z_stream_s;
    typedef struct z_stream_s z_stream;
};

DE_VERTEXWAHN_BEGIN_NAMESPACE

namespace detail {
    constexpr size_t kZStreamBufferSize = 32768;
}

class ZStream {
public:
    enum EStreamType {
        EDeflateStream, /// A raw deflate stream
        EGZipStream /// A gzip-compatible stream
    };

    ZStream(EStreamType stream_type = EDeflateStream,
            int level = -1)
            : inflate_stream_(new z_stream()) {

        int window_bits = 15 + (stream_type == EGZipStream ? 16 : 0);

        inflate_stream_->zalloc = Z_NULL;
        inflate_stream_->zfree = Z_NULL;
        inflate_stream_->opaque = Z_NULL;
        inflate_stream_->avail_in = 0;
        inflate_stream_->next_in = Z_NULL;

        int retval = inflateInit2(inflate_stream_.get(), window_bits);
        if (retval != Z_OK)
            throw std::runtime_error("Could not initialize ZLIB");
    }

    void read(std::ifstream& file, void *ptr, size_t size) {
        //Assert(m_child_stream != nullptr);

        uint8_t *targetPtr = (uint8_t *) ptr;
        while (size > 0) {
            if (inflate_stream_->avail_in == 0) {
                //size_t remaining = m_child_stream->size() - m_child_stream->tell();
                size_t current_pos = file.tellg();
                file.seekg(0, std::ios::end);
                size_t end_of_file = file.tellg();
                size_t remaining = end_of_file - current_pos;
                file.seekg(current_pos); // set cursor back to previous position
                current_pos = file.tellg();
                inflate_stream_->next_in = inflate_buffer_;
                inflate_stream_->avail_in = (uInt) std::min(remaining, sizeof(inflate_buffer_));
                if (inflate_stream_->avail_in == 0)
                    throw std::runtime_error("Read less data than expected");
                //m_child_stream->read(m_inflate_buffer, m_inflate_stream->avail_in);
                //file.read(inflate_buffer_, inflate_stream_->avail_in);
                file.read(reinterpret_cast<char*>(inflate_buffer_),  inflate_stream_->avail_in);
            }

            inflate_stream_->avail_out = (uInt) size;
            inflate_stream_->next_out = targetPtr;

            int retval = inflate(inflate_stream_.get(), Z_NO_FLUSH);
            switch (retval) {
                case Z_STREAM_ERROR:
                    throw std::runtime_error("inflate(): stream error!");
                    break;
                case Z_NEED_DICT:
                    throw std::runtime_error("inflate(): need dictionary!");
                    break;
                case Z_DATA_ERROR:
                    throw std::runtime_error("inflate(): data error!");
                    break;
                case Z_MEM_ERROR:
                    throw std::runtime_error("inflate(): memory error!");
                    break;
            };

            size_t output_size = size - (size_t) inflate_stream_->avail_out;
            targetPtr += output_size;
            size -= output_size;

            if (size > 0 && retval == Z_STREAM_END)
                throw std::runtime_error("inflate(): attempting to read past the end of the stream!");
        }
    }

    std::unique_ptr<z_stream> inflate_stream_;
    uint8_t inflate_buffer_[detail::kZStreamBufferSize];
};

#define MI_FILEFORMAT_HEADER     0x041C
#define MI_FILEFORMAT_VERSION_V3 0x0003
#define MI_FILEFORMAT_VERSION_V4 0x0004

template <typename ScalarType>
class SerializedMesh : public TriangleMesh<ScalarType> {
public:
    enum class TriMeshFlags {
        HasNormals      = 0x0001,
        HasTexcoords    = 0x0002,
        HasTangents     = 0x0004, // unused
        HasColors       = 0x0008,
        FaceNormals     = 0x0010,
        SinglePrecision = 0x1000,
        DoublePrecision = 0x2000
    };

    constexpr bool has_flag(TriMeshFlags flags, TriMeshFlags f) {
        return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(f)) != 0;
    }
    constexpr bool has_flag(uint32_t flags, TriMeshFlags f) {
        return (flags & static_cast<uint32_t>(f)) != 0;
    }

    SerializedMesh(const PropertySet& ps) : TriangleMesh<ScalarType>{ps, true} {
        std::string filename = ps.get_property<std::string>("filename");

        std::stringstream ss;
        if(ps.has_property("parent_path")) {
            ss << ps.get_property<std::string>("parent_path") << "/" << filename;
        }
        else {
            ss << filename;
        }
        filename = ss.str();

        LOG_INFO_WITH_LOCATION("Loading mesh from {}", filename);

        //std::stringstream ss;

        /// When the file contains multiple meshes, this index specifies which one to load
        int shape_index = ps.get_property<int>("shape_index", 0);
        if (shape_index < 0) {
            throw std::runtime_error("shape index must be nonnegative!");
        }

        std::ifstream file(filename, std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("Could not load file");
        }

        std::uint16_t format{};
        std::uint16_t version{};

        file.read(reinterpret_cast<char*>(&format), sizeof(std::uint16_t));
        file.read(reinterpret_cast<char*>(&version), sizeof(std::uint16_t));

        if (format != MI_FILEFORMAT_HEADER) {
            throw std::runtime_error("encountered an invalid file format!");
        }

        if (version != MI_FILEFORMAT_VERSION_V3 &&
            version != MI_FILEFORMAT_VERSION_V4) {
            throw std::runtime_error("encountered an incompatible file version!");
        }

        assert(format == MI_FILEFORMAT_HEADER);
        assert(version == MI_FILEFORMAT_VERSION_V3);

        if (shape_index != 0) {
            size_t current_pos = file.tellg();
            file.seekg(0, std::ios::end);
            size_t file_size = file.tellg();
            file.seekg(current_pos); // set cursor back to previous position

            /* Determine the position of the requested substream. This
            is stored at the end of the file */
            file.seekg(file_size - sizeof(uint32_t));

            std::uint32_t count = 0;
            file.read(reinterpret_cast<char*>(&count), sizeof(std::uint32_t));

            if (shape_index > (int) count) {
                throw std::runtime_error("Unable to unserialize mesh, shape index is out of range!");
            }

            file.seekg(file_size - sizeof(uint32_t) * (count - shape_index + 1));
            std::uint32_t offset = 0;
            file.read(reinterpret_cast<char*>(&offset), sizeof(std::uint32_t));
            file.seekg(offset);

            // Skip header
            file.read(reinterpret_cast<char*>(&format), sizeof(std::uint16_t));
            file.read(reinterpret_cast<char*>(&version), sizeof(std::uint16_t));

            //stream->skip(sizeof(short) * 2); // Skip the header
        }

        ZStream zstream;

        uint32_t flags = 0;
        zstream.read(file, &flags, sizeof(flags));

        size_t vertex_count, face_count;
        zstream.read(file, &vertex_count, sizeof(vertex_count));
        zstream.read(file, &face_count, sizeof(face_count));

        size_t m_vertex_count = vertex_count;
        size_t m_face_count   = face_count;

        std::unique_ptr<uint32_t[]> faces(new uint32_t[m_face_count * 3]);
        std::unique_ptr<float[]> vertex_positions(new float[m_vertex_count * 3]);
        std::unique_ptr<float[]> vertex_normals(new float[m_vertex_count * 3]);
        std::unique_ptr<float[]> vertex_texcoords(new float[m_vertex_count * 2]);

        bool double_precision = has_flag(flags, TriMeshFlags::DoublePrecision);
        bool has_normals      = has_flag(flags, TriMeshFlags::HasNormals);
        bool has_texcoords    = has_flag(flags, TriMeshFlags::HasTexcoords);
        bool has_colors       = has_flag(flags, TriMeshFlags::HasColors);

        zstream.read(file, vertex_positions.get(), m_vertex_count * 3 * sizeof(float));

        for(int i = 0; i < m_vertex_count; ++i) {
            float x = vertex_positions[i*3+0];
            float y = vertex_positions[i*3+1];
            float z = vertex_positions[i*3+2];
            TriangleMesh<ScalarType>::positions_.push_back(Point3f{x, y, z});
        }

        if(has_normals) {
            zstream.read(file, vertex_normals.get(), m_vertex_count * 3 * sizeof(float));

            for(int i = 0; i < m_vertex_count; ++i) {
                float x = vertex_normals[i*3+0];
                float y = vertex_normals[i*3+1];
                float z = vertex_normals[i*3+2];
                TriangleMesh<ScalarType>::positions_.push_back(Normal3f{x, y, -z});
            }
        }

        if(has_texcoords) {
            zstream.read(file, vertex_normals.get(), m_vertex_count * 2 * sizeof(float));

            for(int i = 0; i < m_vertex_count; ++i) {
                float x = vertex_texcoords[i*2+0];
                float y = vertex_texcoords[i*2+1];
                TriangleMesh<ScalarType>::uvs_.push_back(Vector2f{x, y});
            }
        }

        if(has_colors) {
            assert(false);
            // not implemented by now
        }

        // read face data
        zstream.read(file, faces.get(), m_face_count * 3 * sizeof(std::uint32_t));

        for(size_t i = 0; i < m_face_count; ++i) {
            TriangleMesh<ScalarType>::position_indices_.push_back( faces[i*3+0] );
            TriangleMesh<ScalarType>::position_indices_.push_back( faces[i*3+1] );
            TriangleMesh<ScalarType>::position_indices_.push_back( faces[i*3+2] );
            TriangleMesh<ScalarType>::normal_indices_.push_back(faces[i*3+0]);
            TriangleMesh<ScalarType>::normal_indices_.push_back(faces[i*3+1]);
            TriangleMesh<ScalarType>::normal_indices_.push_back(faces[i*3+2]);
        }
    }
};

using SerializedMesh3f = SerializedMesh<float>;

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define De_Vertexwahn_Core_SerializedMesh_9e6c8d39_75a1_4bb1_9b16_8602afb5a378_h
