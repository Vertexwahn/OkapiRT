/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "flatland/rendering/sampler.hpp"
#include "imaging/io/io.hpp"
#include "okapi/rendering/sensor/film.hpp"

#include "gmock/gmock.h"

using namespace de_vertexwahn;

TEST(Film, Film_ctor) {
    // Arrange
    int width = 200;
    int height = 200;
    std::string filename = "output.exr";
    auto filter = make_reference_counted<TentFilter>(PropertySet{});

    PropertySet ps = {
        {"width", width},
        {"height", height},
        {"filename", filename},
        {"filter", filter},
    };

    // Act
    Film f{ps};

    // Assert
    EXPECT_THAT(f.width(), width);
    EXPECT_THAT(f.height(), height);
    EXPECT_THAT(f.filename(), filename);
    EXPECT_THAT(f.size(), Vector2i(width, height));
}

TEST(Film, channel_count) {
    // Arrange
    int width = 200;
    int height = 200;
    std::string filename = "output.exr";
    auto filter = make_reference_counted<TentFilter>(PropertySet{});

    PropertySet ps = {
            {"width", width},
            {"height", height},
            {"filename", filename},
            {"filter", filter},
    };

    Film film{ps};

    // Act
    size_t channel_count = film.channel_count();

    // Asset
    EXPECT_THAT(channel_count, 3);
}

ColorRGB3f f1(Point2f sample_position) {
    float value = 0.5 * (1+sin((sample_position.x()*sample_position.x()+sample_position.y()*sample_position.y())/100.f));

    return ColorRGB3f{value, value, value};
}

TEST(OpenEXR, Roundtrip32BitFloat) {
    {
        PropertySet ps_samplerA{
                {"deterministic_seed", true},
                {"sample_count", 1},
        };

        ReferenceCounted<Sampler2f> samplerA = std::make_shared<IndependentSampler>(ps_samplerA);

        Image3f imgA{100, 100};

        for(int y = 0; y < 100; ++y) {
            for(int x = 0; x < 100; ++x) {
                ColorRGB3f color{samplerA->next_1d(), samplerA->next_1d(), samplerA->next_1d()};
                imgA.set_pixel(x, y, color);
            }
        }

        store_image("test.exr", imgA);
    }

    {
        PropertySet ps_samplerB{
                {"deterministic_seed", true},
                {"sample_count", 1},
        };

        ReferenceCounted<Sampler2f> samplerB = std::make_shared<IndependentSampler>(ps_samplerB);

        auto imgB = load_image("test.exr");

        for(int y = 0; y < 100; ++y) {
            for(int x = 0; x < 100; ++x) {
                ColorRGB3f color = imgB.get_pixel(x, y);

                ASSERT_THAT(color.red(), samplerB->next_1d());
                ASSERT_THAT(color.green(), samplerB->next_1d());
                ASSERT_THAT(color.blue(), samplerB->next_1d());
            }
        }
    }
}

TEST(Film, IndependentSampler_spp_1_TentFilter_radius_1) {
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

    // create film
    ReferenceCounted<Film> film = make_reference_counted<Film>(PropertySet{
            {"width",    400},
            {"height",   400},
            {"filename", std::string("out.exr")},
            {"filter",   filter},
    });

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
    for(int y = 0; y < film->height(); ++y) {
        for (int x = 0; x < film->width(); ++x) {
            for(int sample = 0; sample < spp; ++sample) {
                auto next_2d = sampler->next_2d();

                if(rand_count < 8) {
                    EXPECT_NEAR(next_2d.x(), next_2d_expected[rand_count].x(), 0.01f);
                    EXPECT_NEAR(next_2d.y(), next_2d_expected[rand_count].y(), 0.01f);
                    rand_count++;
                }

                Point2f sample_position = Point2f(x , y) + next_2d;
                auto color = f1(sample_position);
                film->add_sample(sample_position, color.data());
                //film->add_sample_slow_but_correct(sample_position, color);
            }
        }
    }

    auto reference_image = load_image("okapi/tests/images/IndependentSampler_spp_1_TentFilter_radius_1_slow.exr");

    for(int y = 0; y < film->height(); ++y) {
        for (int x = 0; x < film->width(); ++x) {
            ColorRGB3f color;
            film->sample_color(Point2i(x , y), color.data());

            ASSERT_TRUE(color.red() >= 0.f);

            auto ref_color = reference_image.get_pixel(x, y);

            //const float abs_value = 0.00001f; // worked on macOS M1 before update to Sonoma
            const float abs_value = 0.001f;

            if(std::abs(color.red() - ref_color.red()) > abs_value) {
                std::cout << "x " << x << std::endl;
                std::cout << "y " << y << std::endl;
            }

            ASSERT_THAT(color.red(), testing::FloatNear(ref_color.red(), abs_value));
        }
    }
}

TEST(Film, IndependentSampler_spp_1_TentFilter_radius_1_slow) {
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

    // create film
    ReferenceCounted<Film> film = make_reference_counted<Film>(PropertySet{
            {"width",    400},
            {"height",   400},
            {"filename", std::string("out.exr")},
            {"filter",   filter},
    });

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
    for(int y = 0; y < film->height(); ++y) {
        for (int x = 0; x < film->width(); ++x) {
            for(int sample = 0; sample < spp; ++sample) {
                auto next_2d = sampler->next_2d();

                if(rand_count < 8) {
                    EXPECT_NEAR(next_2d.x(), next_2d_expected[rand_count].x(), 0.01f);
                    EXPECT_NEAR(next_2d.y(), next_2d_expected[rand_count].y(), 0.01f);
                    rand_count++;
                }

                Point2f sample_position = Point2f(x , y) + next_2d;
                auto color = f1(sample_position);
                film->add_sample(sample_position, color.data());

                if((x == y) || // test all diagonal elements
                   (x > 30 && x < 50 && y > 30 && y < 50)) { // test 20x20 square
                    film->add_sample_slow_but_correct(sample_position, color.data());
                } else {
                    film->add_sample(sample_position, color.data());
                }
            }
        }
    }

    auto ref_image = load_image("okapi/tests/images/IndependentSampler_spp_1_TentFilter_radius_1_slow.exr");

    for(int y = 0; y < film->height(); ++y) {
        for (int x = 0; x < film->width(); ++x) {
            ColorRGB3f color;
            film->sample_color(Point2i(x , y), color.data());

            ASSERT_TRUE(color.red() >= 0.f);
            
            auto reference_color = ref_image.get_pixel(x, y);

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

TEST(Film, IndependentSampler_spp_1_TentFilter_radius_1_simple1) {
    // create filter
    auto filter = make_reference_counted<TentFilter>(PropertySet{{"radius", Vector2f{1.f, 1.f}}});

    // create film
    ReferenceCounted<Film> film = make_reference_counted<Film>(PropertySet{
            {"width",    400},
            {"height",   400},
            {"filename", std::string("out.exr")},
            {"filter",   filter},
    });

    ColorRGB3f colorx;
    film->sample_color(Point2i(0, 0), colorx.data());
    ASSERT_TRUE(colorx.red() >= 0.f);

    for(int y = 0; y < film->height(); ++y) {
        for (int x = 0; x < film->width(); ++x) {
            Point2f sample_position = Point2f(x , y) +  Vector2f{.5f, .5f};
            auto color = ColorRGB3f{1.f, 1.f, 1.f};
            film->add_sample(sample_position, color.data());
        }
    }

    for(int y = 0; y < film->height(); ++y) {
        for (int x = 0; x < film->width(); ++x) {
            ColorRGB3f color;
            film->sample_color(Point2i(x , y), color.data());
            ASSERT_TRUE(color.red() >= 0.f);
        }
    }
}

TEST(Film, computeBounds) {
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

    // create film
    ReferenceCounted<Film> film = make_reference_counted<Film>(PropertySet{
            {"width",    400},
            {"height",   400},
            {"filename", std::string("out.exr")},
            {"filter",   filter},
    });

    {
        Point2f sample_position = Point2f{100.1f, 100.1f};
        AxisAlignedBoundingBox2i b = film->sample_bounds(sample_position);

        EXPECT_THAT(b.min_.x(), 99);
        EXPECT_THAT(b.max_.x(), 101);
        EXPECT_THAT(b.min_.y(), 99);
        EXPECT_THAT(b.max_.y(), 101);
    }

    {
        Point2f sample_position = Point2f{100.1f, 100.9f};
        AxisAlignedBoundingBox2i b = film->sample_bounds(sample_position);

        EXPECT_THAT(b.min_.x(), 99);
        EXPECT_THAT(b.max_.x(), 101);
        EXPECT_THAT(b.min_.y(), 100);
        EXPECT_THAT(b.max_.y(), 102);
    }

    {
        Point2f sample_position = Point2f{101.25f, 102.25f};
        AxisAlignedBoundingBox2i b = film->sample_bounds(sample_position);

        EXPECT_THAT(b.min_.x(), 100);
        EXPECT_THAT(b.max_.x(), 102);
        EXPECT_THAT(b.min_.y(), 101);
        EXPECT_THAT(b.max_.y(), 103);
    }

    {
        Point2f sample_position = Point2f(2.f, 2.f);
        AxisAlignedBoundingBox2i b = film->sample_bounds(sample_position);

        EXPECT_THAT(b.min_.x(), 1);
        EXPECT_THAT(b.max_.x(), 3);
        EXPECT_THAT(b.min_.y(), 1);
        EXPECT_THAT(b.max_.y(), 3);
    }

    {
        Point2f sample_position = Point2f{8.f, 8.f};
        AxisAlignedBoundingBox2i b = film->sample_bounds(sample_position);

        EXPECT_THAT(b.min_.x(), 7);
        EXPECT_THAT(b.max_.x(), 9);
        EXPECT_THAT(b.min_.y(), 7);
        EXPECT_THAT(b.max_.y(), 9);
    }
}

TEST(Film, to_string) {
    // Arrange
    int width = 200;
    int height = 200;
    std::string filename = "rendering.svg";
    auto filter = make_reference_counted<TentFilter>(PropertySet{});

    PropertySet ps = {
        {"width", width},
        {"height", height},
        {"filename", filename},
        {"filter", filter},
    };

    Film f{ps};

    // Act
    auto output = f.to_string();

    // Assert
    EXPECT_THAT(output, "Film");
}
