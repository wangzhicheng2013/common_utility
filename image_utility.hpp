#pragma once
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
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
};

#define  G_IMAGE_UTILITY single_instance<image_utility>::instance()