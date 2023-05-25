#include <iostream>
#include "image_utility.hpp"
int main() {
    const int pic_w  = 1920;
    const int pic_h  = 1536;

    /* Set up test data's path and output data's path */
    const char *inputPathname  = "./NV12_1_1920x1536.NV12";
    const char *outputPathname = "./OUTNV12_1_1920x1536.NV12";

    FILE *fin  = fopen(inputPathname , "rb+");
    if (!fin) {
        return -1;
    }
    FILE *fout = fopen(outputPathname, "wb+");
    if (!fout) {
        return -1;
    }
    size_t size = pic_w * pic_h * 3 / 2;

    /* Allocate memory for uyvy */
    char *buf = (char *)malloc(size);

    /* Read file data to buffer */
    fread(buf, 1, size, fin);

    /* Draw rectangle border to  uyvy */
    //char *rgb = (char *)malloc(4 * pic_w * pic_h);
    // G_IMAGE_UTILITY.nv12_to_rgb(buf, pic_w, pic_h, rgb);
    //G_IMAGE_UTILITY.NV12ToRGB((unsigned char *)buf, (unsigned char *)rgb, pic_w, pic_h);
    G_IMAGE_UTILITY.nv12_draw_rectangle(buf, pic_w, pic_h, 0, 0, 100, 100, 0, 0, 255);
    /* Write data of buf to fout */
    //fwrite(rgb, 4 * pic_w * pic_h, 1, fout);
    fwrite(buf, size, 1, fout);

    /* Close the file */
    fclose(fin);
    fclose(fout);

    /* Free the allocation memory */
    free(buf);
    //free(rgb);
    return 0;
}