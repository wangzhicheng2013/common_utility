#include <iostream>
#include "image_utility.hpp"
#include "cpu_utility.hpp"
#include "math_utility.hpp"
#include "time_utility.hpp"
int test_pic() {
    const int pic_w  = 1920;
    const int pic_h  = 1536;

    /* Set up test data's path and output data's path */
    const char *inputPathname  = "./NV12_1_1920x1536.NV12";
    //const char *outputPathname = "./OUTNV12_1_1920x1536.NV12";
    const char *outputPathname = "./OUTNV12_1_1920x1536.rgb";
    FILE *fin  = fopen(inputPathname , "rb+");
    if (!fin) {
        return -1;
    }
    FILE *fout = fopen(outputPathname, "wb+");
    if (!fout) {
        return -1;
    }
    size_t size = pic_w * pic_h;

    /* Allocate memory for uyvy */
    unsigned char *buf = (unsigned char *)malloc(size * 3 / 2 * sizeof(unsigned char));
    unsigned char *rgb = (unsigned char *)malloc(size * 3 * sizeof(unsigned char));

    /* Read file data to buffer */
    fread(buf, 1, size, fin);

    /* Draw rectangle border to  uyvy */
    //char *rgb = (char *)malloc(4 * pic_w * pic_h);
    // G_IMAGE_UTILITY.nv12_to_rgb(buf, pic_w, pic_h, rgb);
    //G_IMAGE_UTILITY.NV12ToRGB((unsigned char *)buf, (unsigned char *)rgb, pic_w, pic_h);
    //G_IMAGE_UTILITY.nv12_draw_rectangle(buf, pic_w, pic_h, 0, 0, 100, 100, 0, 0, 255);
    G_IMAGE_UTILITY.nv12_to_rgb(buf, rgb, pic_w, pic_h);
    /* Write data of buf to fout */
    //fwrite(rgb, 4 * pic_w * pic_h, 1, fout);
    //fwrite(buf, size, 1, fout);
    fwrite(rgb, 3 * size, 1, fout);

    /* Close the file */
    fclose(fin);
    fclose(fout);

    /* Free the allocation memory */
    free(buf);
    free(rgb);
    //free(rgb);
    return 0;
}
int test_mosaic() {
    const int pic_w  = 1920;
    const int pic_h  = 1536;

    /* Set up test data's path and output data's path */
    const char *inputPathname  = "./NV12_1_1920x1536.NV12";
    const char *outputPathname = "./NV12_1_1920x1536.NV12";
    FILE *fin  = fopen(inputPathname , "rb+");
    if (!fin) {
        return -1;
    }
    FILE *fout = fopen(outputPathname, "wb+");
    if (!fout) {
        return -1;
    }
    size_t size = pic_w * pic_h;

    /* Allocate memory for uyvy */
    unsigned char *buf = (unsigned char *)malloc(size * 3 / 2 * sizeof(unsigned char));

    /* Read file data to buffer */
    fread(buf, 1, size, fin);
    G_IMAGE_UTILITY.nv12_nv21_mosaic(buf, pic_w, pic_h, 0, 0, 100, 100, 10);
    /* Write data of buf to fout */
    fwrite(buf, size, 1, fout);

    /* Close the file */
    fclose(fin);
    fclose(fout);

    /* Free the allocation memory */
    free(buf);
    return 0;    
}
void test_yuyv_to_nv12() {
    G_IMAGE_UTILITY.convert_yuyv_nv12("20000101_013825_946661905197_1920x1080_1920x1080.YUYV", 1920, 1080, "20000101_013825_946661905197_1920x1080_1920x1080.nv12");
}
void test_combinations() {
    std::cout << G_MATH_UTILITY.get_combinations(10, 2) << std::endl;
}
void test_get_sum_of_binomial() {
    std::cout << G_MATH_UTILITY.get_sum_of_binomials(1, 2, 3) << std::endl;
}
void test_get_cube() {
    std::cout << G_MATH_UTILITY.get_cube(18) << std::endl;
    std::cout << pow(18, 1.0 / 3) << std::endl;
}
void test_blocking_until_timeout() {
    std::cout << "before blocking!" << std::endl;
    struct timeval start_time, end_time;
    double time_used = 0;
    gettimeofday(&start_time, NULL);
    G_TIME_UTILITY.blocking_until_timeout(100000);
    gettimeofday(&end_time, NULL);
    time_used = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    printf("time used:%lf seconds\n", time_used);
    std::cout << "after blocking!" << std::endl;
}
void test_get_bit_from_char() {
    unsigned char ch = 0xE4;
    std::cout << G_CPU_UTILITY.get_bit_from_char(ch, 2) << std::endl;
}
int main() {
    //test_get_sum_of_binomial();
    //test_get_cube();
    //test_blocking_until_timeout();
    test_get_bit_from_char();
    
    return 0;
}