/**
 * @file sharpen.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief
 * @version 0.1
 * @date 2023-07-27
 *
 * Copyright (c) of ADAS_EYES 2023
 *
 */

/*
USM： (src - w * gauss）/（1 - w）
w: 0.1～0.9, default: 0.6
*/
#include "modules/modules.h"

#define MOD_NAME "sharpen"

const int KernelSum = 273;
const int kGaussKernel[5][5] = {
    {1, 4, 7, 4, 1},
    {4, 16, 26, 16, 4},
    {7, 26, 41, 26, 7},
    {4, 16, 26, 16, 4},
    {1, 4, 7, 4, 1},
};

static int Sharpen(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int pixel_idx = 0;

    uint8_t *y_i = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_i.y);
    uint8_t *y_o = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_o.y);

    float ratio = isp_prm->sharpen_prms.ratio;

    FOR_ITER(h, frame->info.height)
    {
        FOR_ITER(w, frame->info.width)
        {
            pixel_idx = h * frame->info.width + w;
            if ((w < 2) || (h < 2) || (w > (frame->info.width - 3)) || (h > (frame->info.height - 3)))
            {
                y_o[pixel_idx] = y_i[pixel_idx];
                continue;
            }

            int y = 0;

            for (int kh = h - 2, gauss_idy = 0; kh <= h + 2; ++kh, ++gauss_idy)
            {
                for (int kw = w - 2, gauss_idx = 0; kw <= w + 2; ++kw, ++gauss_idx)
                {
                    y += (y_i[GET_PIXEL_INDEX(kw, kh, frame->info.width)] * kGaussKernel[gauss_idy][gauss_idx]);
                }
            }
            y = y / KernelSum;

            y = static_cast<int>((y_i[pixel_idx] - ratio * y) / (1 - ratio));

            ClipMinMax(y, 255, 0);
            y_o[pixel_idx] = y;
        }
    }

    SwapMem<void>(frame->data.yuv_u8_i.y, frame->data.yuv_u8_o.y);

    return 0;
}

void RegisterSharpenMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::YUV;
    mod.out_domain = ColorDomains::YUV;

    mod.name = MOD_NAME;
    mod.run_function = Sharpen;

    RegisterIspModule(mod);
}