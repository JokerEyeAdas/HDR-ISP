#ifndef ADAS_ISP_FRAME_H
#define ADAS_ISP_FRAME_H

#include "common.h"

#define SAFE_FREE(arr)     \
    do                     \
    {                      \
        if (arr)           \
        {                  \
            delete[] arr;  \
            arr = nullptr; \
        }                  \
    } while (0)

class Frame
{
private:
    int FrameDateMalloc(int width, int height)
    {
        auto pixel_nums = (width + 8) * (height + 8);
        // raw domain
        data.raw_u8_i = new uint8_t[pixel_nums * 2]; // max raw16
        data.raw_u16_i = new uint16_t[pixel_nums];   // max raw16
        data.raw_u16_o = new uint16_t[pixel_nums];   // max raw16
        data.raw_s32_i = new int32_t[pixel_nums];
        data.raw_s32_o = new int32_t[pixel_nums];
        // bgr domain
        data.bgr_s32_i = new int32_t[pixel_nums * 3];
        data.bgr_s32_o = new int32_t[pixel_nums * 3];
        // yuv domain
        data.yuv_f32_i.y = new float[pixel_nums];
        data.yuv_f32_i.u = new float[pixel_nums];
        data.yuv_f32_i.v = new float[pixel_nums];
        data.yuv_f32_o.y = new float[pixel_nums];
        data.yuv_f32_o.u = new float[pixel_nums];
        data.yuv_f32_o.v = new float[pixel_nums];
        // yuv final out
        data.yuv_u8_i.y = new uint8_t[pixel_nums];
        data.yuv_u8_i.u = new uint8_t[pixel_nums];
        data.yuv_u8_i.v = new uint8_t[pixel_nums];
        data.yuv_u8_o.y = new uint8_t[pixel_nums];
        data.yuv_u8_o.u = new uint8_t[pixel_nums];
        data.yuv_u8_o.v = new uint8_t[pixel_nums];

        data.bgr_u8_o = new uint8_t[pixel_nums * 3];

        return 0;
    }

public:
    ImageInfo info;
    ImageMem data;

public:
    Frame(ImageInfo &img_info)
    {
        info = img_info;
        FrameDateMalloc(info.width, info.height);
    }

    ~Frame()
    {
        SAFE_FREE(data.raw_u8_i);
        SAFE_FREE(data.raw_u16_i);
        SAFE_FREE(data.raw_u16_o);
        SAFE_FREE(data.raw_s32_i);
        SAFE_FREE(data.raw_s32_o);
        SAFE_FREE(data.bgr_s32_i);
        SAFE_FREE(data.bgr_s32_o);
        SAFE_FREE(data.yuv_f32_i.y);
        SAFE_FREE(data.yuv_f32_i.u);
        SAFE_FREE(data.yuv_f32_i.v);
        SAFE_FREE(data.yuv_f32_o.y);
        SAFE_FREE(data.yuv_f32_o.u);
        SAFE_FREE(data.yuv_f32_o.v);
        SAFE_FREE(data.yuv_u8_i.y);
        SAFE_FREE(data.yuv_u8_i.u);
        SAFE_FREE(data.yuv_u8_i.v);
        SAFE_FREE(data.yuv_u8_o.y);
        SAFE_FREE(data.yuv_u8_o.u);
        SAFE_FREE(data.yuv_u8_o.v);
        SAFE_FREE(data.bgr_u8_o);
    }

    int ReadFileToFrame(std::string file, int size)
    {
        return (int)ReadFileToMem(file, data.raw_u8_i, size);
    }

    int RawMemToFrame(void *src, int len)
    {
        if (len > (info.width * info.height * 2))
        {
            LOG(ERROR) << "Frame Size err";
            return -1;
        }
        if (data.raw_u8_i)
        {
            memcpy(data.raw_u8_i, src, len);
            return 0;
        }
        return -2;
    }
};

#endif // !ADAS_ISP_FRAME_H