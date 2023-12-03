#pragma once
#include "image_filter_common.hpp"
class image_mean_filtering {
private:
    coordinate surronding_points_[SURROUNDING_POINTS_NUM] = {};
private:
    image_mean_filtering() = default;
    virtual ~image_mean_filtering() {
    }
private:
    void get_surronding_points(int x, int y) {
        for (int i = 0;i < SURROUNDING_POINTS_NUM;i++) {
            surronding_points_[i].x = x + OFFSET_ARRAY[i].x_offset;
            surronding_points_[i].y = y + OFFSET_ARRAY[i].y_offset;
        }
    }
    double get_average_pixel_value(const unsigned char* image,
                                  int width,
                                  int height,
                                  get_pixel_index_func_type& get_pixel_index) {
        double average = 0;
        int cnt = 0;
        int pixel_index = 0;
        for (int i = 0;i < SURROUNDING_POINTS_NUM;i++) {
            if ((surronding_points_[i].x < 0) || (surronding_points_[i].x >= width) 
                    || (surronding_points_[i].y < 0) || (surronding_points_[i].y >= height)) {
                continue;
            }
            pixel_index = get_pixel_index(width, surronding_points_[i].x, surronding_points_[i].y);
            average += image[pixel_index];
            cnt++;
        }
        if (cnt != 0) {
            average = floor(average / cnt);
        }
        return average;
    }
public:
    static image_mean_filtering& get_instance() {
        static image_mean_filtering instance;
        return instance;
    }
    void process(const unsigned char* image,
                 unsigned char* smooth_image,
                 int width,
                 int height,
                 get_pixel_index_func_type&& get_pixel_index) {
        int x = 0, y = 0;
        int res = 0;
        int pixel_index = 0;
        for (y = 0;y < height;y++) {
            for (x = 0;x < width;x++) {
                get_surronding_points(x, y);
                res = (int)get_average_pixel_value(image, width, height, get_pixel_index);
                if (res != 0) {
                    pixel_index = get_pixel_index(width, x, y);
                    smooth_image[pixel_index] = res;
                }
            }
        }
    }
};
#define IMAGE_MEAN_FILTERING image_mean_filtering::get_instance()