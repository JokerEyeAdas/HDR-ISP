/**
 * @file yuv2rgb.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-27
 * 
 * Copyright (c) of ADAS_EYES 2023
 * 
 */

#include "modules/modules.h"

#define MOD_NAME "yuv2rgb"

static int Yuv2Bgr(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int pixel_idx = 0;
    int pwl_idx = 0;

    uint8_t* bgr_o = reinterpret_cast<uint8_t *>(frame->data.bgr_u8_o);
    uint8_t* y_i = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_i.y);
    uint8_t* u_i = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_i.u);
    uint8_t* v_i = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_i.v);

    FOR_ITER(ih, frame->info.height)
    {
        FOR_ITER(iw, frame->info.width)
        {
            int pixel_idx = GET_PIXEL_INDEX(iw, ih, frame->info.width);
            
            auto &b = bgr_o[3 * pixel_idx + 0];
            auto &g = bgr_o[3 * pixel_idx + 1];
            auto &r = bgr_o[3 * pixel_idx + 2];


            int r_tmp = y_i[pixel_idx] +                  0             + 1.114 * (v_i[pixel_idx] - 128);
            int g_tmp = y_i[pixel_idx] - 0.395 * (u_i[pixel_idx] - 128) - 0.581 * (v_i[pixel_idx] - 128);
            int b_tmp = y_i[pixel_idx] + 2.032 * (u_i[pixel_idx] - 128) +         0                     ;

            ClipMinMax(r_tmp, 255, 0);
            ClipMinMax(g_tmp, 255, 0);
            ClipMinMax(b_tmp, 255, 0);

            r = static_cast<uint8_t>(r_tmp);
            g = static_cast<uint8_t>(g_tmp);
            b = static_cast<uint8_t>(b_tmp);
        }
    }
    
    return 0;
}

void RegisterYuv2RgbMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::YUV;
    mod.out_domain = ColorDomains::BGR;

    mod.name = MOD_NAME;
    mod.run_function = Yuv2Bgr;

    RegisterIspModule(mod);
}