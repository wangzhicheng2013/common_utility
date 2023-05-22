#include <iostream>
#include "image_utility.hpp"
int main() {
    const int pic_w  = 1920;
    const int pic_h  = 1080;

    /* Set up test data's path and output data's path */
    const char *inputPathname  = "./is_unprocessed_4_1920x1080.uyvy";
    const char *outputPathname = "./OUTis_unprocessed_4_1920x1080.uyvy";

    FILE *fin  = fopen(inputPathname , "rb+");
    if (!fin) {
        return -1;
    }
    FILE *fout = fopen(outputPathname, "wb+");
    if (!fout) {
        return -1;
    }
    size_t size = pic_w * pic_h *  2;
    /* Allocate memory for uyvy */
    char *buf = (char *)malloc(size);

    /* Read file data to buffer */
    fread(buf, 1, pic_w * pic_h *  2, fin);

    /* Draw rectangle border to  uyvy */
    G_IMAGE_UTILITY.uyvy_draw_rectangle(buf, pic_w, pic_h, 0, 0, 1920, 1080, 0, 0, 255);

    /* Write data of buf to fout */
    fwrite(buf, size, 1, fout);

    /* Free the allocation memory */
    free(buf);

    /* Close the file */
    fclose(fin);
    fclose(fout);

    return 0;
}