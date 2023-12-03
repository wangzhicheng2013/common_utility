#include "image_filter_common.hpp"
const static double GAUSSIAN_FILTERING_MATRIX[SURROUNDING_POINTS_NUM] = {
    0.05, 0.1, 0.05,
    0.1,  0.4, 0.1,
    0.05, 0.1, 0.05
};
class image_gaussian_filtering {
private:
    coordinate surronding_points_[SURROUNDING_POINTS_NUM] = {};
private:
    image_gaussian_filtering() = default;
    virtual ~image_gaussian_filtering() {
    }
private:
    bool get_surronding_points(int x, 
                               int y,
                               int width,
                               int height) {
        for (int i = 0;i < SURROUNDING_POINTS_NUM;i++) {
            surronding_points_[i].x = x + OFFSET_ARRAY[i].x_offset;
            surronding_points_[i].y = y + OFFSET_ARRAY[i].y_offset;
            if ((surronding_points_[i].x < 0) || (surronding_points_[i].x >= width) 
                    || (surronding_points_[i].y < 0) || (surronding_points_[i].y >= height)) {
                return false;
            }
        }
        return true;
    }
    double get_gussian_pixel_value(const unsigned char* image,
                                  int x,
                                  int y,
                                  int width,
                                  int height,
                                  get_pixel_index_func_type& get_pixel_index) {
        if (false == get_surronding_points(x, y, width, height)) {
            return -1;
        }
        double value = 0;
        int pixel_index = 0;
        for (int i = 0;i < SURROUNDING_POINTS_NUM;i++) {
            pixel_index = get_pixel_index(width, surronding_points_[i].x, surronding_points_[i].y);
            value += image[pixel_index] * GAUSSIAN_FILTERING_MATRIX[i];
        }
        return value;
    }
public:
    static image_gaussian_filtering& get_instance() {
        static image_gaussian_filtering instance;
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
                res = (int)get_gussian_pixel_value(image, x, y, width, height, get_pixel_index);
                if (res < 0) {
                    continue;
                }
                pixel_index = get_pixel_index(width, x, y);
                smooth_image[pixel_index] = res;
            }
        }
    }
};
#define IMAGE_GAUSSIAN_FILTERING image_gaussian_filtering::get_instance()