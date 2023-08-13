/**
 * @file bgr2yuv.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief
 * @version 0.1
 * @date 2023-07-27
 *
 * Copyright (c) of ADAS_EYES 2023
 *
 */

#include "modules/modules.h"

#define MOD_NAME "rgb2yuv"

static int Rgb2Yuv(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int pixel_idx = 0;
    int pwl_idx = 0;

    int32_t *bgr_i = reinterpret_cast<int32_t *>(frame->data.bgr_s32_i);
    uint8_t *y_o = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_o.y);
    uint8_t *u_o = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_o.u);
    uint8_t *v_o = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_o.v);

    const DePwlPrms *pwl_prm = &(isp_prm->depwl_prm);

    FOR_ITER(ih, frame->info.height)
    {
        FOR_ITER(iw, frame->info.width)
        {
            int pixel_idx = GET_PIXEL_INDEX(iw, ih, frame->info.width);

            auto b = bgr_i[3 * pixel_idx + 0];
            auto g = bgr_i[3 * pixel_idx + 1];
            auto r = bgr_i[3 * pixel_idx + 2];

            auto y = static_cast<uint8_t>(0.299 * r + 0.587 * g + 0.114 * b);
            auto u = static_cast<uint8_t>(-0.147 * r - 0.289 * g + 0.436 * b + 128);
            auto v = static_cast<uint8_t>(0.615 * r - 0.515 * g - 0.100 * b + 128);

            y_o[pixel_idx] = y;
            u_o[pixel_idx] = u;
            v_o[pixel_idx] = v;
        }
    }

    SwapMem<void>(frame->data.yuv_u8_o.y, frame->data.yuv_u8_i.y);
    SwapMem<void>(frame->data.yuv_u8_o.u, frame->data.yuv_u8_i.u);
    SwapMem<void>(frame->data.yuv_u8_o.v, frame->data.yuv_u8_i.v);

    return 0;
}

void RegisterRgb2YuvMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::BGR;
    mod.out_domain = ColorDomains::YUV;

    mod.name = MOD_NAME;
    mod.run_function = Rgb2Yuv;

    RegisterIspModule(mod);
}