/*
 *  SPDX-FileCopyrightText: Copyright 2022-2023 Julian Amann <dev@vertexwahn.de>
 *  SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifndef Okapi_FilmTile_62250610_cb06_4843_8f4c_b7dfa2244458_h
#define Okapi_FilmTile_62250610_cb06_4843_8f4c_b7dfa2244458_h

#include "core/reference_counted.hpp"
#include "imaging/image.hpp"
#include "math/axis_aligned_bounding_box.hpp"
#include "math/point.hpp"
#include "math/tensor.hpp"
#include "math/vector.hpp"
#include "okapi/rendering/sensor/reconstruction_filter.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

class FilmTile {
public:
    // A FilmTile will always be clipped to the defined film_bounds
    FilmTile(const Point2i& offset,
             const Vector2i& size,
             uint32_t channel_count,
             const ReconstructionFilter* filter,
             const AxisAlignedBoundingBox2i& film_bounds) :
            channel_count_{channel_count},
            offset_{offset},
            size_{size},
            filter_{filter},
            film_bounds_{film_bounds} {
        tile_bounds_ = tile_bounds();

        int tile_pixel_count = tile_bounds().width() * tile_bounds().height();

        allocate_image_tensor_memory(channel_count);
        filter_weight_sums_.resize(tile_pixel_count);

        clear_filter_weight_sums();
    }

    FilmTile(const PropertySet& ps, uint32_t channel_count) : channel_count_{channel_count} {
        const int width = ps.get_property<int>("width");
        const int height = ps.get_property<int>("height");

        size_ = Vector2i{width, height};
        offset_ = Point2i{0,0};
        auto filter = std::dynamic_pointer_cast<ReconstructionFilter>(
                ps.get_property<ReferenceCounted<Object>>("filter"));
        filter_ = filter.get();
        film_bounds_ = AxisAlignedBoundingBox2i{Vector2i{0, 0}, Vector2i{width, height}};

        // offset_, size_, filter_, and film_bounds_ needs to be set before calling this
        tile_bounds_ = tile_bounds();

        int tile_pixel_count = tile_bounds().width() * tile_bounds().height();

        allocate_image_tensor_memory(channel_count);
        filter_weight_sums_.resize(tile_pixel_count);

        clear_filter_weight_sums();
    }

    virtual ~FilmTile() = default;

    void allocate_image_tensor_memory(int channel_count) {
        channel_count_ = channel_count; // Update channel count!

        size_t height = (size_t)tile_bounds().height();
        size_t width = (size_t)tile_bounds().width();

        //tensor_.resize({height, width, channel_count_});
        xt::xarray<float>::shape_type shape = {height, width, channel_count_};
        tensor_ = xt::xarray<float>::from_shape(shape);

        clear_image_tensor_values();
    }

    void clear() {
        clear_image_tensor_values();
        clear_filter_weight_sums();
    }

    void clear_image_tensor_values() {
        size_t height = (size_t)tile_bounds().height();
        size_t width = (size_t)tile_bounds().width();

        for(int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                for (int c = 0; c < channel_count_; ++c) {
                    tensor_[{y, x, c}] = 0.f;
                }
            }
        }
    }

    void clear_filter_weight_sums() {
        int tile_pixel_count = tile_bounds().width() * tile_bounds().height();

        for (int i = 0; i < tile_pixel_count; ++i) {
            filter_weight_sums_[i] = 0.0f;
        }
    }

    /// Query dimensions
    const Point2i& offset() const {
        return offset_;
    }
    const Vector2i& size() const {
        return size_;
    }
    int width() const {
        return size_.x();
    }
    int height() const {
        return size_.y();
    }

    AxisAlignedBoundingBox2i tile_bounds() const {
        AxisAlignedBoundingBox2i b1 = sample_bounds(
                Point2f{static_cast<float>(offset_.x()),
                        static_cast<float>(offset_.y())});
        AxisAlignedBoundingBox2i b2 = sample_bounds(
                Point2f{static_cast<float>(offset_.x() + size_.x()),
                        static_cast<float>(offset_.y() + size_.y())});

        AxisAlignedBoundingBox2i tile_bounds{b1.min_, b2.max_};

        return tile_bounds;
    }

    AxisAlignedBoundingBox2i sample_bounds(const Point2f &sample_position) const {
        AxisAlignedBoundingBox2i bounds;

        bounds.min_.x() = sample_position.x() - filter_->radius().x() + .5f;
        bounds.min_.y() = sample_position.y() - filter_->radius().y() + .5f;

        bounds.max_.x() = sample_position.x() + filter_->radius().x() - .5f + 1.f;
        bounds.max_.y() = sample_position.y() + filter_->radius().y() - .5f + 1.f;

        // clip to film bounds
        bounds.min_.x() = std::max(bounds.min_.x(), 0);
        bounds.min_.y() = std::max(bounds.min_.y(), 0);

        bounds.max_.x() = std::min(bounds.max_.x(), film_bounds_.max_.x());
        bounds.max_.y() = std::min(bounds.max_.y(), film_bounds_.max_.y());

        return bounds;
    }

    void add_sample(const Point2f& sample_position, float* aovs) {
        AxisAlignedBoundingBox2i bounds = sample_bounds(sample_position);

        for(int y = bounds.min_.y(); y < bounds.max_.y(); ++y) {
            for(int x = bounds.min_.x(); x < bounds.max_.x(); ++x) {
                float dx = x + .5f - sample_position.x();
                float dy = y + .5f - sample_position.y();

                float filter_weight = filter_->evaluate(Point2f{dx, dy});

                int left_border = offset_.x() - tile_bounds_.min_.x();
                int top_boarder = offset().y() - tile_bounds_.min_.y();
                assert(left_border >= 0);
                assert(top_boarder >= 0);
                int tx = x + left_border - offset_.x();
                int ty = y + top_boarder - offset_.y();

                assert(tx >= 0);
                assert(ty >= 0);

                if(filter_weight > 0.f) {
                    filter_weight_sums_[tx + ty * tile_bounds_.width()] += filter_weight;

                    for(int c = 0; c < channel_count_; ++c) {
                        float value = filter_weight * aovs[c];
                        tensor_[{ty,tx,c}] += value;
                    }
                }
            }
        }
    }

    void add_tile(const FilmTile& tile) {
        AxisAlignedBoundingBox2i tile_bounds = tile.tile_bounds();

        for(int y = tile_bounds.min_.y(); y < tile_bounds.max_.y(); ++y) {
            for(int x = tile_bounds.min_.x(); x < tile_bounds.max_.x(); ++x) {
                int tile_x = x - tile_bounds.min_.x();
                int tile_y = y - tile_bounds.min_.y();
                float filter_weight = tile.filter_weight_sums_[tile_x + tile_y * tile_bounds.width()];

                if(filter_weight > 0.f) {
                    filter_weight_sums_[x + y * size_.x()] += filter_weight;

                    for(int c = 0; c < channel_count_; ++c) {
                        float value = tile.tensor_[{tile_y,tile_x,c}];
                        tensor_[{y,x,c}] += value;
                    }
                }
            }
        }
    }

    void add_sample_slow_but_correct(const Point2f &sample_position, float* aovs) {
        // which pixel needs an update? -> compute bound avoid this brute force approach
        for(int y = 0; y < height(); ++y) {
            for(int x = 0; x < width(); ++x) {
                float dx = x + .5f - sample_position.x();
                float dy = y + .5f - sample_position.y();

                if(std::abs(dx) > filter_->radius().x())
                    continue;
                if(std::abs(dx) > filter_->radius().y())
                    continue;

                float filter_weight = filter_->evaluate(Point2f{dx,dy});

                int left_border = offset_.x() - tile_bounds_.min_.x();
                int top_boarder = offset().y() - tile_bounds_.min_.y();
                assert(left_border >= 0);
                assert(top_boarder >= 0);
                int tx = x + left_border - offset_.x();
                int ty = y + top_boarder - offset_.y();

                if(filter_weight > 0.f) {
                    filter_weight_sums_[tx + ty * tile_bounds_.width()] += filter_weight;

                    for(int c = 0; c < channel_count_; ++c) {
                        tensor_[{y,x,c}] += filter_weight * aovs[c];
                    }
                }
            }
        }
    }

    /// Query weights and colors
    ReferenceCounted<Image3f> image(int start_index = 0) const {
        ReferenceCounted<Image3f> image = make_reference_counted<Image3f>(size_.x(), size_.y());

        for(int y = 0; y < size_.y(); ++y) {
            for(int x = 0; x < size_.x(); ++x) {
                int left_border = offset_.x() - tile_bounds_.min_.x();
                int top_boarder = offset_.y() - tile_bounds_.min_.y();
                assert(left_border >= 0);
                assert(top_boarder >= 0);
                int tx = x + left_border;
                int ty = y + top_boarder;

                float total_weight = filter_weight_sums_[tx + ty * tile_bounds_.width()];

                if(total_weight != 0.f) {
                    ColorRGB3f color{
                        tensor_[{ty,tx,0 + start_index}],
                        tensor_[{ty,tx,1 + start_index}],
                        tensor_[{ty,tx,2 + start_index}]
                    };

                    image->set_pixel(x, y, (1.f / total_weight) * color);
                }
            }
        }

        return image;
    }

    // This method exist only for testing reasons
    std::vector<ColorRGB3f> radiance_sums(int start_index = 0) const {
        size_t height = (size_t)tile_bounds().height();
        size_t width = (size_t)tile_bounds().width();

        std::vector<ColorRGB3f> radiance_sums;

        for(int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                radiance_sums.push_back(
                    ColorRGB3f{
                        tensor_[{y, x, 0 + start_index}],
                        tensor_[{y, x, 1 + start_index}],
                        tensor_[{y, x, 2 + start_index}]
                    }
                );
            }
        }

        return radiance_sums;
    }

    // This method exist only for testing reasons
    // * = boarder
    // point = (0,0) is relative to tile. Offset is only used to calculate boarder but not relevant for sample position.
    // +------+------+------+
    // |      |      |      |
    // |     *|******|*     |
    // +------+------+------+
    // |     *|(0|0) |*     |
    // |     *|      |*     |
    // +------+------+------+
    // |     *|******|*     |
    // |      |      |      |
    // +------+------+------+
    void sample_color(const Point2i& point, float *aovs) const {
        int x = point.x();
        int y = point.y();

        int left_border = offset_.x() - tile_bounds_.min_.x();
        int top_boarder = offset_.y() - tile_bounds_.min_.y();
        assert(left_border >= 0);
        assert(top_boarder >= 0);
        int tx = x + left_border;
        int ty = y + top_boarder;

        float total_filter_weight = filter_weight_sums_[tx + ty * tile_bounds_.width()];

        if (total_filter_weight != 0.f) {
            for (int c = 0; c < channel_count_; ++c) {
                *aovs++ = tensor_[{ty, tx, c}] / total_filter_weight;
            }
        } else {
            for (int c = 0; c < channel_count_; ++c) {
                *aovs++ = 0;
            }
        }
    }

    // Only used for testing proposes
    std::vector<float> filter_weight_sums() const {
        return filter_weight_sums_;
    }

    uint32_t channel_count() const {
        return channel_count_;
    }

private:
    TensorXf tensor_; // r,g,b, aovs
    uint32_t channel_count_; // number of color channels (note filter_weight_sums_ is not counted here since its not a color channel)
    std::vector<float> filter_weight_sums_;

    Point2i offset_;
    Vector2i size_;

    const ReconstructionFilter* filter_;

    AxisAlignedBoundingBox2i tile_bounds_;
    AxisAlignedBoundingBox2i film_bounds_;
};

DE_VERTEXWAHN_END_NAMESPACE

#endif // end define Okapi_FilmTile_62250610_cb06_4843_8f4c_b7dfa2244458_h
