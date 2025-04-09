/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/sensor/film.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

Film::Film(const PropertySet& ps) : FilmTile(ps, 3) {
    filename_ = ps.get_property<std::string>("filename");
    filter_ = std::dynamic_pointer_cast<ReconstructionFilter>(ps.get_property<ReferenceCounted<Object>>("filter"));
}

Film::~Film() {

}

std::string_view Film::filename() const {
    return filename_;
}

std::string Film::to_string() const {
    return "Film";
}

ReferenceCounted<ReconstructionFilter> Film::filter() {
    return filter_;
}

DE_VERTEXWAHN_END_NAMESPACE
