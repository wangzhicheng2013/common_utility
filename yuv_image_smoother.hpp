#pragma once
#include <unistd.h>
#include <string.h>
#include "image_mean_filtering.hpp"
#include "image_gaussian_filtering.hpp"
enum image_format : uint8_t {
    UYVY = 0x10,
    YUYV,
    NV12,
    NV21,
    I420,
};
enum smoothing_stype : uint8_t {
    MEAN_FILTERING = 0x20,
    GAUSSIAN_FILTERING,
};

class yuv_image_smoother {
private:
    int image_width_ = 0;
    int image_height_ = 0;
    uint8_t format_ = 0;
    uint8_t smoothing_style_ = 0;
public:
    yuv_image_smoother(int w = 1920,
                       int h = 1080,
                       uint8_t format = UYVY,
                       uint8_t style = MEAN_FILTERING):
        image_width_(w),
        image_height_(h),
        format_(format),
        smoothing_style_(style) {}
    yuv_image_smoother(const yuv_image_smoother&) = delete;
    virtual ~yuv_image_smoother() {}
public:
    void process(const unsigned char* image, unsigned char* smooth_image) {
        int image_size = get_image_size();
        if (0 == image_size) {
            return;
        }
        switch (smoothing_style_)
        {
        case MEAN_FILTERING:
            {
                switch (format_)
                {
                case UYVY:
                    IMAGE_MEAN_FILTERING.process(image, smooth_image, image_width_, image_height_, uyyv_get_pixel_index);
                    break;
                case NV12:
                    IMAGE_MEAN_FILTERING.process(image, smooth_image, image_width_, image_height_, nv12_get_pixel_index);
                default:
                    break;
                }
            }
            break;
        case GAUSSIAN_FILTERING:
            {
                switch (format_)
                {
                case UYVY:
                    IMAGE_GAUSSIAN_FILTERING.process(image, smooth_image, image_width_, image_height_, uyyv_get_pixel_index);
                    break;
                case NV12:
                    IMAGE_GAUSSIAN_FILTERING.process(image, smooth_image, image_width_, image_height_, nv12_get_pixel_index);
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
    }
    inline int get_image_size() {
        switch (format_)
        {
        case UYVY:
        case YUYV:
            return 2 * image_width_ * image_height_;
            break;
        case NV12:
        case NV21:
        case I420:
            return 3 * image_width_ * image_height_ / 2;
        default:
            break;
        }
        return 0;
    }
    inline static int uyyv_get_pixel_index(int width, 
                                           int x,
                                           int y) {
        int pixels_in_a_row = width << 1;
        int y_index = y * pixels_in_a_row + (x << 1) + 1;
        return y_index;
    }
    inline static int nv12_get_pixel_index(int width, 
                                           int x,
                                           int y) {
        int y_index = y * width + x;
        return y_index;
    }
};