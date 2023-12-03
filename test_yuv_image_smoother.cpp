#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include "yuv_image_smoother.hpp"
void add_disturbance(unsigned char* image, int width, int height, get_pixel_index_func_type get_pixel_index) {
    int pixels_in_a_row = width << 1;
    srand(time(nullptr));
    int count = 0;
    int *y_index_array = (int *)malloc(sizeof(int) * width * height);
    int y_index = 0;
    for (int j = 0;j < height;j++) {
        for (int i = 0;i < width;i++) {
            y_index = j * pixels_in_a_row + (i << 1) + 1;
            y_index_array[count++] = get_pixel_index(width, i, j);
        }
    }
    int num = 1000;
    for (int i = 0;i < num;i++) {
        y_index = y_index_array[rand() % count];
        image[y_index] = 0;
    }
    free(y_index_array); 
}
int main() {
    std::string file_path;
    int width = 0;
    int height = 0;
    int format = 0;
    int style = 0;
    std::cout << "file path:";
    std::cin >> file_path;

    std::cout << "width:";
    std::cin >> width;

    std::cout << "height:";
    std::cin >> height;

    std::cout << "format(0--UYVY,1--NV12):";
    std::cin >> format;

    std::cout << "style(0--MEAN,1--GUASSIAN):";
    std::cin >> style;

    FILE* fp = fopen(file_path.c_str(), "rb");
    if (!fp) {
        std::cerr << file_path << " open failed!" << std::endl;
        return -1;
    }
    std::cout << file_path << " open success!" << std::endl;
    switch (format)
    {
    case 0:
        format = UYVY;
        break;
    case 1:
        format = NV12;
        break;
    default:
        break;
    }
    switch (style)
    {
    case 0:
        style = MEAN_FILTERING;
        break;
    case 1:
        style = GAUSSIAN_FILTERING;
        break;
    default:
        break;
    }
    yuv_image_smoother smoother(width, height, format, style);
    int size = smoother.get_image_size();
    unsigned char* image = (unsigned char*)malloc(size);
    if (nullptr == image) {
        fclose(fp);
        return -1;    
    }
    fread(image, size, 1, fp);
    fclose(fp);
    switch (format)
    {
    case UYVY:
        add_disturbance(image, width, height, yuv_image_smoother::uyyv_get_pixel_index);
        break;
    case NV12:
        add_disturbance(image, width, height, yuv_image_smoother::nv12_get_pixel_index);
        break;
    default:
        break;
    }
    // save disturbance image file
    fp = fopen("./disturbance.image", "wb");
    if (fp) {
        fwrite(image, size, 1, fp);
        fclose(fp);
    }   
    unsigned char* smooth_image = (unsigned char*)malloc(size);
    if (nullptr == smooth_image) {
        free(image);
        return -1;
    }
    memcpy(smooth_image, image, size);  // keep uv channel
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    smoother.process(image, smooth_image);
    gettimeofday(&end_time, NULL);
    double time_used = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    std::cout << "smoothing image elapse time:" << time_used << "s" << std::endl;
    free(image);
    std::cout << "save file path:";
    std::cin >> file_path;
    fp = fopen(file_path.c_str(), "wb+");
    if (!fp) {
        free(smooth_image);
        std::cerr << file_path << " open failed!" << std::endl;
        return -1;
    }
    fwrite(smooth_image, size, 1, fp);
    free(smooth_image);
    fclose(fp);

    return 0;
}
