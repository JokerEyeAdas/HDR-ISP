/**
 * @file depwl.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief decode pwl curve
 * @version 0.1
 * @date 2023-07-27
 * Copyright (c) of ADAS_EYES 2023
 */
#include "modules/modules.h"

#define MOD_NAME "depwl"

static int Depwl(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int pixel_idx = 0;
    int pwl_idx = 0;

    uint16_t* raw16_in = reinterpret_cast<uint16_t *>(frame->data.raw_u16_i);
    int32_t*  raw32_out = reinterpret_cast<int32_t *>(frame->data.raw_s32_o);

    const DePwlPrms *pwl_prm = &(isp_prm->depwl_prm);

    FOR_ITER(h, frame->info.height)
    {
        FOR_ITER(w, frame->info.width)
        {
            pixel_idx = h * frame->info.width + w;
            for(int index = 1; index < pwl_prm->pwl_nums; ++index){
                if (raw16_in[pixel_idx] <= pwl_prm->x_cood[index]) {
                    pwl_idx = index;
                    break;
                }
                pwl_idx = index;
            }
            if (pwl_idx == 0) {
                return -1;
            }
            //y = slope * (Xn - Xn-1) + Yn-1
            raw32_out[pixel_idx] = (raw16_in[pixel_idx] - pwl_prm->x_cood[pwl_idx - 1]) * pwl_prm->slope[pwl_idx] \
                                 + pwl_prm->y_cood[pwl_idx - 1];

            ClipMinMax(raw32_out[pixel_idx], isp_prm->info.max_val, 0);
        }
    }

    SwapMem<void>(frame->data.raw_s32_o, frame->data.raw_s32_i);

    return 0;
}

void RegisterDePwlMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_UINT16;
    mod.out_type = DataPtrTypes::TYPE_INT32;


    mod.in_domain = ColorDomains::RAW;
    mod.out_domain = ColorDomains::RAW;

    mod.name = MOD_NAME;
    mod.run_function = Depwl;

    RegisterIspModule(mod);
}