/**
 * @file ygamma.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-27
 * 
 * Copyright (c) of ADAS_EYES 2023
 * 
 */

#include "modules/modules.h"

#define MOD_NAME "ygamma"

static int YGamma(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int pixel_idx = 0;


    uint8_t* y_i = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_i.y);
    uint8_t* y_o = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_o.y);

    const auto& gamma_prm = isp_prm->y_gamma;

    float step_coff = (float)(gamma_prm.nums - 1) / (1 << gamma_prm.in_bits);
    float out_max = (1 << gamma_prm.out_bits) - 1;

    FOR_ITER(h, frame->info.height)
    {
        FOR_ITER(w, frame->info.width)
        {
            pixel_idx = h * frame->info.width + w;
            auto y = y_i[pixel_idx];

            float cuvre_id_f = y * step_coff;
            int   curve_id = static_cast<int>(cuvre_id_f);
            //scale to 0~1
            float scale = (cuvre_id_f - curve_id) * (gamma_prm.curve[curve_id + 1] - gamma_prm.curve[curve_id])
                         + gamma_prm.curve[curve_id];
                //get scale value
            y = out_max * scale;
            y_o[pixel_idx] = y;               
        }
    }

    SwapMem<void>(frame->data.yuv_u8_i.y, frame->data.yuv_u8_o.y);
    
    return 0;
}

void RegisterYGammaMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::YUV;
    mod.out_domain = ColorDomains::YUV;

    mod.name = MOD_NAME;
    mod.run_function = YGamma;

    RegisterIspModule(mod);
}