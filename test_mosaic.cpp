#include <stdio.h>
#include <fcnt.h>
#include <string.h>
void nv12_nv21_mosaic(unsigned char* data,
                 int width,
                 int height,
                 int x,
                 int y,
                 int w,
                 int h,
                 int block_size) {
    int i = 0, j = 0;
    int sum = 0, count = 0;
    int ii = 0, jj = 0;
    int avg = 0;
    if (block_size <= 0 || block_size >= w || block_size >= h) {
        return;
    }
    for (j = y;j < y + h;j += block_size) {
        for (i = x;i < x + w;i += block_size) {
            sum = 0;
            count = 0;
            for (jj = j;jj < j + block_size && jj < y + h;jj++) {
                for (ii = i;ii < i + block_size && ii < x + w;ii++) {
                    sum += data[jj * width + ii];
                    count++;
                }
            }
            avg = sum / count;
            if (avg <= 0) {
                return;
            }
            count = 0;
            for (jj = j;jj < j + block_size && jj < y + h;jj++) {
                if (i + block_size >= x + w) {
                    continue;
                }
                if (count % 2) {
                    memset(data + jj * width + i, avg + 1, block_size);
                }
                else {
                    memset(data + jj * width + i, avg - 1, block_size);
                }
                ++count;
            }
        }
    }
}
int test_mosaic() {
    const int pic_w  = 1280;
    const int pic_h  = 720;
 
    /* Set up test data's path and output data's path */
    const char *inputPathname  = "./20220715_192642_1657884402662_1280x720";
    const char *outputPathname = "./XXX20220715_192642_1657884402662_1280x720";
    FILE *fin  = fopen(inputPathname , "rb+");
    if (!fin) {
        return -1;
    }
    FILE *fout = fopen(outputPathname, "wb+");
    if (!fout) {
        return -1;
    }
    const size_t size = pic_w * pic_h * 3 / 2;
 
    /* Allocate memory for uyvy */
    char *buf = (char *)malloc(size);
 
    /* Read file data to buffer */
    fread(buf, size, 1, fin);
    nv12_nv21_mosaic(buf, pic_w, pic_h, 650, 100, 300, 300, 22);
    /* Write data of buf to fout */
    fwrite(buf, size, 1, fout);
 
    /* Close the file */
    fclose(fin);
    fclose(fout);
 
    /* Free the allocation memory */
    free(buf);
    return 0;
}
int main() {
	test_mosaic();
	return 0;
}