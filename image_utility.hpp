#pragma once
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <math.h>
#include "single_instance.hpp"
typedef struct __tag_ASVL_OFFSCREEN {
    unsigned int u32PixelArrayFormat;
    int i32Width;
    int i32Height;
    unsigned char* ppu8Plane[4];
    int pi32Pitch[4];
}ASVLOFFSCREEN, *LPASVLOFFSCREEN;
enum IMAGE_FORMAT {
    /*8 bit Y plane followed by 8 bit 2x2 subsampled UV planes*/
    ASVL_PAF_NV12 = 0x801,
    /*8 bit Y plane followed by 8 bit 2x2 subsampled VU planes*/
    ASVL_PAF_NV21 = 0x802,
    ASVL_PAF_UYVY = 0x503,
};
typedef struct __tag_rect {
	int left;
	int top;
	int right;
	int bottom;
} MRECT, *PMRECT;
class image_utility {
public:
    // Correction of abscissa according to width
    void fix_x(int &x, int width) {
        if (x < 0) {
            x = 0;
        }
        else if (x >= width) {
            x = width - 1;
        }
    }
    void fix_y(int &y, int height) {
        if (y < 0) {
            y = 0;
        }
        else if (y >= height) {
            y = height - 1;
        }    
    }
    // Correct rectangular area according to image width and height
    bool fix(int &left, int &top, int &right, int &bottom, int width, int height) {
        fix_x(left, width);
        fix_x(right, width);

        fix_y(top, height);
        fix_y(bottom, height);
        int w = right - left;
        int h = bottom - top;
        if (w <= 0 || h <= 0) {
            return false;
        }
        return true;    
    }
    // blacken the designated area of nv12 image
    // left:Abscissa of upper left corner of rectangular area
    // top:Vertical coordinate of upper left corner of rectangular area
    // right:Abscissa of lower right corner of rectangular area
    // bottom:Vertical coordinate of the lower right corner of the rectangular area
    // width:Original image width
    // height:Original image height
    // ppu8Plane_y:Y channel address
    // ppu8Plane_uv:UV channel address
    void blackening_nv12(int left, 
                        int top,
                        int right, 
                        int bottom,
                        int width,
                        int height,
                        unsigned char *ppu8Plane_y,
                        unsigned char *ppu8Plane_uv) {
        if (!ppu8Plane_y || !ppu8Plane_uv) {
            return;
        }
        if (!fix(left, top, right, bottom, width, height)) {
            return;
        }
        int w = right - left;
        for (int j = top;j <= bottom;j++) {
            memset(ppu8Plane_y + left + j * width, 0, w);
            memset(ppu8Plane_uv + left + j / 2 * width, 128, w);
        }
    }
    bool read_image_data(const char *file_path, ASVLOFFSCREEN *image) {
        if (!file_path || !image) {
            return false;
        }
        int  frame_length = 0;
        switch (image->u32PixelArrayFormat)
        {
        case ASVL_PAF_NV12:
        case ASVL_PAF_NV21:
            frame_length = image->i32Height * image->pi32Pitch[0] * 3 / 2;
            break;
        default:
            frame_length = image->i32Height * image->pi32Pitch[0] * 3;
            break;
        }
        if (0 == frame_length) {
            return false;
        }
        if (image->ppu8Plane[0] != nullptr) {
            free(image->ppu8Plane[0]);
        }
        image->ppu8Plane[0]  = (unsigned char*)malloc(frame_length);
        if (nullptr == image->ppu8Plane[0]) {
            return false;
        }
        image->ppu8Plane[1] = image->ppu8Plane[0] + image->i32Height * image->pi32Pitch[0];
        FILE *pFile = fopen(file_path, "rb");
        if(nullptr != pFile){
            fread(image->ppu8Plane[0], 1, frame_length, pFile);
            fclose(pFile);
            return true;
        }
        free(image->ppu8Plane[0]);
        image->ppu8Plane[0] = image->ppu8Plane[1] = nullptr;
        return false;
    }
    bool convert_i420_nv12(const char *i420_file_path, size_t width, size_t height, const char *nv12_file_path) {
        if (!i420_file_path || !nv12_file_path) {
            return false;
        }
        FILE *fp = fopen(i420_file_path, "rb");
        if (!fp) {
            return false;
        }
        fseek(fp, 0, SEEK_END);
        size_t file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        size_t y_size = width * height;
        size_t uv_size = y_size / 4;
        if (file_size != (y_size + uv_size * 2)) {
            fclose(fp);
            return false;
        }
        char *i420_content = (char *)malloc(sizeof(char) * file_size);
        if (!i420_content) {
            fclose(fp);
            return false;
        }
        if (file_size != fread(i420_content, 1, file_size, fp)) {
            free(i420_content);
            fclose(fp);
            return false;
        }
        fclose(fp);
        // convert i420 to nv12
        char *nv12_content = (char *)malloc(sizeof(char) * file_size);
        if (!nv12_content) {
            free(i420_content);
            return false;
        }
        // copy y channel
        memcpy(nv12_content, i420_content, y_size);
        // copy uv channel
        char *i420_u_base = i420_content + y_size;
        char *i420_v_base = i420_u_base + uv_size;
        char *nv12_uv_base = nv12_content + y_size;
        int i = 0, j = 0;
        for (;i < uv_size;i++) {
            nv12_uv_base[j] = i420_u_base[i];
            nv12_uv_base[j + 1] = i420_v_base[i];
            j += 2;
        }
        free(i420_content);
        fp = fopen(nv12_file_path, "wb");
        if (!fp) {
            free(nv12_content);
            return false;
        }
        if (file_size != fwrite(nv12_content, 1, file_size, fp)) {
            free(nv12_content);
            fclose(fp);
            return false;
        }
        free(nv12_content);
        fclose(fp);
        return true;
    }
    bool convert_i420_nv21(const char *i420_file_path, size_t width, size_t height, const char *nv21_file_path) {
        if (!i420_file_path || !nv21_file_path) {
            return false;
        }
        FILE *fp = fopen(i420_file_path, "rb");
        if (!fp) {
            return false;
        }
        fseek(fp, 0, SEEK_END);
        size_t file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        size_t y_size = width * height;
        size_t uv_size = y_size / 4;
        if (file_size != (y_size + uv_size * 2)) {
            fclose(fp);
            return false;
        }
        char *i420_content = (char *)malloc(sizeof(char) * file_size);
        if (!i420_content) {
            fclose(fp);
            return false;
        }
        if (file_size != fread(i420_content, 1, file_size, fp)) {
            free(i420_content);
            fclose(fp);
            return false;
        }
        fclose(fp);
        // convert i420 to nv21
        char *nv21_content = (char *)malloc(sizeof(char) * file_size);
        if (!nv21_content) {
            free(i420_content);
            return false;
        }
        // copy y channel
        memcpy(nv21_content, i420_content, y_size);
        // copy uv channel
        char *i420_u_base = i420_content + y_size;
        char *i420_v_base = i420_u_base + uv_size;
        char *nv21_uv_base = nv21_content + y_size;
        int i = 0, j = 0;
        for (;i < uv_size;i++) {
            nv21_uv_base[j] = i420_v_base[i];
            nv21_uv_base[j + 1] = i420_u_base[i];
            j += 2;
        }
        free(i420_content);
        fp = fopen(nv21_file_path, "wb");
        if (!fp) {
            free(nv21_content);
            return false;
        }
        if (file_size != fwrite(nv21_content, 1, file_size, fp)) {
            free(nv21_content);
            fclose(fp);
            return false;
        }
        free(nv21_content);
        fclose(fp);
        return true;
    }

    bool convert_uyvy_nv12(const char *uyvy_file_path, size_t width, size_t height, const char *nv12_file_path) {
        if (!uyvy_file_path || !nv12_file_path) {
            return false;
        }
        FILE *fp = fopen(uyvy_file_path, "rb");
        if (!fp) {
            return false;
        }
        fseek(fp, 0, SEEK_END);
        size_t file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        size_t frame_size = width * height * 2;
        if (file_size != frame_size) {
            fclose(fp);
            return false;
        }
        char *uyvy_content = (char *)malloc(sizeof(char) * file_size);
        if (!uyvy_content) {
            fclose(fp);
            return false;
        }
        if (file_size != fread(uyvy_content, 1, file_size, fp)) {
            free(uyvy_content);
            fclose(fp);
            return false;
        }
        fclose(fp);
        // convert uyvy to nv12
        frame_size = width * height * 3 / 2;
        char *nv12_content = (char *)malloc(sizeof(char) * frame_size);
        if (!nv12_content) {
            free(uyvy_content);
            return false;
        }
        size_t y_size = width * height;
        size_t pixels_in_a_row = width * 2;
        char *nv12_y_ptr = nv12_content;
        char *nv12_uv_ptr = nv12_content + y_size;
        int lines = 0;
        for (int i = 0;i < file_size;i += 4) {
            // copy y channel
            *nv12_y_ptr++ = uyvy_content[i + 1];
            *nv12_y_ptr++ = uyvy_content[i + 3];
            if (0 == i % pixels_in_a_row) {
                ++lines;
            }
            if (lines % 2) {       // extract the UV value of odd rows
                // copy uv channel
                *nv12_uv_ptr++ = uyvy_content[i];
                *nv12_uv_ptr++ = uyvy_content[i + 2];
            }
        }
        free(uyvy_content);
        fp = fopen(nv12_file_path, "wb");
        if (!fp) {
            free(nv12_content);
            return false;
        }
        if (frame_size != fwrite(nv12_content, 1, frame_size, fp)) {
            free(nv12_content);
            fclose(fp);
            return false;
        }
        free(nv12_content);
        fclose(fp);
        return true;
    }
    bool convert_uyvy_nv21(const char *uyvy_file_path, size_t width, size_t height, const char *nv21_file_path) {
        if (!uyvy_file_path || !nv21_file_path) {
            return false;
        }
        FILE *fp = fopen(uyvy_file_path, "rb");
        if (!fp) {
            return false;
        }
        fseek(fp, 0, SEEK_END);
        size_t file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        size_t frame_size = width * height * 2;
        if (file_size != frame_size) {
            fclose(fp);
            return false;
        }
        char *uyvy_content = (char *)malloc(sizeof(char) * file_size);
        if (!uyvy_content) {
            fclose(fp);
            return false;
        }
        if (file_size != fread(uyvy_content, 1, file_size, fp)) {
            free(uyvy_content);
            fclose(fp);
            return false;
        }
        fclose(fp);
        // convert uyvy to nv12
        frame_size = width * height * 3 / 2;
        char *nv21_content = (char *)malloc(sizeof(char) * frame_size);
        if (!nv21_content) {
            free(uyvy_content);
            return false;
        }
        size_t y_size = width * height;
        size_t pixels_in_a_row = width * 2;
        char *nv21_y_ptr = nv21_content;
        char *nv21_uv_ptr = nv21_content + y_size;
        int lines = 0;
        for (int i = 0;i < file_size;i += 4) {
            // copy y channel
            *nv21_y_ptr++ = uyvy_content[i + 1];
            *nv21_y_ptr++ = uyvy_content[i + 3];
            if (0 == i % pixels_in_a_row) {
                ++lines;
            }
            if (lines % 2) {       // extract the UV value of odd rows
                // copy uv channel
                *nv21_uv_ptr++ = uyvy_content[i + 2];
                *nv21_uv_ptr++ = uyvy_content[i];
            }
        }
        free(uyvy_content);
        fp = fopen(nv21_file_path, "wb");
        if (!fp) {
            free(nv21_content);
            return false;
        }
        if (frame_size != fwrite(nv21_content, 1, frame_size, fp)) {
            free(nv21_content);
            fclose(fp);
            return false;
        }
        free(nv21_content);
        fclose(fp);
        return true;
    }
    void nv12_draw_rectangle(char *pic, 
                    int pic_w,
                    int pic_h, 
                    int rect_x, 
                    int rect_y, 
                    int rect_w,
                    int rect_h,
                    int R,
                    int G,
                    int B) {
        /* Set up the rectangle border size */
        static const int border = 5;
        /* RGB convert YUV */
        int Y =  0.299  * R + 0.587  * G + 0.114  * B;
        int U = -0.1687 * R + 0.3313 * G + 0.5    * B + 128;
        int V =  0.5    * R - 0.4187 * G - 0.0813 * B + 128;
        /* Locking the scope of rectangle border range */
        int j = 0, k = 0;
        int size = pic_w * pic_h;
        int size1 = pic_w * 2;
        int y_index = 0;
        int u_index = 0;
        int v_index = 0;
        for (j = rect_y;j < rect_y + rect_h;j++) {
            for (k = rect_x;k < rect_x + rect_w;k++) {
                if (k < (rect_x + border) || k > (rect_x + rect_w - border) || j < (rect_y + border) || j > (rect_y + rect_h - border)) {
                    /* Components of YUV's storage address index */
                    y_index = j * pic_w + k;
                    //u_index = (y_index / 2 - pic_w / 2 * ((j + 1) / 2)) * 2 + size;
                    u_index = (y_index / size1) * pic_w + (((y_index % pic_w) / 2) << 1) + size;
                    //printf("y:%d, u:%d, u1:%d\n", y_index, u_index, u_index1);
                    v_index = u_index + 1;
                    /* set up YUV's conponents value of rectangle border */
                    pic[y_index] =  Y;
                    pic[u_index] =  U;
                    pic[v_index] =  V;
                }
            }
        }
    }
    void nv21_draw_rectangle(char *pic, 
                    int pic_w,
                    int pic_h, 
                    int rect_x, 
                    int rect_y, 
                    int rect_w,
                    int rect_h,
                    int R,
                    int G,
                    int B) {
        /* Set up the rectangle border size */
        static const int border = 5;
        /* RGB convert YUV */
        int Y =  0.299  * R + 0.587  * G + 0.114  * B;
        int U = -0.1687 * R + 0.3313 * G + 0.5    * B + 128;
        int V =  0.5    * R - 0.4187 * G - 0.0813 * B + 128;
        /* Locking the scope of rectangle border range */
        int j = 0, k = 0;
        int size = pic_w * pic_h;
        int size1 = pic_w * 2;
        int y_index = 0;
        int u_index = 0;
        int v_index = 0;
        for (j = rect_y;j < rect_y + rect_h;j++) {
            for (k = rect_x;k < rect_x + rect_w;k++) {
                if (k < (rect_x + border) || k > (rect_x + rect_w - border) || j < (rect_y + border) || j > (rect_y + rect_h - border)) {
                    /* Components of YUV's storage address index */
                    y_index = j * pic_w + k;
                    //u_index = (y_index / 2 - pic_w / 2 * ((j + 1) / 2)) * 2 + size;
                    v_index = (y_index / size1) * pic_w + (((y_index % pic_w) / 2) << 1) + size;
                    u_index = v_index + 1;
                    /* set up YUV's conponents value of rectangle border */
                    pic[y_index] =  Y;
                    pic[u_index] =  U;
                    pic[v_index] =  V;
                }
            }
        }
    }
    void uyvy_draw_rectangle(char *pic, 
                    int pic_w,
                    int pic_h, 
                    int rect_x, 
                    int rect_y, 
                    int rect_w,
                    int rect_h,
                    int R,
                    int G,
                    int B) {
        /* Set up the rectangle border size */
        static const int border = 5;
        /* RGB convert YUV */
        int Y =  0.299  * R + 0.587  * G + 0.114  * B;
        int U = -0.1687 * R + 0.3313 * G + 0.5    * B + 128;
        int V =  0.5    * R - 0.4187 * G - 0.0813 * B + 128;
        /* Locking the scope of rectangle border range */
        int j = 0, k = 0;
        int n = 0;
        int y_index = 0;
        for (j = rect_y;j < rect_y + rect_h;j++) {
            for (k = rect_x;k < rect_x + rect_w;k++) {
                if (k < (rect_x + border) || k > (rect_x + rect_w - border) || j < (rect_y + border) || j > (rect_y + rect_h - border)) {
                    /* Components of YUV's storage address index */
                    n = j * pic_w + k;
                    y_index = (n << 1) - 1;
                    pic[y_index] = Y;
                    if (1 == y_index % 4) {
                        pic[y_index - 1] = U;
                        pic[y_index + 1] = V;
                    }
                }
            }
        }
    }
    void nv12_fill_rectangle(char *pic, 
                    int pic_w,
                    int pic_h, 
                    int rect_x, 
                    int rect_y, 
                    int rect_w,
                    int rect_h,
                    int R,
                    int G,
                    int B) {
        /* Set up the rectangle border size */
        static const int border = 5;
        /* RGB convert YUV */
        int Y =  0.299  * R + 0.587  * G + 0.114  * B;
        int U = -0.1687 * R + 0.3313 * G + 0.5    * B + 128;
        int V =  0.5    * R - 0.4187 * G - 0.0813 * B + 128;
        /* Locking the scope of rectangle border range */
        int j = 0, k = 0;
        int size = pic_w * pic_h;
        int size1 = pic_w * 2;
        int y_index = 0;
        int u_index = 0;
        int v_index = 0;
        for (j = rect_y;j < rect_y + rect_h;j++) {
            for (k = rect_x;k < rect_x + rect_w;k++) {
                /* Components of YUV's storage address index */
                y_index = j * pic_w + k;
                //u_index = (y_index / 2 - pic_w / 2 * ((j + 1) / 2)) * 2 + size;
                u_index = (y_index / size1) * pic_w + (((y_index % pic_w) / 2) << 1) + size;
                //printf("y:%d, u:%d, u1:%d\n", y_index, u_index, u_index1);
                v_index = u_index + 1;
                /* set up YUV's conponents value of rectangle border */
                pic[y_index] =  Y;
                pic[u_index] =  U;
                pic[v_index] =  V;
            }
        }
    }
    void nv21_fill_rectangle(char *pic, 
                    int pic_w,
                    int pic_h, 
                    int rect_x, 
                    int rect_y, 
                    int rect_w,
                    int rect_h,
                    int R,
                    int G,
                    int B) {
        /* Set up the rectangle border size */
        static const int border = 5;
        /* RGB convert YUV */
        int Y =  0.299  * R + 0.587  * G + 0.114  * B;
        int U = -0.1687 * R + 0.3313 * G + 0.5    * B + 128;
        int V =  0.5    * R - 0.4187 * G - 0.0813 * B + 128;
        /* Locking the scope of rectangle border range */
        int j = 0, k = 0;
        int size = pic_w * pic_h;
        int size1 = pic_w * 2;
        int y_index = 0;
        int u_index = 0;
        int v_index = 0;
        for (j = rect_y;j < rect_y + rect_h;j++) {
            for (k = rect_x;k < rect_x + rect_w;k++) {
                /* Components of YUV's storage address index */
                y_index = j * pic_w + k;
                //u_index = (y_index / 2 - pic_w / 2 * ((j + 1) / 2)) * 2 + size;
                v_index = (y_index / size1) * pic_w + (((y_index % pic_w) / 2) << 1) + size;
                u_index = v_index + 1;
                /* set up YUV's conponents value of rectangle border */
                pic[y_index] =  Y;
                pic[u_index] =  U;
                pic[v_index] =  V;
            }
        }
    }
    void uyvy_fill_rectangle(char *pic, 
                    int pic_w,
                    int pic_h, 
                    int rect_x, 
                    int rect_y, 
                    int rect_w,
                    int rect_h,
                    int R,
                    int G,
                    int B) {
        /* Set up the rectangle border size */
        static const int border = 5;
        /* RGB convert YUV */
        int Y =  0.299  * R + 0.587  * G + 0.114  * B;
        int U = -0.1687 * R + 0.3313 * G + 0.5    * B + 128;
        int V =  0.5    * R - 0.4187 * G - 0.0813 * B + 128;
        /* Locking the scope of rectangle border range */
        int j = 0, k = 0;
        int n = 0;
        int y_index = 0;
        for (j = rect_y;j < rect_y + rect_h;j++) {
            for (k = rect_x;k < rect_x + rect_w;k++) {
                /* Components of YUV's storage address index */
                n = j * pic_w + k;
                y_index = (n << 1) - 1;
                pic[y_index] = Y;
                if (1 == y_index % 4) {
                    pic[y_index - 1] = U;
                    pic[y_index + 1] = V;
                }
            }
        }
    }
    void nv12_to_rgb(unsigned char* nv12_data, 
                     unsigned char* rgb_data,
                     int width,
                     int height) {
        int y_size = width * height;
        int uv_size = y_size / 2;
        int y_pos = 0, u_pos = 0, v_pos = 0;
        int rgb_pos = 0;
        unsigned char* y_data = nv12_data;
        unsigned char* uv_data = nv12_data + y_size;
        unsigned char y = 0, u = 0, v = 0;
        unsigned char r = 0, g = 0, b = 0;
        for (int i = 0;i < height;i++) {
            for (int j = 0;j < width;j++) {
                y_pos = i * width + j;
                y = y_data[y_pos];

                u_pos = (i / 2) * width + (j / 2) * 2;
                u = uv_data[u_pos];

                v_pos = u_pos + 1;
                v = uv_data[v_pos];

                r = y + 1.370705 * (v - 128);
                g = y - 0.698001 * (v - 128) - 0.337633 * (u - 128);
                b = y + 1.732446 * (u - 128);

                r = r < 0 ? 0 : (r > 255 ? 255 : r);
                g = g < 0 ? 0 : (g > 255 ? 255 : g);
                b = b < 0 ? 0 : (b > 255 ? 255 : b);

                rgb_pos = y_pos * 3;
                rgb_data[rgb_pos] = r;
                rgb_data[rgb_pos + 1] = g;
                rgb_data[rgb_pos + 2] = b;
            }
        }
    }
	void get_fixed_size(int& display_width, 
                    int& display_height,
                    int source_width,
                    int source_height,
                    int screen_width,
                    int screen_height) {
		if ((screen_width * source_height) > (screen_height * source_width)) {
			display_width = screen_height * source_width / source_height;
			display_height = screen_height;
		}
		else {
			display_width = screen_width;
			display_height = screen_width * source_height / source_width;
		}
	}
    void nv12_mosaic(unsigned char* nv12_data,
                     int width,
                     int height,
                     int block_size) {
        int x = 0, y = 0, i = 0, j = 0;
        int y_size = width * height;
        int uv_size = y_size / 2;
        int y_sum = 0, u_sum = 0, v_sum = 0;
        int count = 0;
        int y_index = 0;
        int uv_index = 0;
        int y_avg = 0, u_avg = 0, v_avg = 0;
        for (y = 0;y < height;y += block_size) {
            for (x = 0;x < width;x += block_size) {
                y_sum = 0, u_sum = 0, v_sum = 0;
                count = 0;
                for (j = y;j < y + block_size && j < height;j++) {
                    for (i = x;i < x + block_size && i < width;i++) {
                        y_index = j * width + i;
                        uv_index = (j / 2) * width + (i / 2) * 2;
                        y_sum += nv12_data[y_index];
                        if ((0 == i % 2) && (0 == j % 2)) {
                            u_sum += nv12_data[uv_size + uv_index];
                            v_sum += nv12_data[uv_size + uv_index + 1];
                        }
                        count++;
                    }
                }
                y_avg = y_sum / count;
                u_avg = u_sum / (count / 4);        // 4Y -- 1UV
                v_avg = v_sum / (count / 4);
                for (j = y;j < y + block_size && j < height;j++) {
                    for (i = x;i < x + block_size && i < width;i++) {
                        y_index = j * width + i;
                        uv_index = (j / 2) * width + (i / 2) * 2;
                        nv12_data[y_index] = y_avg;
                        if (0 == (i % 2) && (0 == j % 2)) {
                            nv12_data[uv_size + uv_index] = u_avg;
                            nv12_data[uv_size + uv_index + 1] = v_avg;
                        }
                    }
                }
            }
        }
    }
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
    void nv12_mosaic(char *pic, 
                    int pic_w,
                    int pic_h, 
                    int rect_x, 
                    int rect_y, 
                    int rect_w,
                    int rect_h,
                    int block_size) {
        /* Locking the scope of rectangle border range */
        int size = pic_w * pic_h;
        int size1 = pic_w * 2;
        int y_index = 0;
        int u_index = 0;
        int v_index = 0;
        int x = 0, y = 0, i = 0, j = 0;
        int y_sum = 0, u_sum = 0, v_sum = 0;
        int count = 0;
        int y_avg = 0, u_avg = 0, v_avg = 0;
        int w = rect_x + rect_w, h = rect_y + rect_h;
        for (y = rect_y;y < h;y += block_size) {
            for (x = rect_x;x < w;x += block_size) {
                y_sum = 0, u_sum = 0, v_sum = 0;
                count = 0;
                for (j = y;j < y + block_size && j < h;j++) {
                    for (i = x;i < x + block_size && i < w;i++) {
                        y_index = j * pic_w + i;
                        u_index = (y_index / size1) * pic_w + (((y_index % pic_w) / 2) << 1) + size;
                        v_index = u_index + 1;
                        y_sum += pic[y_index];
                        u_sum += pic[u_index];
                        v_sum += pic[v_index];
                        count++;
                    }
                }
                y_avg = y_sum / count;
                u_avg = u_sum / (count / 4);        // 4Y -- 1UV
                v_avg = v_sum / (count / 4);
                for (j = y;j < y + block_size && j < rect_y + rect_h;j++) {
                    for (i = x;i < x + block_size && i < rect_x + rect_w;i++) {
                        y_index = j * pic_w + i;
                        u_index = (y_index / size1) * pic_w + (((y_index % pic_w) / 2) << 1) + size;
                        v_index = u_index + 1;
                        pic[y_index] = y_avg;
                        pic[u_index] = u_avg;
                        pic[v_index] = v_avg;
                    }
                }    
            }
        }
    }
    void uyvy_mosaic(unsigned char* data,
                     int width,
                     int height,
                     int x,
                     int y,
                     int w,
                     int h,
                     int block_size) {
        int i = 0, j = 0, ii = 0, jj = 0;
        int u = 0, y1 = 0, v = 0, y2 = 0;
        int u_sum = 0, y1_sum = 0, v_sum = 0, y2_sum = 0;
        int count = block_size * block_size / 2;
        unsigned char *p = nullptr;
        if (block_size <= 0 || block_size >= w || block_size >= h) {
            return;
        }
        for (j = y;j < y + h;j += block_size) {
            for (i = x;i < x + w;i += block_size) {
                u_sum = y1_sum = v_sum = y2_sum = 0;
                for (jj = j;jj < j + block_size;jj++) {
                    if (jj >= j + h) continue;
                    for (ii = i;ii < i + block_size;ii += 2) {
                        if (ii >= i + w) continue;
                        p = data + jj * width * 2 + ii * 2;
                        u = *p;
                        y1 = *(p + 1);
                        v = *(p + 2);
                        y2 = *(p + 3);
                        u_sum += u;
                        y1_sum += y1;
                        v_sum += v;
                        y2_sum += y2;
                    }
                }
                u = u_sum / count;
                y1 = y1_sum / count;
                v = v_sum / count;
                y2 = y2_sum / count;
                for (jj = j;jj < j + block_size;jj++) {
                    if (jj >= j + h) continue;
                    for (ii = i;ii < i + block_size;ii += 2) {
                        if (ii >= i + w) continue;
                        p = data + jj * width * 2 + ii * 2;
                        *p = u;
                        *(p + 1) = y1;
                        *(p + 2) = v;
                        *(p + 3) = y2;
                    }
                }
            }
        }
    }
    bool convert_yuyv_nv12(const char *yuyv_file_path, size_t width, size_t height, const char *nv12_file_path) {
        if (!yuyv_file_path || !nv12_file_path) {
            return false;
        }
        FILE *fp = fopen(yuyv_file_path, "rb");
        if (!fp) {
            return false;
        }
        fseek(fp, 0, SEEK_END);
        size_t file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        size_t frame_size = width * height * 2;
        if (file_size != frame_size) {
            fclose(fp);
            return false;
        }
        char *yuyv_content = (char *)malloc(sizeof(char) * file_size);
        if (!yuyv_content) {
            fclose(fp);
            return false;
        }
        if (file_size != fread(yuyv_content, 1, file_size, fp)) {
            free(yuyv_content);
            fclose(fp);
            return false;
        }
        fclose(fp);
        // convert uyvy to nv12
        frame_size = width * height * 3 / 2;
        char *nv12_content = (char *)malloc(sizeof(char) * frame_size);
        if (!nv12_content) {
            free(yuyv_content);
            return false;
        }
        size_t y_size = width * height;
        size_t pixels_in_a_row = width * 2;
        char *nv12_y_ptr = nv12_content;
        char *nv12_uv_ptr = nv12_content + y_size;
        int lines = 0;
        for (int i = 0;i < file_size;i += 4) {
            // copy y channel
            *nv12_y_ptr++ = yuyv_content[i];
            *nv12_y_ptr++ = yuyv_content[i + 2];
            if (0 == i % pixels_in_a_row) {
                ++lines;
            }
            if (lines % 2) {       // extract the UV value of odd rows
                // copy uv channel
                *nv12_uv_ptr++ = yuyv_content[i + 1];
                *nv12_uv_ptr++ = yuyv_content[i + 3];
            }
        }
        free(yuyv_content);
        fp = fopen(nv12_file_path, "wb");
        if (!fp) {
            free(nv12_content);
            return false;
        }
        if (frame_size != fwrite(nv12_content, 1, frame_size, fp)) {
            free(nv12_content);
            fclose(fp);
            return false;
        }
        free(nv12_content);
        fclose(fp);
        return true;
    }
    double resize_uyvy_to_nv12_not_good(unsigned char* puyvy, 
                             int w, 
                             int h, 
                             unsigned char* pOutnv12y/*must malloc outside*/,
                             unsigned char* pOutnv12uv/*must malloc outside*/) {
        //uyuv->nv12
        //w/2
        //h/2
        struct timeval start_time, end_time;
        gettimeofday(&start_time, NULL);
        unsigned char* py = pOutnv12y;
        unsigned char* puv = pOutnv12uv;
        for(int i=0; i<h; i+=2)
        {
            for(int j=0; j<w; j+=2)
            {
                *py = *(puyvy + i*w*2 + j*2 + 1);	//y
                py++;
                if(i%4==0&&j%4==0)
                {
                    *puv = *(puyvy + i*w*2 + j*2);	//u
                    puv++;
                    *puv = *(puyvy + i*w*2 + j*2 + 2);	//v
                    puv++;
                }
            }
        }
        gettimeofday(&end_time, NULL);
        double time_used = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
        return time_used;
    }
    double resize_uyvy_to_nv12_not_good1(unsigned char* puyvy, 
                             int w, 
                             int h, 
                             unsigned char* nv12_y,
                             unsigned char* nv12_uv) {
        // uyuv->nv12
        // w / 2
        // h / 2
        struct timeval start_time, end_time;
        gettimeofday(&start_time, NULL);
        int pixels_in_a_row = w << 1;
        int y_index = 0, u_index = 0, v_index = 0;
        const auto divisible_by_four = [] (int i) { return 0 == (i & 0x03); };
        for (int i = 0;i < h;i += 2) {
            for (int j = 0;j < w;j += 2) {
                y_index = i * pixels_in_a_row + (j << 1) + 1;
                *nv12_y++ = puyvy[y_index];
                if (divisible_by_four(i) && divisible_by_four(j)) {
                    u_index = y_index - 1;
                    v_index = y_index + 1;
                    *nv12_uv++ = puyvy[u_index];
                    *nv12_uv++ = puyvy[v_index];
                }
            }
        }
        gettimeofday(&end_time, NULL);
        double time_used = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
        return time_used;
    }
    double resize_uyvy_to_nv12_not_good2(unsigned char* puyvy, 
                             int w, 
                             int h, 
                             unsigned char* nv12_y,
                             unsigned char* nv12_uv) {
        // uyuv->nv12
        // w / 2
        // h / 2
        struct timeval start_time, end_time;
        gettimeofday(&start_time, NULL);
        int pixels_in_a_row = w << 1;
        int y_index = 0, u_index = 0, v_index = 0;
        for (int i = 0;i < h;i += 2) {
            for (int j = 0;j < w;j += 2) {
                y_index = i * pixels_in_a_row + (j << 1) + 1;
                *nv12_y++ = puyvy[y_index];
                if ((0 == (i & 0x03)) && (0 == (j & 0x03))) {
                    u_index = y_index - 1;
                    v_index = y_index + 1;
                    *nv12_uv++ = puyvy[u_index];
                    *nv12_uv++ = puyvy[v_index];
                }
            }
        }
        gettimeofday(&end_time, NULL);
        double time_used = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
        return time_used;
    }
    double resize_uyvy_to_nv12(unsigned char* puyvy, 
                             int w, 
                             int h, 
                             unsigned char* nv12_y,
                             unsigned char* nv12_uv) {
        // uyuv->nv12
        // w / 2
        // h / 2
        struct timeval start_time, end_time;
        gettimeofday(&start_time, NULL);
        int pixels_in_a_row = w << 1;
        int u_index = 0;
        for (int i = 0;i < h;i += 2) {
            for (int j = 0;j < w;j += 2) {
                u_index = i * pixels_in_a_row + (j << 1);
                *nv12_y++ = puyvy[u_index + 1];
                if ((0 == (i & 0x03)) && (0 == (j & 0x03))) {
                    *nv12_uv++ = puyvy[u_index];
                    *nv12_uv++ = puyvy[u_index + 2];
                }
            }
        }
        gettimeofday(&end_time, NULL);
        double time_used = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
        return time_used;
    }
    void resize_uyvy_to_nv12_(unsigned char* puyvy, 
                             int w, 
                             int h, 
                             unsigned char* nv12_y) {
        // uyuv->nv12
        // w / 2
        // h / 2
        struct timeval start_time, end_time;
        gettimeofday(&start_time, NULL);
        int i = 0;
        int pixels_in_a_row = w << 1;
        int size = (w * h) << 1;
        int lines = 0;
        bool flag = true;
        unsigned char* nv2_uv = nv12_y + w * h / 4;
        while (i < size) {
            if (0 == i % pixels_in_a_row) {
                ++lines;
            }
            if (lines & 0x01) {       // odd rows
                *nv12_y++ = puyvy[i + 1];
                if (flag && (i == ((i >> 3) << 3))) {       // divisible by 8
                    *nv2_uv++ = puyvy[i];
                    *nv2_uv++ = puyvy[i + 2];
                }
                i += 4;
            }
            else {
                i += pixels_in_a_row;       // skip even lines
                flag = !flag;               // not get uv value
            }
        }
        gettimeofday(&end_time, NULL);
        double time_used = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
        printf("time used:%lf seconds\n", time_used);
    }
    static void get_channel_value_from_nv12(const unsigned char* img,
                                    size_t width,
                                    size_t height,
                                    size_t y_size,
                                    int x,
                                    int y,
                                    unsigned char& y_val,
                                    unsigned char& u_val,
                                    unsigned char& v_val ) {
        int y_index = y * width + x;
        int uv_index = y_size + (y / 2) * width + (x / 2) * 2;
        y_val = img[y_index];
        u_val = img[uv_index];
        v_val = img[uv_index + 1];
    }
    static void get_channel_value_from_uyvy(const unsigned char* img,
                                    size_t width,
                                    size_t height,
                                    int x,
                                    int y,
                                    unsigned char& u_val,
                                    unsigned char& y1_val,
                                    unsigned char& v_val,
                                    unsigned char& y2_val ) {
        int u_index = y * (width << 1) + (x << 1);
        u_val = img[u_index];
        y1_val = img[u_index + 1];
        v_val = img[u_index + 2];
        y2_val = img[u_index + 3];
    }
    static void set_channel_value_to_nv12(unsigned char* img,
                                    size_t width,
                                    size_t height,
                                    size_t y_size,
                                    int x,
                                    int y,
                                    unsigned char y_val,
                                    unsigned char u_val,
                                    unsigned char v_val ) {
        int y_index = y * width + x;
        int uv_index = y_size + (y / 2) * width + (x / 2) * 2;
        img[y_index] = y_val;
        img[uv_index] = u_val;
        img[uv_index + 1] = v_val;
    }
    static void set_channel_value_to_uyvy(unsigned char* img,
                                   size_t width,
                                   size_t height,
                                   int x,
                                   int y,
                                   unsigned char u_val,
                                   unsigned char y1_val,
                                   unsigned char v_val,
                                   unsigned char y2_val) {
        int u_index = y * (width << 1) + (x << 1);
        img[u_index] = u_val;
        img[u_index + 1] = y1_val;
        img[u_index + 2] = v_val;
        img[u_index + 3] = y2_val;
    }
    static inline double get_bilinear_interpolation(double x1, double x2,
                                             double y1, double y2, 
                                             double x, double y,
                                             double f11, double f21,
                                             double f12, double f22)  {
        double tmp = (x2 - x1) * (y2 - y1);
        return f11 * (x2 - x) * (y2 - y) / tmp
            +  f21 * (x - x1) * (y2 - y) / tmp
            +  f12 * (x2 - x) * (y - y1) / tmp
            +  f22 * (x - x1) * (y - y1) / tmp;
    }
    void nv12_resize(const unsigned char* src_img, 
                unsigned char* dest_img,
                size_t src_width,
                size_t src_height,
                size_t dest_width,
                size_t dest_height) {
        size_t dest_x = 0, dest_y = 0;
        double src_x = 0, src_y = 0;
        // find 4 points
        int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
        // y channel val for 4 points
        unsigned char y11 = 0, y12 = 0, y21 = 0, y22 = 0;
        // u channel val for 4 points
        unsigned char u11 = 0, u12 = 0, u21 = 0, u22 = 0;
        // v channel val for 4 points
        unsigned char v11 = 0, v12 = 0, v21 = 0, v22 = 0;
        // the target interpolation point yuv value
        double dest_y_val = 0, dest_u_val = 0, dest_v_val = 0;
        size_t src_y_size = src_width * src_height;
        size_t dest_y_size = dest_width * dest_height;
        for (;dest_y < dest_height;++dest_y) {
            for (dest_x = 0;dest_x < dest_width;++dest_x) {
                src_x = (dest_x + 0.5) * src_width / dest_width - 0.5;
                src_y = (dest_y + 0.5) * src_height / dest_height - 0.5;
                x1 = floor(src_x);
                x2 = ceil(src_x);
                y1 = floor(src_y);
                y2 = ceil(src_y);

                get_channel_value_from_nv12(src_img, src_width, src_height, src_y_size, x1, y1, y11, u11, v11);
                get_channel_value_from_nv12(src_img, src_width, src_height, src_y_size, x1, y2, y12, u12, v12);
                get_channel_value_from_nv12(src_img, src_width, src_height, src_y_size, x2, y1, y21, u21, v21);
                get_channel_value_from_nv12(src_img, src_width, src_height, src_y_size, x2, y2, y22, u22, v22);

                dest_y_val = get_bilinear_interpolation(x1, x2, y1, y2, src_x, src_y, y11, y21, y12, y22);
                dest_u_val = get_bilinear_interpolation(x1, x2, y1, y2, src_x, src_y, u11, u21, u12, u22);
                dest_v_val = get_bilinear_interpolation(x1, x2, y1, y2, src_x, src_y, v11, v21, v12, v22);

                set_channel_value_to_nv12(dest_img, dest_width, dest_height, dest_y_size, dest_x, dest_y, dest_y_val, dest_u_val, dest_v_val);
            }
        }
    }
    void uyvy_resize(const unsigned char* src_img, 
                unsigned char* dest_img,
                size_t src_width,
                size_t src_height,
                size_t dest_width,
                size_t dest_height) {
        size_t dest_x = 0, dest_y = 0;
        double src_x = 0, src_y = 0;
        // find 4 points
        int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
        // y channel val for 4 points
        unsigned char y11_1 = 0, y12_1 = 0, y21_1 = 0, y22_1 = 0;
        unsigned char y11_2 = 0, y12_2 = 0, y21_2 = 0, y22_2 = 0;
        // u channel val for 4 points
        unsigned char u11 = 0, u12 = 0, u21 = 0, u22 = 0;
        // v channel val for 4 points
        unsigned char v11 = 0, v12 = 0, v21 = 0, v22 = 0;
        // the target interpolation point yuv value
        double dest_y1_val = 0, dest_y2_val = 0, dest_u_val = 0, dest_v_val = 0;
        for (;dest_y < dest_height;++dest_y) {
            for (dest_x = 0;dest_x < dest_width;++dest_x) {
                src_x = (dest_x + 0.5) * src_width / dest_width - 0.5;
                src_y = (dest_y + 0.5) * src_height / dest_height - 0.5;
                x1 = floor(src_x);
                x2 = ceil(src_x);
                y1 = floor(src_y);
                y2 = ceil(src_y);

                get_channel_value_from_uyvy(src_img, src_width, src_height, x1, y1, u11, y11_1, v11, y11_2);
                get_channel_value_from_uyvy(src_img, src_width, src_height, x1, y2, u12, y12_1, v12, y12_2);
                get_channel_value_from_uyvy(src_img, src_width, src_height, x2, y1, u21, y21_1, v21, y21_2);
                get_channel_value_from_uyvy(src_img, src_width, src_height, x2, y2, u22, y22_1, v22, y22_2);

                dest_y1_val = get_bilinear_interpolation(x1, x2, y1, y2, src_x, src_y, y11_1, y21_1, y12_1, y22_1);
                dest_y2_val = get_bilinear_interpolation(x1, x2, y1, y2, src_x, src_y, y11_2, y21_2, y12_2, y22_2);
                dest_u_val = get_bilinear_interpolation(x1, x2, y1, y2, src_x, src_y, u11, u21, u12, u22);
                dest_v_val = get_bilinear_interpolation(x1, x2, y1, y2, src_x, src_y, v11, v21, v12, v22);

                set_channel_value_to_uyvy(dest_img, dest_width, dest_height, dest_x, dest_y, dest_u_val, dest_y1_val, dest_v_val, dest_y2_val);
            }
        }
    }
    static double bicubicpoly(double x) {
        double abs_x = abs(x);
        const float a = -0.5;
        if (abs_x <= 1.0) {
            return (a + 2) * pow(abs_x, 3) - (a + 3) * pow(abs_x, 2) + 1;
        }
        else if(abs_x < 2.0) {
            return a * pow(abs_x, 3) - 5 * a * pow(abs_x, 2) + 8 * a * abs_x - 4 * a;
        }
        return 0.0;
    }
    void uyvy_resize_with_bicubicpoly(const unsigned char* src_img, 
                unsigned char* dest_img,
                size_t src_width,
                size_t src_height,
                size_t dest_width,
                size_t dest_height) {
        size_t dest_x = 0, dest_y = 0;
        double src_x = 0, src_y = 0;
        // the target interpolation point yuv value
        double dest_y1_val = 0, dest_y2_val = 0, dest_u_val = 0, dest_v_val = 0;
        int x0 = 0, x1 = 0, x2 = 0, x3 = 0;
        int y0 = 0, y1 = 0, y2 = 0, y3 = 0;
        double dist_x0 = 0, dist_x1 = 0, dist_x2 = 0, dist_x3 = 0;
        double dist_y0 = 0, dist_y1 = 0, dist_y2 = 0, dist_y3 = 0;
     	double dist_x0y0 = 0, dist_x0y1 = 0, dist_x0y2 = 0, dist_x0y3 = 0, 
               dist_x1y0 = 0, dist_x1y1 = 0, dist_x1y2 = 0, dist_x1y3 = 0,
               dist_x2y0 = 0, dist_x2y1 = 0, dist_x2y2 = 0, dist_x2y3 = 0,
               dist_x3y0 = 0, dist_x3y1 = 0, dist_x3y2 = 0, dist_x3y3 = 0;
        // the target points y u v
        unsigned char u_x0y0 = 0, u_x0y1 = 0, u_x0y2 = 0, u_x0y3 = 0, 
               u_x1y0 = 0, u_x1y1 = 0, u_x1y2 = 0, u_x1y3 = 0,
               u_x2y0 = 0, u_x2y1 = 0, u_x2y2 = 0, u_x2y3 = 0,
               u_x3y0 = 0, u_x3y1 = 0, u_x3y2 = 0, u_x3y3 = 0;

        unsigned char y1_x0y0 = 0, y1_x0y1 = 0, y1_x0y2 = 0, y1_x0y3 = 0, 
               y1_x1y0 = 0, y1_x1y1 = 0, y1_x1y2 = 0, y1_x1y3 = 0,
               y1_x2y0 = 0, y1_x2y1 = 0, y1_x2y2 = 0, y1_x2y3 = 0,
               y1_x3y0 = 0, y1_x3y1 = 0, y1_x3y2 = 0, y1_x3y3 = 0;

        unsigned char v_x0y0 = 0, v_x0y1 = 0, v_x0y2 = 0, v_x0y3 = 0, 
               v_x1y0 = 0, v_x1y1 = 0, v_x1y2 = 0, v_x1y3 = 0,
               v_x2y0 = 0, v_x2y1 = 0, v_x2y2 = 0, v_x2y3 = 0,
               v_x3y0 = 0, v_x3y1 = 0, v_x3y2 = 0, v_x3y3 = 0;

        unsigned char y2_x0y0 = 0, y2_x0y1 = 0, y2_x0y2 = 0, y2_x0y3 = 0, 
               y2_x1y0 = 0, y2_x1y1 = 0, y2_x1y2 = 0, y2_x1y3 = 0,
               y2_x2y0 = 0, y2_x2y1 = 0, y2_x2y2 = 0, y2_x2y3 = 0,
               y2_x3y0 = 0, y2_x3y1 = 0, y2_x3y2 = 0, y2_x3y3 = 0;   
        for (;dest_y < dest_height;++dest_y) {
            for (dest_x = 0;dest_x < dest_width;++dest_x) {
                // get the src coordinate
                src_x = (dest_x + 0.5) * src_width / dest_width - 0.5;
                src_y = (dest_y + 0.5) * src_height / dest_height - 0.5;
                // get the target 116 points
                x0 = floor(src_x);
                x1 = ceil(src_x);
                y0 = floor(src_y);
                y1 = ceil(src_y);

                x2 = x0 - 1;
                if (x2 < 0) {
                    x2 = 0;
                }
                x3 = x1 + 1;
                if (x3 >= src_width) {
                    x3 = src_width - 1;
                }

                y2 = y0 - 1;
                if (y2 < 0) {
                    y2 = 0;
                }
                y3 = y1 + 1;
                if (y3 >= src_height) {
                    y3 = src_height - 1;
                }
                // compute bicubicpoly
                dist_x0 = bicubicpoly(src_x - x0);
                dist_x1 = bicubicpoly(src_x - x1);
                dist_x2 = bicubicpoly(src_x - x2);
                dist_x3 = bicubicpoly(src_x - x3);

                dist_y0 = bicubicpoly(src_y - y0);
                dist_y1 = bicubicpoly(src_y - y1);
                dist_y2 = bicubicpoly(src_y - y2);
                dist_y3 = bicubicpoly(src_y - y3);
                // make 16 point weight
                dist_x0y0 = dist_x0 * dist_y0;
                dist_x0y1 = dist_x0 * dist_y1;
				dist_x0y2 = dist_x0 * dist_y2;
				dist_x0y3 = dist_x0 * dist_y3;

				dist_x1y0 = dist_x1 * dist_y0;
				dist_x1y1 = dist_x1 * dist_y1;
				dist_x1y2 = dist_x1 * dist_y2;
				dist_x1y3 = dist_x1 * dist_y3;

				dist_x2y0 = dist_x2 * dist_y0;
				dist_x2y1 = dist_x2 * dist_y1;
				dist_x2y2 = dist_x2 * dist_y2;
				dist_x2y3 = dist_x2 * dist_y3;
                
				dist_x3y0 = dist_x3 * dist_y0;
				dist_x3y1 = dist_x3 * dist_y1;
				dist_x3y2 = dist_x3 * dist_y2;
                dist_x3y3 = dist_x3 * dist_y3;

                get_channel_value_from_uyvy(src_img, src_width, src_height, x0, y0, 
                                 u_x0y0, y1_x0y0, v_x0y0, y2_x0y0);
                get_channel_value_from_uyvy(src_img, src_width, src_height, x0, y1, 
                                 u_x0y1, y1_x0y1, v_x0y1, y2_x0y1);
                get_channel_value_from_uyvy(src_img, src_width, src_height, x0, y2, 
                                 u_x0y2, y1_x0y2, v_x0y2, y2_x0y2);
                get_channel_value_from_uyvy(src_img, src_width, src_height, x0, y3, 
                                 u_x0y3, y1_x0y3, v_x0y3, y2_x0y3);
                
                get_channel_value_from_uyvy(src_img, src_width, src_height, x1, y0, 
                                 u_x1y0, y1_x1y0, v_x1y0, y2_x1y0);
                get_channel_value_from_uyvy(src_img, src_width, src_height, x1, y1, 
                                 u_x1y1, y1_x1y1, v_x1y1, y2_x1y1);
                get_channel_value_from_uyvy(src_img, src_width, src_height, x1, y2, 
                                 u_x1y2, y1_x1y2, v_x1y2, y2_x1y2);
                get_channel_value_from_uyvy(src_img, src_width, src_height, x1, y3, 
                                 u_x1y3, y1_x1y3, v_x1y3, y2_x1y3);

                get_channel_value_from_uyvy(src_img, src_width, src_height, x2, y0, 
                                 u_x2y0, y1_x2y0, v_x2y0, y2_x2y0);
                get_channel_value_from_uyvy(src_img, src_width, src_height, x2, y1, 
                                 u_x2y1, y1_x2y1, v_x2y1, y2_x2y1);
                get_channel_value_from_uyvy(src_img, src_width, src_height, x2, y2, 
                                 u_x2y2, y1_x2y2, v_x2y2, y2_x2y2);
                get_channel_value_from_uyvy(src_img, src_width, src_height, x2, y3, 
                                 u_x2y3, y1_x2y3, v_x2y3, y2_x2y3);

                get_channel_value_from_uyvy(src_img, src_width, src_height, x3, y0, 
                                 u_x3y0, y1_x3y0, v_x3y0, y2_x3y0);
                get_channel_value_from_uyvy(src_img, src_width, src_height, x3, y1, 
                                 u_x3y1, y1_x3y1, v_x3y1, y2_x3y1);
                get_channel_value_from_uyvy(src_img, src_width, src_height, x3, y2, 
                                 u_x3y2, y1_x3y2, v_x3y2, y2_x3y2);
                get_channel_value_from_uyvy(src_img, src_width, src_height, x3, y3, 
                                 u_x3y3, y1_x3y3, v_x3y3, y2_x3y3);

                dest_u_val = u_x0y0 * dist_x0y0 +
                             u_x0y1 * dist_x0y1 +
                             u_x0y2 * dist_x0y2 +
                             u_x0y3 * dist_x0y3 +

                             u_x1y0 * dist_x1y0 +
                             u_x1y1 * dist_x1y1 +
                             u_x1y2 * dist_x1y2 +
                             u_x1y3 * dist_x1y3 +

                             u_x2y0 * dist_x2y0 +
                             u_x2y1 * dist_x2y1 +
                             u_x2y2 * dist_x2y2 +
                             u_x2y3 * dist_x2y3 +

                             u_x3y0 * dist_x3y0 +
                             u_x3y1 * dist_x3y1 +
                             u_x3y2 * dist_x3y2 +
                             u_x3y3 * dist_x3y3;

                dest_y1_val = y1_x0y0 * dist_x0y0 +
                             y1_x0y1 * dist_x0y1 +
                             y1_x0y2 * dist_x0y2 +
                             y1_x0y3 * dist_x0y3 +

                             y1_x1y0 * dist_x1y0 +
                             y1_x1y1 * dist_x1y1 +
                             y1_x1y2 * dist_x1y2 +
                             y1_x1y3 * dist_x1y3 +

                             y1_x2y0 * dist_x2y0 +
                             y1_x2y1 * dist_x2y1 +
                             y1_x2y2 * dist_x2y2 +
                             y1_x2y3 * dist_x2y3 +

                             y1_x3y0 * dist_x3y0 +
                             y1_x3y1 * dist_x3y1 +
                             y1_x3y2 * dist_x3y2 +
                             y1_x3y3 * dist_x3y3;

                dest_v_val = v_x0y0 * dist_x0y0 +
                             v_x0y1 * dist_x0y1 +
                             v_x0y2 * dist_x0y2 +
                             v_x0y3 * dist_x0y3 +

                             v_x1y0 * dist_x1y0 +
                             v_x1y1 * dist_x1y1 +
                             v_x1y2 * dist_x1y2 +
                             v_x1y3 * dist_x1y3 +

                             v_x2y0 * dist_x2y0 +
                             v_x2y1 * dist_x2y1 +
                             v_x2y2 * dist_x2y2 +
                             v_x2y3 * dist_x2y3 +

                             v_x3y0 * dist_x3y0 +
                             v_x3y1 * dist_x3y1 +
                             v_x3y2 * dist_x3y2 +
                             v_x3y3 * dist_x3y3;

                dest_y2_val = y2_x0y0 * dist_x0y0 +
                             y2_x0y1 * dist_x0y1 +
                             y2_x0y2 * dist_x0y2 +
                             y2_x0y3 * dist_x0y3 +

                             y2_x1y0 * dist_x1y0 +
                             y2_x1y1 * dist_x1y1 +
                             y2_x1y2 * dist_x1y2 +
                             y2_x1y3 * dist_x1y3 +

                             y2_x2y0 * dist_x2y0 +
                             y2_x2y1 * dist_x2y1 +
                             y2_x2y2 * dist_x2y2 +
                             y2_x2y3 * dist_x2y3 +

                             y2_x3y0 * dist_x3y0 +
                             y2_x3y1 * dist_x3y1 +
                             y2_x3y2 * dist_x3y2 +
                             y2_x3y3 * dist_x3y3;
                set_channel_value_to_uyvy(dest_img, dest_width, dest_height, dest_x, dest_y, dest_u_val, dest_y1_val, dest_v_val, dest_y2_val);
            }
        }
    }
    void reduce_uyvy_1_4(const unsigned char* src_img,
                unsigned char* dest_img,
                size_t src_size,
                size_t src_width) {
        size_t start = 0;
        size_t i = 0;
        int count = 0;
        size_t span = 4 * src_width;
        while (start < src_size) {
            memcpy(dest_img, src_img + i, 4);
            dest_img += 4;
            count += 2;
            if ((count << 1) >= src_width) {
                    start += span;
                    i = start;
                    count = 0;
                    continue;
            }
            i += 8;
        }
    }
    void uyvy_set_color(unsigned char* pic, 
                int pic_w,
                int i, 
                int j, 
                int Y,
                int U,
                int V) {
        int pixels_in_a_row = pic_w << 1;
        int u_index = j * pixels_in_a_row + (i << 1);
        int y_index = u_index + 1;
        int v_index = 0;
        if (pic[y_index] != Y) {
            pic[y_index] = Y;
        }
        if (1 == y_index % 4) {
            if (pic[u_index] != U) {
                pic[u_index] = U;
            }
            v_index = y_index + 1; 
            if (pic[v_index] != V) {
                pic[v_index] = V;
            }
        }
    }
    void uyvy_fill_zone(unsigned char* pic, 
                int pic_w,
                int A_i, 
                int A_j,
                int B_i,
                int B_j, 
                int Y,
                int U,
                int V) {
        int start_i = A_i, end_i = B_i, start_j = A_j, end_j = B_j;
        if (A_i > B_i) {
            start_i = B_i;
            end_i = A_i; 
        }
        if (A_j > B_j) {
            start_j = B_j;
            end_j = A_j;
        }
        for (int j = start_j;j <= end_j;j++) {
            for (int i = start_i;i <= end_i;i++) {
                uyvy_set_color(pic, pic_w, i, j, Y, U, V);
            }
        }
    }
    double uyvy_draw_circle(unsigned char* pic, 
                    int pic_w,
                    int pic_h, 
                    int x, 
                    int y, 
                    int r,
                    int R,
                    int G,
                    int B) {
        if (x <= 0 || x >= pic_w) {
            return 0;
        }
        if (y <= 0 || y >= pic_h) {
            return 0;
        }
        if ((x + r > pic_w) || (x - r) < 0) {
            return 0;
        }
        if ((y + r > pic_h) || (y - r) < 0) {
            return 0;
        }
        struct timeval start_time, end_time;
        gettimeofday(&start_time, NULL);
        /* RGB convert YUV */
        int Y =  0.299  * R + 0.587  * G + 0.114  * B;
        int U = -0.1687 * R + 0.3313 * G + 0.5    * B + 128;
        int V =  0.5    * R - 0.4187 * G - 0.0813 * B + 128;
        int i = 0, j = 0;
        // find the target point
        int tx = 0, ty = r, d = 3 - 2 * r;
        while (tx <= ty) {
            // 1 point
            i = x + tx;
            j = y + ty;
            uyvy_set_color(pic, pic_w, i, j, Y, U, V);
            uyvy_fill_zone(pic, pic_w, x, y, i, j, Y, U, V);
            // 2 point
            i = x + tx;
            j = y - ty;
            uyvy_set_color(pic, pic_w, i, j, Y, U, V);
            uyvy_fill_zone(pic, pic_w, x, y, i, j, Y, U, V);
            // 3 point
            i = x - tx;
            j = y + ty;
            uyvy_set_color(pic, pic_w, i, j, Y, U, V);
            uyvy_fill_zone(pic, pic_w, x, y, i, j, Y, U, V);
            // 4 point
            i = x - tx;
            j = y - ty;
            uyvy_set_color(pic, pic_w, i, j, Y, U, V);
            uyvy_fill_zone(pic, pic_w, x, y, i, j, Y, U, V);
            // 5 point
            i = x + ty;
            j = y + tx;
            uyvy_set_color(pic, pic_w, i, j, Y, U, V);
            uyvy_fill_zone(pic, pic_w, x, y, i, j, Y, U, V);
            // 6 point
            i = x + ty;
            j = y - tx;
            uyvy_set_color(pic, pic_w, i, j, Y, U, V);
            uyvy_fill_zone(pic, pic_w, x, y, i, j, Y, U, V);
            // 7 point
            i = x - ty;
            j = y + tx;
            uyvy_set_color(pic, pic_w, i, j, Y, U, V);
            uyvy_fill_zone(pic, pic_w, x, y, i, j, Y, U, V);
            // 8 point
            i = x - ty;
            j = y - tx;
            uyvy_set_color(pic, pic_w, i, j, Y, U, V);
            uyvy_fill_zone(pic, pic_w, x, y, i, j, Y, U, V);
            if (d <= 0) {
                d += (tx << 2) + 6;
            }
            else {
                d += ((tx - ty) << 2) + 10;
                --ty;
            }
            ++tx;
        }
        gettimeofday(&end_time, NULL);
        double time_used = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
        return time_used;
    }
    double convert_uyvy_i420(unsigned const char *uyvy,
                           int width,
                           int height,
                           unsigned char *i420) {
        struct timeval start_time, end_time;
        gettimeofday(&start_time, NULL);
        int pixels_in_a_row = width << 1;
        int u_index = 0;
        int y_size = width * height;
        int uv_size = y_size / 4;
        unsigned char* i420_y = i420;
        unsigned char* i420_u = i420_y + y_size;
        unsigned char* i420_v = i420_u + uv_size;

        for (int i = 0;i < height;++i) {
            for (int j = 0;j < width;j += 2) {
                u_index = i * pixels_in_a_row + (j << 1);
                *i420_y++ = uyvy[u_index + 1];
                *i420_y++ = uyvy[u_index + 3];
                if (0 == (i & 0x01)) {      // odd lines
                    *i420_u++ = uyvy[u_index];
                    *i420_v++ = uyvy[u_index + 2];
                }
            }
        }
        gettimeofday(&end_time, NULL);
        double time_used = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
        return time_used;        
    }
};

#define  G_IMAGE_UTILITY single_instance<image_utility>::instance()