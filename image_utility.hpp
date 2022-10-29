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
};
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
        image->ppu8Plane[1] = image->ppu8Plane[0] + image->i32Height * pImage->pi32Pitch[0];
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
};

#define  G_IMAGE_UTILITY single_instance<image_utility>::instance()