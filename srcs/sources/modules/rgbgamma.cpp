/**
 * @file rgbgamma.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief
 * @version 0.1
 * @date 2023-08-10
 *
 * Copyright (c) of ADAS_EYES 2023
 *
 */

#include "modules/modules.h"

#define MOD_NAME "rgbgamma"

static int RgbGamma(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int pixel_idx = 0;

    const auto &gamma_prm = isp_prm->rgb_gamma;

    float step_coff = (float)(gamma_prm.nums - 1) / (1 << gamma_prm.in_bits);
    float out_max = (1 << gamma_prm.out_bits) - 1;

    int32_t *bgr_i = reinterpret_cast<int32_t *>(frame->data.bgr_s32_i);
    int32_t *bgr_o = reinterpret_cast<int32_t *>(frame->data.bgr_s32_o);

    FOR_ITER(h, frame->info.height)
    {
        FOR_ITER(w, frame->info.width)
        {
            pixel_idx = h * frame->info.width + w;

            FOR_ITER(color_idx, 3)
            {
                auto color = bgr_i[3 * pixel_idx + color_idx];

                float cuvre_id_f = color * step_coff;
                int curve_id = static_cast<int>(cuvre_id_f);
                // scale to 0~1
                float scale = (cuvre_id_f - curve_id) * (gamma_prm.curve[curve_id + 1] - gamma_prm.curve[curve_id]) + gamma_prm.curve[curve_id];
                // get scale value
                color = out_max * scale;
                // ClipMinMax(color, isp_prm->info.max_val, 0);
                bgr_o[3 * pixel_idx + color_idx] = color;
            }
        }
    }

    SwapMem<void>(frame->data.bgr_s32_i, frame->data.bgr_s32_o);

    return 0;
}

void RegisterRgbGammaMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::BGR;
    mod.out_domain = ColorDomains::BGR;

    mod.name = MOD_NAME;
    mod.run_function = RgbGamma;

    RegisterIspModule(mod);
}