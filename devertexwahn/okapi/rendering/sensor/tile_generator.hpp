/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef Okapi_TileGenerator_00d7dd0e_b670_4c1e_877f_0f9975f24240_h
#define Okapi_TileGenerator_00d7dd0e_b670_4c1e_877f_0f9975f24240_h

#include "math/point.hpp"
#include "math/vector.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

struct FilmTileDescription {
    Point2i offset;
    Vector2i size;
};

class TileGenerator {
public:
    TileGenerator(const Vector2i& preferred_tile_size, const Vector2i& film_size) :
            preferred_tile_size_(preferred_tile_size),
            film_size_(film_size) {
        cols_ = film_size_.x() / preferred_tile_size_.x() + (film_size_.x() % preferred_tile_size_.x() != 0 ? 1 : 0);
        rows_ = film_size_.y() / preferred_tile_size_.y() + (film_size_.y() % preferred_tile_size_.y() != 0 ? 1 : 0);
    }

    int tile_count() const {
        int number_of_tiles = cols_ * rows_;
        return number_of_tiles;
    }

    FilmTileDescription tile_description(const int tile_index) const {
        assert(tile_index >= 0);
        assert(tile_index < tile_count());

        Vector2i tile_size = preferred_tile_size_;

        int tile_index_x = tile_index % cols_;
        int tile_index_y = tile_index / cols_;

        int x = tile_index_x * preferred_tile_size_.x();
        int y = tile_index_y * preferred_tile_size_.y();

        if(tile_index_x == cols_ - 1) {
            tile_size.x() = film_size_.x() - x;
        }

        if(tile_index_y == rows_ - 1) {
            tile_size.y() = film_size_.y() - y;
        }

        FilmTileDescription ftd{Point2i{x,y}, tile_size};

        return ftd;
    }

private:
    Vector2i preferred_tile_size_;
    Vector2i film_size_;
    int cols_;
    int rows_;
};

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define Okapi_TileGenerator_00d7dd0e_b670_4c1e_877f_0f9975f24240_h
