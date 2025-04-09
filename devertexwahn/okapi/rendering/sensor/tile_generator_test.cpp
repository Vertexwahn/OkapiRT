/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/sensor/tile_generator.hpp"
#include "okapi/rendering/sensor/film.hpp"

#include "gmock/gmock.h"

#include <mutex>

using namespace de_vertexwahn;

// Test tile_count
class TileGeneratorParametersTests : public ::testing::TestWithParam<std::tuple<Vector2i, Vector2i, int>> {
protected:
};

TEST_P(TileGeneratorParametersTests, tile_count) {
    Vector2i preferred_tile_size = std::get<0>(GetParam());
    Vector2i film_size = std::get<1>(GetParam());
    TileGenerator tg{preferred_tile_size, film_size};

    EXPECT_THAT(tg.tile_count(), std::get<2>(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(
        CheckTileCount,
        TileGeneratorParametersTests,
        ::testing::Values(
                std::make_tuple(Vector2i{40,40}, Vector2i{0, 0}, 0),
                std::make_tuple(Vector2i{40,40}, Vector2i{80, 80}, 4),
                std::make_tuple(Vector2i{40,40}, Vector2i{81, 81}, 9),
                std::make_tuple(Vector2i{40,40}, Vector2i{90, 90}, 9),
                std::make_tuple(Vector2i{1000,1000}, Vector2i{90, 90}, 1)
        )
);

TEST(TileGenerator, tile_description_regular_tiles_first_tile_offset) {
    Vector2i preferred_tile_size{40, 40};
    Vector2i film_size{80, 80};
    TileGenerator tg{preferred_tile_size, film_size};

    EXPECT_THAT(tg.tile_description(0).offset, (Point2i{0, 0}));
}

TEST(TileGenerator, tile_description_regular_tiles_first_tile_size) {
    Vector2i preferred_tile_size{40, 40};
    Vector2i film_size{80, 80};
    TileGenerator tg{preferred_tile_size, film_size};

    EXPECT_THAT(tg.tile_description(0).size, (Vector2i{40, 40}));
}

TEST(TileGenerator, tile_description_regular_tiles_second_tile_offset) {
    Vector2i preferred_tile_size{40, 40};
    Vector2i film_size{80, 80};
    TileGenerator tg{preferred_tile_size, film_size};

    EXPECT_THAT(tg.tile_description(1).offset, (Point2i{40, 0}));
}

TEST(TileGenerator, tile_description_regular_tiles_second_tile_size) {
    Vector2i preferred_tile_size{40, 40};
    Vector2i film_size{80, 80};
    TileGenerator tg{preferred_tile_size, film_size};

    EXPECT_THAT(tg.tile_description(1).size, (Vector2i{40, 40}));
}

TEST(TileGenerator, tile_description_regular_tiles_other_tiles) {
    Vector2i preferred_tile_size{40,40};
    Vector2i film_size{80,80};
    TileGenerator tg{preferred_tile_size, film_size};

    EXPECT_THAT(tg.tile_description(2).offset, (Point2i{0, 40}));
    EXPECT_THAT(tg.tile_description(2).size, (Vector2i{40, 40}));

    EXPECT_THAT(tg.tile_description(3).offset, (Point2i{40, 40}));
    EXPECT_THAT(tg.tile_description(3).size, (Vector2i{40, 40}));
}

TEST(TileGenerator, tile_description_irregular_tiles_first_tile) {
    Vector2i preferred_tile_size{40,40};
    Vector2i film_size{100,80};
    TileGenerator tg{preferred_tile_size, film_size};

    EXPECT_THAT(tg.tile_description(0).offset, (Point2i{0, 0}));
    EXPECT_THAT(tg.tile_description(0).size, (Vector2i{40, 40}));
}

TEST(TileGenerator, tile_description_irregular_tiles_second_tile) {
    Vector2i preferred_tile_size{40,40};
    Vector2i film_size{100,80};
    TileGenerator tg{preferred_tile_size, film_size};

    EXPECT_THAT(tg.tile_description(1).offset, (Point2i{40, 0}));
    EXPECT_THAT(tg.tile_description(1).size, (Vector2i{40, 40}));
}

TEST(TileGenerator, tile_description_irregular_tiles_third_tile_offset) {
    Vector2i preferred_tile_size{40,40};
    Vector2i film_size{100,80};
    TileGenerator tg{preferred_tile_size, film_size};

    EXPECT_THAT(tg.tile_description(2).offset, (Point2i{80, 0}));
}

TEST(TileGenerator, tile_description_irregular_tiles_third_tile_size) {
    Vector2i preferred_tile_size{40,40};
    Vector2i film_size{100,80};
    TileGenerator tg{preferred_tile_size, film_size};

    EXPECT_THAT(tg.tile_description(2).size, (Vector2i{20, 40}));
}

TEST(TileGenerator, tile_description_irregular_tiles_last_tile_offset) {
    Vector2i preferred_tile_size{40,40};
    Vector2i film_size{100,100};
    TileGenerator tg{preferred_tile_size, film_size};

    EXPECT_THAT(tg.tile_description(8).offset, (Point2i{80, 80}));
}

TEST(TileGenerator, tile_description_irregular_tiles_last_tile_size) {
    Vector2i preferred_tile_size{40,40};
    Vector2i film_size{100,100};
    TileGenerator tg{preferred_tile_size, film_size};

    EXPECT_THAT(tg.tile_description(8).size, (Vector2i{20, 20}));
}

TEST(TileGenerator, multiple_tiles) {
    /// Arange
    // Film
    auto filter = make_reference_counted<TentFilter>(PropertySet{{"radius", Vector2f{1.f,1.f}}});
    Vector2i film_size{12, 12};
    AxisAlignedBoundingBox2i film_bounds{Vector2i{0, 0}, film_size};
    FilmTile film{Point2i{0, 0}, film_size, 3, filter.get(), film_bounds};

    // TileGenerator
    Vector2i preferred_tile_size{4, 4};
    TileGenerator tile_generator{preferred_tile_size, film_size};

    /// Act
    TileGenerator tg{preferred_tile_size, film_size};

    for(int index = 0; index < tg.tile_count(); ++index) {
        FilmTileDescription ftd = tg.tile_description(index);

        FilmTile film_tile{ftd.offset, ftd.size, 3, filter.get(), film.tile_bounds()};

        auto tile_size = ftd.size;
        for(int y = 0; y < tile_size.y(); ++y) {
            for(int x = 0; x < tile_size.x(); ++x) {
                ColorRGB3f c{1.f};
                film_tile.add_sample(Point2f{ftd.offset.x() + x + 0.5f, ftd.offset.y() + y + 0.5f}, c.data());
            }
        }

        film.add_tile(film_tile);
    }

    /// Expected
    std::vector<float> expected_film_weights = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    };

    EXPECT_THAT(film.filter_weight_sums(), expected_film_weights);
}
