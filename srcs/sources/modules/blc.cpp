/**
 * @file blc.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "modules/modules.h"

#define MOD_NAME "blc"

static int Blc(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int pixel_idx = 0;
    int pwl_idx = 0;

    int32_t* raw32_in = reinterpret_cast<int32_t *>(frame->data.raw_s32_i);
    int32_t* raw32_out = reinterpret_cast<int32_t *>(frame->data.raw_s32_o);

    const DePwlPrms *pwl_prm = &(isp_prm->depwl_prm);

    FOR_ITER(ih, frame->info.height)
    {
        FOR_ITER(iw, frame->info.width)
        {
            int pixel_idx = GET_PIXEL_INDEX(iw, ih, frame->info.width);
            raw32_out[pixel_idx] = raw32_in[pixel_idx] - isp_prm->blc;
            ClipMinMax<int32_t>(raw32_out[pixel_idx], (int32_t)isp_prm->info.max_val, 0);
        }
    }

    //std::cout << "blc" << isp_prm->blc << "\n";

    SwapMem<void>(frame->data.raw_s32_i, frame->data.raw_s32_o);
    
    return 0;
}

void RegisterBlcMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::RAW;
    mod.out_domain = ColorDomains::RAW;

    mod.name = MOD_NAME;
    mod.run_function = Blc;

    RegisterIspModule(mod);
}