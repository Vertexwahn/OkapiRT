/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "okapi/rendering/sensor/film.hpp"
#include "okapi/rendering/sensor/sensor.hpp"
#include "imaging/io/io.hpp"

#include "gmock/gmock.h"

using namespace de_vertexwahn;

class Sensor2fTest : public ::testing::Test {
protected:
    ReferenceCounted<ReconstructionFilter> filter = make_reference_counted<TentFilter>(PropertySet{});

    ReferenceCounted<Film_> film = make_reference_counted<Film_>(
            PropertySet{
                    {"width",    800},
                    {"height",   600},
                    {"filename", std::string("out.svg")},
                    {"filter",   filter},
            });

    Sensor2f sensor{{
        {"film", film}
    }};
};

TEST_F(Sensor2fTest, ctor) {
    // Assert
    Matrix44f identityMatrix;
    identityMatrix << 1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f;

    EXPECT_THAT(sensor.transform().matrix(), identityMatrix);

    EXPECT_THAT(sensor.film()->width(), 800);
    EXPECT_THAT(sensor.film()->height(), 600);
    EXPECT_THAT(sensor.film()->filename(), "out.svg");
}

TEST_F(Sensor2fTest, getFilm) {
    auto film = sensor.film();

    EXPECT_THAT(film->width(), 800);
    EXPECT_THAT(film->height(), 600);
    EXPECT_THAT(film->filename(), "out.svg");
}

TEST_F(Sensor2fTest, setAndGetTransform) {
    // Arrange
    auto translation = translate(1.f,2.f,3.f);

    // Act
    sensor.set_transform(translation);
    auto transform = sensor.transform();

    // Assert
    EXPECT_THAT(transform, translation);
}

TEST_F(Sensor2fTest, generate_ray) {
    Ray2f ray = sensor.generate_ray(Point2f{0.f, 0.f});

    EXPECT_THAT(ray.origin.x(), 0.f);
    EXPECT_THAT(ray.origin.y(), 0.f);

    EXPECT_NEAR(ray.direction.x(), 1.f, 0000.1f);
    EXPECT_NEAR(ray.direction.y(), 0.f, 0.1f);
}

TEST_F(Sensor2fTest, generate_ray2) {
    sensor.set_transform(look_at(Point2f{0.f, 0.f}, Point2f{1.f, 0.f}));
    Ray2f ray = sensor.generate_ray(Point2f{0.f, 0.f});

    EXPECT_THAT(ray.origin, (Point2f{0.f, 0.f}));
    EXPECT_THAT(ray.direction, (Vector2f{1.f, 0.f}));
}

TEST_F(Sensor2fTest, generate_ray3) {
    sensor.set_transform(look_at(Point2f{1.f, 1.f}, Point2f{2.f, 2.f}));
    Ray2f ray = sensor.generate_ray(Point2f{0.f, 0.f});

    EXPECT_NEAR(ray.origin.x(), 1.f, 0.1f);
}

TEST_F(Sensor2fTest, generate_ray4) {
    sensor.set_transform(look_at(Point2f{1.f, 1.f}, Point2f{2.f, 2.f}));

    Ray2f ray = sensor.generate_ray(Point2f{0.f, 0.f});

    auto direction = Vector2f{1.f, 1.f}.normalized();
    EXPECT_NEAR(ray.direction.x(), direction.x(), 0.1f);
}

TEST(Sensor3f, CtorInvalidParameters) {
    const Vector2i film_plane_size_in_pixels{800, 800};

    ReferenceCounted<ReconstructionFilter> filter = make_reference_counted<TentFilter>(PropertySet{});

    ReferenceCounted<Film> film = make_reference_counted<Film>(
            PropertySet{
                    {"width",    film_plane_size_in_pixels.x()},
                    {"height",   film_plane_size_in_pixels.y()},
                    {"filename", std::string("out.exr")},
                    {"filter",   filter},
            });

    EXPECT_THROW(Sensor3f({
                            {"fov", 90.f},
                            {"near_clip", 500.f},
                            {"far_clip", 100.f},
                            {"film", film}
                    }), Exception);
}

class Sensor3fTest : public ::testing::Test {
protected:
    const Vector2i film_plane_size_in_pixels{800, 800};

    ReferenceCounted<ReconstructionFilter> filter = make_reference_counted<TentFilter>(PropertySet{});

    ReferenceCounted<Film> film = make_reference_counted<Film>(
            PropertySet{
                    {"width",    film_plane_size_in_pixels.x()},
                    {"height",   film_plane_size_in_pixels.y()},
                    {"filename", std::string("out.exr")},
                    {"filter",   filter},
            });

    Sensor3f sensor{{
        {"fov", 90.f},
        {"near_clip", 100.f},
        {"far_clip", 500.f},
        {"film", film}
    }};

    Sensor3fTest() {
        auto transform = translate(0.f,0.f,0.f);
        sensor.set_transform(transform);
    }
};

TEST_F(Sensor3fTest, ctor) {
    // Assert
    Matrix44f identityMatrix;
    identityMatrix << 1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f;

    EXPECT_THAT(sensor.transform().matrix(), identityMatrix);

    EXPECT_THAT(sensor.film()->width(), film_plane_size_in_pixels.x());
    EXPECT_THAT(sensor.film()->height(), film_plane_size_in_pixels.y());
    EXPECT_THAT(sensor.film()->filename(), "out.exr");

    EXPECT_THAT(sensor.film()->image()->width(), film_plane_size_in_pixels.x());
    EXPECT_THAT(sensor.film()->image()->height(), film_plane_size_in_pixels.y());
}

TEST_F(Sensor3fTest, GivenRasterPositionAtCenterOfImagePlane_WhenGeneratingRay_ThenDirectionIsTowardsZ) {
    Point3f camera_positions[] = {
        {0.f, 0.f, 0.f},
        {0.f, 0.f, 10.f},
        {0.f, 0.f, 0.f},
        {0.f, 0.f, 0.f},
        {0.f, 0.f, 0.f},
        {0.f, 0.f, 0.f},
    };

    Point3f camera_look_at_positions[] = {
        {0.f, 0.f, 100.f},
        {0.f, 0.f, 100.f},
        {45.f, 0.f, 45.f},
        {100.f, 0.f, 0.f},
        {100.f, 100.f, 100.f},
        {-100.f, -100.f, -100.f},
    };

    Point3f expectedray_origin[] = {
         {0.f, 0.f, 0.f},
         {0.f, 0.f, 10.f},
         {0.f, 0.f, 0.f},
         {0.f, 0.f, 0.f},
         {0.f, 0.f, 0.f},
         {0.f, 0.f, 0.f},
    };

    Vector3f expected_center_ray[] = {
        {0.f, 0.f, 1.f},
        {0.f, 0.f, 1.f},
        {std::sqrt(.5f), 0.f, std::sqrt(.5f)},
        {1.f, 0.f, 0.f},
        {std::sqrt(1.f/3.f), std::sqrt(1.f/3.f), std::sqrt(1.f/3.f)},
        {-std::sqrt(1.f/3.f), -std::sqrt(1.f/3.f), -std::sqrt(1.f/3.f)},
    };

    size_t count_test_cases = sizeof(camera_positions) / sizeof(camera_positions[0]);

    ASSERT_THAT(sizeof(camera_look_at_positions) / sizeof(camera_look_at_positions[0]), count_test_cases);
    ASSERT_THAT(sizeof(expectedray_origin) / sizeof(expectedray_origin[0]), count_test_cases);
    ASSERT_THAT(sizeof(expected_center_ray) / sizeof(expected_center_ray[0]), count_test_cases);

    for(size_t i = 0; i < count_test_cases; ++i) {
        // Arrange
        auto transform = look_at(camera_positions[i], camera_look_at_positions[i], Vector3f{0.f, 1.f, 0.f});
        sensor.set_transform(transform);

        auto film = sensor.film();
        float x = film->width() / 2.f;
        float y = film->height() / 2.f;

        // Act
        Point2f raster_position{x, y};
        Ray3f ray = sensor.generate_ray(raster_position);

        // Assert
        EXPECT_THAT(ray.origin, expectedray_origin[i]);
        EXPECT_THAT(ray.direction.x(), testing::FloatEq(expected_center_ray[i].x()));
        EXPECT_THAT(ray.direction.y(), testing::FloatEq(expected_center_ray[i].y()));
        EXPECT_THAT(ray.direction.z(), testing::FloatEq(expected_center_ray[i].z()));
    }
}

TEST_F(Sensor3fTest, GivenRasterPositionAtLeftClippingPlane_WhenGeneratingRay_ThenDirectionIsParallelToLeftClippingPlane) {
    // Arrange
    auto film = sensor.film();
    float x = 0.f;
    float y = film->height() / 2.f;

    // Act
    Point2f raster_position{x, y};
    Ray3f ray = sensor.generate_ray(raster_position);

    // Assert
    EXPECT_THAT(ray.origin, (Point3f{0.f, 0.f, 0.f}));

    EXPECT_THAT(ray.direction.x(), testing::FloatNear(-std::sqrt(.5f), .0001f));
    EXPECT_THAT(ray.direction.y(), testing::FloatNear(-std::sqrt(0.f), .0001f));
    EXPECT_THAT(ray.direction.z(), testing::FloatNear(std::sqrt(.5f), .0001f));
}

TEST_F(Sensor3fTest, GivenRasterPositionAtRightClippingPlane_WhenGeneratingRay_ThenDirectionIsParallelToLeftClippingPlane) {
    // Arrange
    auto film = sensor.film();
    float x = film->width();
    float y = film->height() / 2.f;

    // Act
    Point2f raster_position{x, y};
    Ray3f ray = sensor.generate_ray(raster_position);

    // Assert
    EXPECT_THAT(ray.origin, (Point3f{0.f, 0.f, 0.f}));

    EXPECT_THAT(ray.direction.x(), testing::FloatNear(std::sqrt(.5f), .0001f));
    EXPECT_THAT(ray.direction.y(), testing::FloatNear(-std::sqrt(0.f), .0001f));
    EXPECT_THAT(ray.direction.z(), testing::FloatNear(std::sqrt(.5f), .0001f));
}

TEST_F(Sensor3fTest, ray_directions) {
    Image3f expected_camera_ray_dirs_image = load_image("okapi/tests/images/expected_camera_ray_dirs.exr");

    for(int y = 0; y < expected_camera_ray_dirs_image.height(); ++y) {
        for (int x = 0; x < expected_camera_ray_dirs_image.width(); ++x) {
            Ray3f ray = sensor.generate_ray(Point2f(x + .5f, y + .5f));

            ColorRGB3f c = expected_camera_ray_dirs_image.get_pixel(x, y);

            EXPECT_NEAR(std::abs(ray.direction.x()), c.red(), .001f);
            EXPECT_NEAR(std::abs(ray.direction.y()), c.green(), .001f);
            EXPECT_NEAR(std::abs(ray.direction.z()), c.blue(), .001f);
        }
    }
}

class Sensor3fTestFixtureBasedOnFixture :
    public Sensor3fTest,
    public ::testing::WithParamInterface<float> {
};

TEST_P(Sensor3fTestFixtureBasedOnFixture, GivenRotatedCamera_WhenGeneratingRay_ThenExpectCorrectray_direction) {
	// Arrange
	float angle = GetParam();
    sensor.set_transform(rotate_z(degree_to_radian(angle)));
	Point2f sample_position{film_plane_size_in_pixels.x() / 2.f, film_plane_size_in_pixels.y() / 2.f};

	// Act
	Ray3f ray = sensor.generate_ray(sample_position);

	// Assert
    Vector3f expectedRayDir = rotate_z(degree_to_radian(angle)) * Vector3f{.0f, .0f, 1.f};

	EXPECT_THAT(ray.direction.x(), expectedRayDir.x());
	EXPECT_THAT(ray.direction.y(), expectedRayDir.y());
	EXPECT_THAT(ray.direction.z(), expectedRayDir.z());
}

INSTANTIATE_TEST_SUITE_P(
  GivenRotatedCamera_WhenGeneratingRay_ThenExpectCorrectray_direction,
  Sensor3fTestFixtureBasedOnFixture,
  ::testing::Values(
    10.0f, 20.0f, 30.f, 45.f, 90.f, 120.f, 180.0f, 300.0f
  ));

class RasterSpaceToNormalizedDeviceCoordinatesTests : public ::testing::TestWithParam<std::tuple<Point3f, Point3f>> {
protected:
    Vector2f film_size_f{800,600};
};

TEST_P(RasterSpaceToNormalizedDeviceCoordinatesTests, GivenRasterSpace_WhenConvertingToNDC_ThenValidNDCCoordinates) {
    // Act
    auto film_size_ndc = raster_space_to_ndc(film_size_f);

    // Assert
    Point3f given_raster_positions = std::get<0>(GetParam());
	auto ndc_position = film_size_ndc * given_raster_positions;

    Point3f expected_ndc_positions = std::get<1>(GetParam());
	EXPECT_THAT(ndc_position, expected_ndc_positions);
}

INSTANTIATE_TEST_SUITE_P(
  GivenRasterSpace_WhenConvertingToNDC_ThenValidNDCCoordinates,
  RasterSpaceToNormalizedDeviceCoordinatesTests,
  ::testing::Values(
    std::make_tuple(Point3f{0.f, 0.f, 0.f},Point3f{-1.f,1.f,0.0f}),
    std::make_tuple(Point3f{0.f, 300.f, 0.f},Point3f{-1.f,0.f,0.0f}),
    std::make_tuple(Point3f{0.f, 600.f, 0.f},Point3f{-1.f,-1.f,0.0f}),
    std::make_tuple(Point3f{400.f, 300.f, 0.f},Point3f{0.f,0.f,0.0f}))
);
