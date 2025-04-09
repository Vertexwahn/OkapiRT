/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef Okapi_Film_2bdf700f_45a2_433b_96f4_422b900e7d1a_h
#define Okapi_Film_2bdf700f_45a2_433b_96f4_422b900e7d1a_h

#include "okapi/rendering/sensor/film_tile.hpp"

#include <string>

DE_VERTEXWAHN_BEGIN_NAMESPACE

class Film : public FilmTile, public Object {
public:
    Film(const PropertySet& ps);
    virtual ~Film();

    std::string_view filename() const;
    std::string to_string() const override;

    ReferenceCounted<ReconstructionFilter> filter();

private:
    std::string filename_;
    ReferenceCounted<ReconstructionFilter> filter_;
};

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define Okapi_Film_2bdf700f_45a2_433b_96f4_422b900e7d1a_h
