/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "flatland/rendering/sampler.hpp"
#include "imaging/io/io.hpp"
#include "okapi/rendering/sensor/film.hpp"

#include "gmock/gmock.h"

using namespace de_vertexwahn;

TEST(FilmTile, ctor) {
    // Arrange
    int x = 80;
    int y = 400;
    int width = 40;
    int height = 40;
    auto filter = make_reference_counted<TentFilter>(PropertySet{{"radius", Vector2f{1.f,1.f}}});
    AxisAlignedBoundingBox2i film_bounds{Point2i{0, 0}, Point2i{800, 600}};

    // Act
    FilmTile tile{Point2i{x, y}, Vector2i{width, height}, 3, filter.get(), film_bounds};


    // Assert
    EXPECT_THAT(tile.offset().x(), x);
    EXPECT_THAT(tile.offset().y(), y);
    EXPECT_THAT(tile.size().x(), width);
    EXPECT_THAT(tile.size().y(), height);
}

TEST(FilmTile, allocate_image_tensor_memory) {
    // Arrange
    int width = 40;
    int height = 40;
    auto filter = make_reference_counted<TentFilter>(PropertySet{{"radius", Vector2f{1.f,1.f}}});
    AxisAlignedBoundingBox2i film_bounds{Point2i{0, 0}, Point2i{800, 600}};

    FilmTile tile{Point2i{0, 0}, Vector2i{width, height}, 3, filter.get(), film_bounds};

    // Act
    tile.allocate_image_tensor_memory(9);

    // Assert
    EXPECT_THAT(tile.channel_count(), 9);
}

class FilmTileTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        PropertySet ps_sampler{
                {"deterministic_seed", true},
                {"sample_count", 1},
        };

        sampler = std::make_shared<IndependentSampler>(ps_sampler);
        filter = make_reference_counted<TentFilter>(PropertySet{{"radius", Vector2f{1.f,1.f}}});

        film = make_reference_counted<Film>(PropertySet{
                {"width", 33},
                {"height", 22},
                {"filename", std::string("out.exr")},
                {"filter", filter},
        });

        // sample is expected to be deterministic for this test
        float first_random_number = sampler->next_1d();
        EXPECT_THAT(first_random_number, testing::FloatEq(0.157555878f));

        ASSERT_THAT(filter->radius(), (Vector2f{1.f,1.f}));
    }

public:
    ReferenceCounted<Sampler2f> sampler = nullptr;
    de_vertexwahn::ReferenceCounted<de_vertexwahn::TentFilter> filter = nullptr;
    ReferenceCounted<Film> film = nullptr;
};



TEST_F(FilmTileTest, CorrectOffsetAndSize) {
    /// Arrange
    AxisAlignedBoundingBox2i film_bounds{Vector2i{0, 0}, Vector2i{30, 30}};
    Point2i tile_offset{20, 20};
    Vector2i tile_size{4, 4};

    /// Act
    FilmTile ft{tile_offset, tile_size, 3, filter.get(), film_bounds};

    /// Assert
    EXPECT_THAT(ft.offset(), (Point2i{20,20}));
    EXPECT_THAT(ft.size(), (Vector2i{4,4}));
    EXPECT_THAT(ft.width(), 4);
    EXPECT_THAT(ft.height(), 4);
}

TEST_F(FilmTileTest, StartTileBounds) {
    /// Arrange
    AxisAlignedBoundingBox2i film_bounds{Vector2i{0, 0}, Vector2i{30, 30}};
    Point2i tile_offset{0, 0};
    Vector2i tile_size{4, 4};
    FilmTile ft{tile_offset, tile_size, 3, filter.get(), film_bounds};

    /// Act
    AxisAlignedBoundingBox2i tile_bounds = ft.tile_bounds();

    /// Assert
    EXPECT_THAT(tile_bounds.min_.x(), 0);
    EXPECT_THAT(tile_bounds.min_.y(), 0);
    EXPECT_THAT(tile_bounds.max_.x(), 5);
    EXPECT_THAT(tile_bounds.max_.y(), 5);
}

TEST_F(FilmTileTest, TopTileBounds) {
    /// Arrange
    AxisAlignedBoundingBox2i film_bounds{Vector2i{0, 0}, Vector2i{30, 30}};
    Point2i tile_offset{4, 0};
    Vector2i tile_size{4, 4};
    FilmTile ft{tile_offset, tile_size, 3, filter.get(), film_bounds};

    /// Act
    AxisAlignedBoundingBox2i tile_bounds = ft.tile_bounds();

    /// Assert
    EXPECT_THAT(tile_bounds.min_.x(), 3);
    EXPECT_THAT(tile_bounds.min_.y(), 0);
    EXPECT_THAT(tile_bounds.max_.x(), 9);
    EXPECT_THAT(tile_bounds.max_.y(), 5);
}

TEST_F(FilmTileTest, InnerTileBounds) {
    /// Arrange
    AxisAlignedBoundingBox2i film_bounds{Vector2i{0, 0}, Vector2i{30, 30}};
    Point2i tile_offset{4, 4};
    Vector2i tile_size{4, 4};
    FilmTile ft{tile_offset, tile_size, 3, filter.get(), film_bounds};

    /// Act
    AxisAlignedBoundingBox2i tile_bounds = ft.tile_bounds();

    /// Assert
    EXPECT_THAT(tile_bounds.min_.x(), 3);
    EXPECT_THAT(tile_bounds.min_.y(), 3);
    EXPECT_THAT(tile_bounds.max_.x(), 9);
    EXPECT_THAT(tile_bounds.max_.y(), 9);
}

TEST_F(FilmTileTest, ClipTileOnRightBoarder) {
    /// Arrange
    AxisAlignedBoundingBox2i film_bounds{Vector2i{0, 0}, Vector2i{30, 30}};
    Point2i tile_offset{28, 4};
    Vector2i tile_size{4, 4};
    FilmTile ft{tile_offset, tile_size, 3, filter.get(), film_bounds};

    /// Act
    AxisAlignedBoundingBox2i tile_bounds = ft.tile_bounds();

    /// Assert
    EXPECT_THAT(tile_bounds.min_.x(), 27);
    EXPECT_THAT(tile_bounds.min_.y(), 3);
    EXPECT_THAT(tile_bounds.max_.x(), 30);
    EXPECT_THAT(tile_bounds.max_.y(), 9);
}

TEST_F(FilmTileTest, ClipTileOnRightAndBottomBoarder) {
    /// Arrange
    AxisAlignedBoundingBox2i film_bounds{Vector2i{0, 0}, Vector2i{30, 30}};
    Point2i tile_offset{28, 28};
    Vector2i tile_size{4, 4};
    FilmTile ft{tile_offset, tile_size, 3, filter.get(), film_bounds};

    /// Act
    AxisAlignedBoundingBox2i tile_bounds = ft.tile_bounds();

    /// Assert
    EXPECT_THAT(tile_bounds.min_.x(), 27);
    EXPECT_THAT(tile_bounds.min_.y(), 27);
    EXPECT_THAT(tile_bounds.max_.x(), 30);
    EXPECT_THAT(tile_bounds.max_.y(), 30);
}

TEST_F(FilmTileTest, add_sample) {
    /// Arrange
    AxisAlignedBoundingBox2i film_bounds{Vector2i{0, 0}, Vector2i{30, 30}};
    Point2i tile_offset{4, 4};
    Vector2i tile_size{4, 4};
    FilmTile ft{tile_offset, tile_size, 3, filter.get(), film_bounds};

    /// Act
    ft.add_sample(Point2f{tile_offset.x() + 0.5f,tile_offset.y() +0.5f}, ColorRGB3f{1.0f}.data());
    ft.add_sample(Point2f{tile_offset.x() + 3.5f,tile_offset.y() + 2.5f}, ColorRGB3f{0.5f}.data());
    auto image = ft.image();

    /// Assert
    EXPECT_THAT(image->get_pixel(0, 0), (ColorRGB3f{1.f, 1.f, 1.f}));
    EXPECT_THAT(image->get_pixel(3, 2), (ColorRGB3f{.5f, .5f, .5f}));
}

ColorRGB3f f1(Point2f sample_position) {
    float value = 0.5f * (1+sin((sample_position.x()*sample_position.x()+sample_position.y()*sample_position.y())/100.f));

    return ColorRGB3f{value, value, value};
}

TEST_F(FilmTileTest, add_sample_slow_but_correct) {
    // create sampler
    PropertySet ps_sampler{
        {"deterministic_seed", true},
        {"sample_count", 1},
    };

    ReferenceCounted<Sampler2f> sampler = std::make_shared<IndependentSampler>(ps_sampler);

    float first_random_number = sampler->next_1d();

    EXPECT_THAT(first_random_number, testing::FloatEq(0.157555878f));

    // create filter
    auto filter = make_reference_counted<TentFilter>(PropertySet{{"radius", Vector2f{1.f, 1.f}}});

    /// Arrange
    AxisAlignedBoundingBox2i film_bounds{Vector2i{0, 0}, Vector2i{400, 400}};
    FilmTile film{Point2i{0, 0}, Vector2i{400, 400}, 3, filter.get(), film_bounds};

    int rand_count = 0;
    Point2f next_2d_expected[] = {
        {0.0978490859f, 0.795962334f},
        {0.837165892f, 0.760376275f},
        {0.0599010214f, 0.839832187f},
        {0.309668094f, 0.393747687f},
        {0.626871109f, 0.334569722f},
        {0.792928874f, 0.744295239f},
        {0.47442773f, 0.416714907f},
        {0.848185658f, 0.729767203f},
    };

    int spp = sampler->sample_count();
    for(int y = 0; y < film.height(); ++y) {
        for (int x = 0; x < film.width(); ++x) {
            for(int sample = 0; sample < spp; ++sample) {
                auto next_2d = sampler->next_2d();

                if(rand_count < 8) {
                    EXPECT_NEAR(next_2d.x(), next_2d_expected[rand_count].x(), 0.01f);
                    EXPECT_NEAR(next_2d.y(), next_2d_expected[rand_count].y(), 0.01f);
                    rand_count++;
                }

                Point2f sample_position = Point2f(x , y) + next_2d;
                auto color = f1(sample_position);
                film.add_sample(sample_position, color.data());

                if((x == y) || // test all diagonal elements
                   (x > 30 && x < 50 && y > 30 && y < 50)) { // test 20x20 square
                    film.add_sample_slow_but_correct(sample_position, color.data());
                    //film.add_sample(sample_position, color);
                } else {
                    film.add_sample(sample_position, color.data());
                }
            }
        }
    }

    auto reference_image = load_image("okapi/tests/images/IndependentSampler_spp_1_TentFilter_radius_1_slow.exr");

    for(int y = 0; y < film.height(); ++y) {
        for (int x = 0; x < film.width(); ++x) {
            ColorRGB3f color;
            film.sample_color(Point2i(x , y), color.data());

            ASSERT_TRUE(color.red() >= 0.f);

            auto reference_color = reference_image.get_pixel(x, y);

            //const float abs_value = 0.00001f; // worked on macOS M1 before update to Sonoma
            const float abs_value = 0.001f;

            if(std::abs(color.red() - reference_color.red()) > abs_value) {
                std::cout << "x " << x << std::endl;
                std::cout << "y " << y << std::endl;
            }

            ASSERT_THAT(color.red(), testing::FloatNear(reference_color.red(), abs_value));
        }
    }
}

TEST(FilmTile, add_tile_center_tile) {
    /// Arrange

    // Film
    auto filter = make_reference_counted<TentFilter>(PropertySet{{"radius", Vector2f{1.f,1.f}}});
    Vector2i film_size{12, 12};
    AxisAlignedBoundingBox2i film_bounds{Vector2i{0, 0}, film_size};
    FilmTile film{Point2i{0, 0}, film_size, 3, filter.get(), film_bounds};

    // Tile
    Point2i tile_offset{4, 4};
    Vector2i tile_size{4, 4};
    FilmTile tile{tile_offset, tile_size, 3, filter.get(), film_bounds};

    /// Act
    for (int y = 4; y < 8; ++y) {
        for(int x = 4; x < 8; ++x) {
            tile.add_sample(Point2f{x + 0.5f, y + 0.5f}, ColorRGB3f{1.f}.data());
        }
    }

    film.add_tile(tile);

    /// Expected
    std::vector<float> tile_weights = {
            0, 0, 0, 0, 0, 0,
            0, 1, 1, 1, 1, 0,
            0, 1, 1, 1, 1, 0,
            0, 1, 1, 1, 1, 0,
            0, 1, 1, 1, 1, 0,
            0, 0, 0, 0, 0, 0,
    };

    EXPECT_THAT(tile.filter_weight_sums(), tile_weights);

    std::vector<float> expected_film_weights = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    EXPECT_THAT(film.filter_weight_sums(), expected_film_weights);
}

TEST(FilmTile, add_tile_center_tile_multiple_samples) {
    /// Arrange

    // Film
    auto filter = make_reference_counted<TentFilter>(PropertySet{{"radius", Vector2f{1.f, 1.f}}});
    Vector2i film_size{12, 12};
    AxisAlignedBoundingBox2i film_bounds{Vector2i{0, 0}, film_size};
    FilmTile film{Point2i{0, 0}, film_size, 3, filter.get(), film_bounds};

    // Tile
    Point2i tile_offset{4, 4};
    Vector2i tile_size{4, 4};
    FilmTile tile{tile_offset, tile_size, 3, filter.get(), film_bounds};

    /// Act
    for (int y = 4; y < 8; ++y) {
        for (int x = 4; x < 8; ++x) {
            for(int sample = 0; sample < 2; ++sample) {
                tile.add_sample(Point2f{x + .5f, y + .5f}, ColorRGB3f{1.f}.data());
            }
        }
    }

    film.add_tile(tile);

    /// Expected
    std::vector<float> expected_tile_weights = {
            0, 0, 0, 0, 0, 0,
            0, 2, 2, 2, 2, 0,
            0, 2, 2, 2, 2, 0,
            0, 2, 2, 2, 2, 0,
            0, 2, 2, 2, 2, 0,
            0, 0, 0, 0, 0, 0,
    };

    EXPECT_THAT(tile.filter_weight_sums(), expected_tile_weights);

    std::vector<ColorRGB3f> expected_radiance_sum_tile = {
            ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0},
            ColorRGB3f{0}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{0},
            ColorRGB3f{0}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{0},
            ColorRGB3f{0}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{0},
            ColorRGB3f{0}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{0},
            ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0},
    };

    //ReferenceCounted<Image3f> image = tile.image(0);
    //std::vector<Color3f> radiance_sums{image->data(), image->data() + (image->width() * image->height())};
    //EXPECT_THAT(radiance_sums, expected_radiance_sum_tile);
    EXPECT_THAT(tile.radiance_sums(), expected_radiance_sum_tile);

    std::vector<float> expected_film_weights = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0,
            0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0,
            0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0,
            0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    EXPECT_THAT(film.filter_weight_sums(), expected_film_weights);

    std::vector<ColorRGB3f> expected_film_radiance_sums = {
            ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0},
            ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0},
            ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0},
            ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0},

            ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0},
            ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0},
            ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0},
            ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{2}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0},

            ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0},
            ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0},
            ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0},
            ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0}, ColorRGB3f{0},
    };

    //ReferenceCounted<Image3f> image2 = film.image(0);
    //std::vector<Color3f> radiance_sums2(image2->data(), image2->data() + (image2->width() * image2->height()));
    //EXPECT_THAT(radiance_sums2, expected_film_radiance_sums);
    EXPECT_THAT(film.radiance_sums(), expected_film_radiance_sums);
}

TEST(FilmTile, AddTileWithColorNormalAlbedo) {
    /// Arrange

    // Film
    auto filter = make_reference_counted<TentFilter>(PropertySet{{"radius", Vector2f{1.f,1.f}}});
    Vector2i film_size{12, 12};
    AxisAlignedBoundingBox2i film_bounds{Vector2i{0, 0}, film_size};
    uint32_t channel_count = 9;
    FilmTile film{Point2i{0, 0}, film_size, channel_count, filter.get(), film_bounds};

    // Tile
    Point2i tile_offset{4, 4};
    Vector2i tile_size{4, 4};
    FilmTile tile{tile_offset, tile_size, channel_count, filter.get(), film_bounds};

    EXPECT_THAT(tile.size().x(), 4);
    EXPECT_THAT(tile.size().y(), 4);

    /// Act
    for (int y = 4; y < 8; ++y) {
        for(int x = 4; x < 8; ++x) {
            std::unique_ptr<float[]> aovs(new float[channel_count]);
            aovs[0] = .114452f;
            aovs[1] = .0226659f;
            aovs[2] = .000832037f;
            aovs[3] = 0.f;
            aovs[4] = -1.f;
            aovs[5] = 0.f;
            aovs[6] = 0.725f;
            aovs[7] = 0.71f;
            aovs[8] = 0.68f;
            tile.add_sample(Point2f{x + .5f, y + .5f}, aovs.get());
        }
    }

    // Expect correct values in tile
    std::unique_ptr<float[]> aovs(new float[channel_count]);
    tile.sample_color(Point2i(2 , 2), aovs.get());

    EXPECT_THAT(aovs[0], 0.114452f);
    EXPECT_THAT(aovs[1], .0226659f);
    EXPECT_THAT(aovs[2], .000832037f);
    EXPECT_THAT(aovs[3], .0f);
    EXPECT_THAT(aovs[4], -1.f);
    EXPECT_THAT(aovs[5], 0.f);
    EXPECT_THAT(aovs[6], 0.725f);
    EXPECT_THAT(aovs[7], 0.71f);
    EXPECT_THAT(aovs[8], 0.68f);

    // Act
    film.add_tile(tile);

    // Expect correct values in film
    film.sample_color(Point2i(5 , 5), aovs.get());
    EXPECT_THAT(aovs[0], 0.114452f);
    EXPECT_THAT(aovs[1], .0226659f);
    EXPECT_THAT(aovs[2], .000832037f);
    EXPECT_THAT(aovs[3], .0f);
    EXPECT_THAT(aovs[4], -1.f);
    EXPECT_THAT(aovs[5], 0.f);
    EXPECT_THAT(aovs[6], 0.725f);
    EXPECT_THAT(aovs[7], 0.71f);
    EXPECT_THAT(aovs[8], 0.68f);

    // Act
    EXPECT_THAT(film.image(0)->get_pixel(5,5).red(), 0.114452f);
    EXPECT_THAT(film.image(0)->get_pixel(5,5).green(), .0226659f);
    EXPECT_THAT(film.image(0)->get_pixel(5,5).blue(), .000832037f);

    EXPECT_THAT(film.image(3)->get_pixel(5,5).red(), 0.f);
    EXPECT_THAT(film.image(3)->get_pixel(5,5).green(), -1.0f);
    EXPECT_THAT(film.image(3)->get_pixel(5,5).blue(), .0f);

    EXPECT_THAT(film.image(6)->get_pixel(5,5).red(), 0.725f);
    EXPECT_THAT(film.image(6)->get_pixel(5,5).green(), 0.71f);
    EXPECT_THAT(film.image(6)->get_pixel(5,5).blue(), 0.68f);

    /// Expected

    EXPECT_THAT(film.radiance_sums()[5+5*12].red(), 0.114452f);

    std::vector<float> tile_weights = {
            0, 0, 0, 0, 0, 0,
            0, 1, 1, 1, 1, 0,
            0, 1, 1, 1, 1, 0,
            0, 1, 1, 1, 1, 0,
            0, 1, 1, 1, 1, 0,
            0, 0, 0, 0, 0, 0,
    };

    EXPECT_THAT(tile.filter_weight_sums(), tile_weights);

    std::vector<float> expected_film_weights = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    EXPECT_THAT(film.filter_weight_sums(), expected_film_weights);
}

TEST(FilmTile, SingleSampleWeights) {
    // Create a 2x2 tile
    Point2i tile_offset{0, 0};
    Vector2i tile_size{2, 2};
    uint32_t channel_count = 3;
    AxisAlignedBoundingBox2i film_bounds{Vector2i{0, 0}, tile_size};
    auto filter = make_reference_counted<BoxFilter>(PropertySet{{"radius", Vector2f{1.f,1.f}}});
    FilmTile tile{tile_offset, tile_size, channel_count, filter.get(), film_bounds};

    // Set every pixel to 1.0
    for (int y = 0; y < tile_size.y(); ++y) {
        for(int x = 0; x < tile_size.x(); ++x) {
            std::unique_ptr<float[]> aovs(new float[channel_count]);
            aovs[0] = 1.0f;
            aovs[1] = 1.0f;
            aovs[2] = 1.0f;
            tile.add_sample(Point2f{x + .5f, y + .5f}, aovs.get());
        }
    }

    // Expect that every pixel is 1.0
    EXPECT_THAT(tile.image()->get_pixel(0,0).red(), 1.0f);
    EXPECT_THAT(tile.image()->get_pixel(0,0).green(), 1.0f);
    EXPECT_THAT(tile.image()->get_pixel(0,0).blue(), 1.0f);
}

TEST(FilmTile, MultipleSamplesWeights) {
    // Create a 2x2 tile
    Point2i tile_offset{0, 0};
    Vector2i tile_size{2, 2};
    uint32_t channel_count = 3;
    AxisAlignedBoundingBox2i film_bounds{Vector2i{0, 0}, tile_size};
    auto filter = make_reference_counted<BoxFilter>(PropertySet{{"radius", Vector2f{1.f,1.f}}});
    FilmTile tile{tile_offset, tile_size, channel_count, filter.get(), film_bounds};

    // Set every pixel to 1.0
    for (int y = 0; y < tile_size.y(); ++y) {
        for(int x = 0; x < tile_size.x(); ++x) {
            for(int s = 0; s < 16; ++s) {
                std::unique_ptr<float[]> aovs(new float[channel_count]);
                aovs[0] = 1.0f;
                aovs[1] = 1.0f;
                aovs[2] = 1.0f;
                tile.add_sample(Point2f{x + .5f, y + .5f}, aovs.get());
            }
        }
    }

    // Same result as in SingleSampleWeights
    // Expect that every pixel is 1.0
    EXPECT_THAT(tile.image()->get_pixel(0,0).red(), 1.0f);
    EXPECT_THAT(tile.image()->get_pixel(0,0).green(), 1.0f);
    EXPECT_THAT(tile.image()->get_pixel(0,0).blue(), 1.0f);
}
