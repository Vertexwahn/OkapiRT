/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "core/reference_counted.hpp"
#include "okapi/rendering/sensor/reconstruction_filter.hpp"
#include "imaging/io/io.hpp"

#include "gmock/gmock.h"

using namespace de_vertexwahn;

// Copied form https://github.com/mmp/pbrt-v4/blob/90e7ff4f9696f26cde22b5bf5c092713aa8886a4/src/pbrt/filters_test.cpp
/*
TEST(GaussianFilter2f, GivenGausianFilter_WhenEvaluationgOutsideOfRadius_ExecptZeroContribution) {
    PropertySet ps;
    ps.addProperty("radius", Vector2f(1.5f, .25f));
    auto filter = GaussianFilter(ps);

    for (Vector2f r : {Vector2f(1, 1), Vector2f(1.5, .25), Vector2f(.33, 5.2),
                       Vector2f(.1, .1), Vector2f(3, 3)}) {
        EXPECT_EQ(0, filter.evaluate(Point2f(0, r.y() + 1e-3)));
        EXPECT_EQ(0, filter.evaluate(Point2f(r.x(), r.y() + 1e-3)));
        EXPECT_EQ(0, filter.evaluate(Point2f(-r.x(), r.y() + 1e-3)));
        EXPECT_EQ(0, filter.evaluate(Point2f(0, -r.y() - 1e-3)));
        EXPECT_EQ(0, filter.evaluate(Point2f(r.x(), -r.y() - 1e-3)));
        EXPECT_EQ(0, filter.evaluate(Point2f(-r.x(), -r.y() - 1e-3)));
        EXPECT_EQ(0, filter.evaluate(Point2f(r.x() + 1e-3, 0)));
        EXPECT_EQ(0, filter.evaluate(Point2f(r.x() + 1e-3, r.y())));
        EXPECT_EQ(0, filter.evaluate(Point2f(r.x() + 1e-3, -r.y())));
        EXPECT_EQ(0, filter.evaluate(Point2f(-r.x() - 1e-3, 0)));
        EXPECT_EQ(0, filter.evaluate(Point2f(-r.x() - 1e-3, r.y())));
        EXPECT_EQ(0, filter.evaluate(Point2f(-r.x() - 1e-3, -r.y())));
	}
}
*/

TEST(TentFilter, GivenTentFilter_WhenEvaluationFilterOrRadiusOrToString_ThenCorrectValues) {
    PropertySet ps;
    ps.add_property("radius", Vector2f(1.f, 1.f));
    auto filter = TentFilter(ps);

	EXPECT_THAT(filter.evaluate(Point2f(0.f, 0.f)), testing::FloatEq(1.f));
	EXPECT_THAT(filter.evaluate(Point2f(.5f, .0f)), testing::FloatEq(.5f));
	EXPECT_THAT(filter.evaluate(Point2f(.6f, .0f)), testing::FloatEq(.4f));
	EXPECT_THAT(filter.evaluate(Point2f(.5f, .5f)), testing::FloatEq(.25f));
	EXPECT_THAT(filter.evaluate(Point2f(100.f, 100.f)), testing::FloatEq(0.f));
	EXPECT_THAT(filter.radius(), Vector2f(1.f, 1.f));
	EXPECT_THAT(filter.to_string(), "TentFilter");
}

TEST(TentFilter, CompareToReferenceTentFilter) {
    int width = 201;
    int height = 201;

    Vector2f radius = {width / 2.f, height / 2.f};

    PropertySet ps_filter{
            {"radius", radius},
    };
    auto filter = make_reference_counted<TentFilter>(ps_filter);

    Image3f image = load_image("okapi/tests/images/tent_filter_ref.exr");

    for(int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Point2f sample_position = Point2f(x , y) + Vector2f{.5f,.5f};
            float value = radius.x() * radius.y() * filter->evaluate(sample_position - radius);
           
            float reference_value = image.get_pixel(x, y).red();
            ASSERT_THAT(value, reference_value);
        }
    }
}

TEST(GaussianFilter, CompareToReferenceGaussianFilter) {
    int width = 201;
    int height = 201;

    Vector2f radius = {2.f, 2.f};
    Vector2f center = {width / 2.f, height / 2.f};

    PropertySet ps_filter{};
    auto filter = make_reference_counted<GaussianFilter>(ps_filter);

    Image3f image = load_image("okapi/tests/images/GaussianFilter_standard_deviation_0.5_radius_2_2_filter_ref.exr");

    for(int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Point2f sample_position = Point2f(x , y) + Vector2f{.5f,.5f};
            float value = radius.x() * radius.y() * filter->evaluate(sample_position - center);

            float reference_value = image.get_pixel(x, y).red();
            ASSERT_THAT(value, reference_value);
        }
    }
}

TEST(GaussianFilter, CompareToReferenceGaussianFilter2) {
    int width = 201;
    int height = 201;

    Vector2f radius = {100.f, 100.f};
    Vector2f center = {width / 2.f, height / 2.f};

    PropertySet ps_filter{
            {"radius", radius},
            {"standard_deviation", 0.3f * radius.x()}
    };
    auto filter = make_reference_counted<GaussianFilter>(ps_filter);

    Image3f image = load_image("okapi/tests/images/GaussianFilter_standard_deviation_30_radius_100_100_filter_ref.exr");

    for(int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Point2f sample_position = Point2f(x , y) + Vector2f{.5f,.5f};
            float value = radius.x() * radius.y() * filter->evaluate(sample_position - center);
            auto color = hot_to_cold_color_ramp(value, 0.f, 1.f);
            ColorRGB3f reference_value = image.get_pixel(x, y);
            ASSERT_THAT(color, reference_value);
        }
    }
}
