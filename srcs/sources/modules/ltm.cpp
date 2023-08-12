/**
 * @file raw_ltm.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief
 * @version 0.1
 * @date 2023-07-27
 *
 * Copyright (c) of ADAS_EYES 2023
 *
 */

#include "modules/modules.h"

#define MOD_NAME "ltm"

static float g_guass_kernel[kMaxLtmKenerlSize][kMaxLtmKenerlSize];
static float g_range_kernel[kMaxLtmKenerlSize][kMaxLtmKenerlSize];
static float g_final_kernel[kMaxLtmKenerlSize][kMaxLtmKenerlSize];

static void bgrToYFast(int32_t *bgr, float *y, float *y_log, int width, int height)
{
    int max_y = 0, min = 1000;

    FOR_ITER(ih, height)
    {
        FOR_ITER(iw, width)
        {
            int pixel_index = GET_PIXEL_INDEX(iw, ih, width);
            y[pixel_index] = (bgr[pixel_index * 3 + 0] * 20 + bgr[pixel_index * 3 + 1] * 40 + bgr[pixel_index * 3 + 2]) / 61.0f;
            y_log[pixel_index] = log10f(y[pixel_index]);
            if (y[pixel_index] > max_y)
            {
                max_y = y[pixel_index];
            }
            else if (y[pixel_index] < min)
            {
                min = y[pixel_index];
            }
        }
    }
}

static void BilateralFilter(float *ylog_i, float *bp_log, int kernel_size,
                            int width, int height, float space_sigma,
                            float range_sigma, float &log_base_max, float &log_base_min)
{
    log_base_max = 0;
    log_base_min = 100;

    if (kernel_size > kMaxLtmKenerlSize)
    {
        kernel_size = kMaxLtmKenerlSize;
    }
    // std::cout << kernel_size << "\n";
    int center = kernel_size >> 1;
    float sigma_2 = space_sigma * space_sigma;

    for (int kh = 0; kh < kernel_size; ++kh)
    {
        for (int kw = 0; kw < kernel_size; ++kw)
        {
            float exp_scale = -0.5f * ((kh - center) * (kh - center) + (kw - center) * (kw - center)) / sigma_2;
            g_guass_kernel[kw][kh] = exp(exp_scale);
            // printf("[%f]", g_guass_kernel[kw][kh]);
        }
        // printf("\n");
    }

    sigma_2 = range_sigma * range_sigma;
    FOR_ITER(ih, height)
    {
        FOR_ITER(iw, width)
        {
            // 每个像素做filter
            float y_gray = 0;
            float filter_kernel_sum = 0;
            float filter_result = 0;
            int pixel_id = GET_PIXEL_INDEX(iw, ih, width);

            for (int kh = -center; kh <= center; ++kh)
            {
                for (int kw = -center; kw <= center; ++kw)
                {
                    int idx = iw + kw;
                    int idy = ih + kh;

                    if ((idx < 0) && (idy < 0)) {
                        y_gray = ylog_i[0];
                    } else if ((idx > 0) && (idy < 0)) {
                        if (idx < width) {
                            y_gray = ylog_i[idx];
                        } else {
                            y_gray = ylog_i[width - 1];
                        }
                    } else if ((idx < 0) && (idy > 0)) {
                        if (idy < height) {
                            y_gray = ylog_i[idy * width];
                        } else {
                            y_gray = ylog_i[(height - 1) * width];
                        }
                    } else if ((idx >= 0) && (idy >= 0)) {
                        if ((idx < width) && (idy < height)) {
                            y_gray = ylog_i[idx + idy * width];
                        } else if ((idx >= width) && (idy < height)) {
                            y_gray = ylog_i[(width - 1) + idy * width];
                        } else if ((idx >= width) && (idy >= height)) {
                            y_gray = ylog_i[height * width - 1];
                        } else if ((idx < width) && (idy >= height)) {
                            y_gray = ylog_i[(height -1) * width + idx];
                        } else {
                            LOG(ERROR) << "error padding";
                            y_gray = 0;
                        }
                    }

                    float exp_scale = (-0.5 * (y_gray -  ylog_i[pixel_id])
                                     * (y_gray -  ylog_i[pixel_id])) /sigma_2;
                    g_range_kernel[kw + center][kh + center] = exp(exp_scale);

                    g_final_kernel[kw + center][kh + center] = g_guass_kernel[kw + center][kh + center] * g_range_kernel[kw + center][kh + center];

                    filter_result += (g_final_kernel[kw + center][kh + center] * y_gray);
                    filter_kernel_sum += g_final_kernel[kw + center][kh + center];
                }
            }
            filter_result = filter_result / filter_kernel_sum;
            bp_log[pixel_id] = filter_result;

            if (bp_log[pixel_id] < 0)
            {
                bp_log[pixel_id] = 0;
            }

            if (filter_result > log_base_max)
            {
                log_base_max = filter_result;
            }
            else if (filter_result < log_base_min)
            {
                log_base_min = filter_result;
            }
        }
    }
}

static int Ltm(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int pixel_idx = 0;

    const auto &ltm_prms = isp_prm->ltm_prms;

    int32_t *bgr_in = reinterpret_cast<int32_t *>(frame->data.bgr_s32_i);
    int32_t *bgr_out = reinterpret_cast<int32_t *>(frame->data.bgr_s32_o);

    // 借用内存
    float *y_log = reinterpret_cast<float *>(frame->data.yuv_f32_i.y);
    float *bp_log = reinterpret_cast<float *>(frame->data.yuv_f32_i.u);
    float *dp_log = reinterpret_cast<float *>(frame->data.yuv_f32_i.v);

    float *y_val = reinterpret_cast<float *>(frame->data.yuv_f32_o.y);
    float *funstion_val = reinterpret_cast<float *>(frame->data.yuv_f32_o.u);

    float max_log_base = 0;
    float min_log_base = 0;

    bgrToYFast(bgr_in, y_val, y_log, frame->info.width, frame->info.height);

    BilateralFilter(y_log, bp_log, 9, frame->info.width, frame->info.height,
                    ltm_prms.space_sigma, ltm_prms.range_sigma, max_log_base, min_log_base);

    FOR_ITER(ih, frame->info.height)
    {
        FOR_ITER(iw, frame->info.width)
        {
            pixel_idx = GET_PIXEL_INDEX(iw, ih, frame->info.width);
            dp_log[pixel_idx] = y_log[pixel_idx] - bp_log[pixel_idx]; // detail layber
        }
    }

    float scale_factor = log10(ltm_prms.constrast) / (max_log_base - min_log_base);
    float max_scale_val = (float)powf(10, scale_factor * max_log_base);

    float ratio = 0;
    int max_out_val = (1 << ltm_prms.out_bits) - 1;
    //LOG(INFO) << "ltm ratio " << scale_factor;
    // std::cout << "max_scale_val " << max_scale_val << "scale_factor " << scale_factor << "max log base " << max_log_base << " min "<< min_log_base<< "max_out_val " << max_out_val;
    FOR_ITER(ih, frame->info.height)
    {
        FOR_ITER(iw, frame->info.width)
        {
            pixel_idx = GET_PIXEL_INDEX(iw, ih, frame->info.width);

            y_log[pixel_idx] = scale_factor * bp_log[pixel_idx] + dp_log[pixel_idx];
            funstion_val[pixel_idx] = (float)powf(10, y_log[pixel_idx]);

            if ((y_val[pixel_idx] > 0) && (funstion_val[pixel_idx] > 0))
                ratio = funstion_val[pixel_idx] / y_val[pixel_idx];
            else
                ratio = 0;
            // std::cout << "ratio " << ratio << " | "<< funstion_val[pixel_idx] <<  " | "<< y_val[pixel_idx]  << "| " << bp_log[pixel_idx] << "|" << dp_log[pixel_idx]<< "\n";
            //LOG(INFO) << "ltm ratio " << ratio;
            bgr_in[3 * pixel_idx + 0] = static_cast<int32_t>(bgr_in[3 * pixel_idx + 0] * ratio);
            bgr_in[3 * pixel_idx + 1] = static_cast<int32_t>(bgr_in[3 * pixel_idx + 1] * ratio);
            bgr_in[3 * pixel_idx + 2] = static_cast<int32_t>(bgr_in[3 * pixel_idx + 2] * ratio);

            bgr_out[3 * pixel_idx + 0] = static_cast<int32_t>(max_out_val * bgr_in[3 * pixel_idx + 0] / max_scale_val);
            bgr_out[3 * pixel_idx + 1] = static_cast<int32_t>(max_out_val * bgr_in[3 * pixel_idx + 1] / max_scale_val);
            bgr_out[3 * pixel_idx + 2] = static_cast<int32_t>(max_out_val * bgr_in[3 * pixel_idx + 2] / max_scale_val);

            ClipMinMax<int32_t>(bgr_out[3 * pixel_idx + 0], max_out_val, 0);
            ClipMinMax<int32_t>(bgr_out[3 * pixel_idx + 1], max_out_val, 0);
            ClipMinMax<int32_t>(bgr_out[3 * pixel_idx + 2], max_out_val, 0);
        }
    }

    SwapMem<void>(frame->data.bgr_s32_i, frame->data.bgr_s32_o);

    return 0;
}

void RegisterLtmMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::BGR;
    mod.out_domain = ColorDomains::BGR;

    mod.name = MOD_NAME;
    mod.run_function = Ltm;

    RegisterIspModule(mod);
}