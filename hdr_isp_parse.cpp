/**
 * @file parse.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief
 * @version 0.1
 * @date 2023-07-29
 *
 * Copyright (c) of ADAS_EYES 2023
 *
 */

#include "common/common.h"
#include "modules/modules.h"
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

std::list<std::string> split(const std::string &str, const std::string &pattern)
{
    std::list<std::string> res;
    if (str == "")
        return res;
    std::string strs = str + pattern;
    size_t pos = strs.find(pattern);

    while (pos != strs.npos)
    {
        std::string temp = strs.substr(0, pos);
        res.push_back(temp);

        strs = strs.substr(pos + 1, strs.size());
        pos = strs.find(pattern);
    }

    return res;
}

HdrIspErrCode ParseIspCfgFile(const std::string cfg_file_path, IspPrms &isp_prm)
{

    std::ifstream fs(cfg_file_path);

    if (!fs.is_open())
    {
        LOG(ERROR) << cfg_file_path << " open failed";
        return HdrIspErrCode::ERR_FILE_INVAILD;
    }

    json j_root;

    fs >> j_root;

    try {
        std::string device = j_root["device"];
        isp_prm.device = DeviceType::HOST_CPU;
        if (device == "gpu") {
            isp_prm.device = DeviceType::SLAVE_GPU;
        }

        std::string run_mode = j_root["mode"];
        isp_prm.mode = AppMode::OFFLINE_MODE;
        if (run_mode == "online") {
            isp_prm.mode = AppMode::ONLINE_MODE;
        }

        isp_prm.topic = j_root["topic"];

        // raw path
        isp_prm.raw_file = j_root["raw_file"];
        isp_prm.out_file_path = j_root["out_file_path"];
        // sensor info
        isp_prm.sensor_name = j_root["info"]["sensor_name"];
        LOG(INFO) << "Sensor Name: " << isp_prm.sensor_name;

        auto cfa_str = j_root["info"]["cfa"];
        if (cfa_str == "RGGB")
        {
            isp_prm.info.cfa = CfaTypes::RGGB;
        }
        else if (cfa_str == "BGGR")
        {
            isp_prm.info.cfa = CfaTypes::BGGR;
        }
        else if (cfa_str == "GBRG")
        {
            isp_prm.info.cfa = CfaTypes::GBRG;
        }
        else if (cfa_str == "GRBG")
        {
            isp_prm.info.cfa = CfaTypes::GRBG;
        }
        LOG(INFO) << "Sensor CFA: " << cfa_str;

        auto raw_type_str = j_root["info"]["data_type"];
        if (raw_type_str == "RAW10")
        {
            isp_prm.info.dt = RawDataTypes::RAW10;
        }
        else if (raw_type_str == "RAW12")
        {
            isp_prm.info.dt = RawDataTypes::RAW12;
        }
        else if (raw_type_str == "RAW14")
        {
            isp_prm.info.dt = RawDataTypes::RAW14;
        }
        else if (raw_type_str == "RAW16")
        {
            isp_prm.info.dt = RawDataTypes::RAW16;
        }
        LOG(INFO) << "Sensor DT: " << raw_type_str;

        isp_prm.info.bpp = static_cast<int>(j_root["info"]["bpp"]);
        int max_bit = int(j_root["info"]["max_bit"]);
        isp_prm.info.max_val = (1 << max_bit) - 1;
        isp_prm.info.width = static_cast<int>(j_root["info"]["width"]);
        isp_prm.info.height = static_cast<int>(j_root["info"]["height"]);
        isp_prm.info.mipi_packed = static_cast<int>(j_root["info"]["mipi_packed"]);
        LOG(INFO) << "Sensor Resolution: " << isp_prm.info.width << "*" << isp_prm.info.height;
        //
        std::string pipeline = j_root["pipe"];
        isp_prm.pipe = std::move(split(pipeline, "|"));
        // blc
        isp_prm.blc = j_root["blc"];
        // wbgain
        auto d65_gains = j_root["wb_gain"]["d65_gain"];
        if (d65_gains.size() != 4)
        {
            LOG(ERROR) << "d65 gains size error";
            return HdrIspErrCode::ERR_PRM_INVAILED;
        }
        for (int i = 0; i < (int)d65_gains.size(); ++i)
        {
            isp_prm.wb_gains.d65_gain[i] = d65_gains[i];
            //LOG(INFO) << "d65" << isp_prm.wb_gains.d65_gain[i];
        }
        auto d50_gains = j_root["wb_gain"]["d50_gain"];
        if (d50_gains.size() != 4)
        {
            LOG(ERROR) << "d50 gains size error";
            return HdrIspErrCode::ERR_PRM_INVAILED;
        }
        for (int i = 0; i < (int)d50_gains.size(); ++i)
        {
            isp_prm.wb_gains.d50_gain[i] = d50_gains[i];
        }

        // pwl
        isp_prm.depwl_prm.pedestal = j_root["depwl"]["pedestal"];
        isp_prm.depwl_prm.pwl_nums = j_root["depwl"]["pwl_nums"];
        auto pwl_x = j_root["depwl"]["pwl_x"];
        auto pwl_y = j_root["depwl"]["pwl_y"];
        auto pwl_slope = j_root["depwl"]["slope"];
        if (((int)pwl_x.size() != isp_prm.depwl_prm.pwl_nums) || ((int)pwl_y.size() != isp_prm.depwl_prm.pwl_nums) || (pwl_slope.size() != isp_prm.depwl_prm.pwl_nums))
        {
            LOG(ERROR) << "pwl input prms error";
            return HdrIspErrCode::ERR_PRM_INVAILED;
        }

        for (int i = 0; i < isp_prm.depwl_prm.pwl_nums; ++i)
        {
            isp_prm.depwl_prm.x_cood[i] = pwl_x[i];
            isp_prm.depwl_prm.y_cood[i] = pwl_y[i];
            isp_prm.depwl_prm.slope[i] = pwl_slope[i];
        }

        // pwl
        isp_prm.ltm_prms.constrast = j_root["ltm"]["constrast"];
        isp_prm.ltm_prms.in_bits = j_root["ltm"]["in_bit"];
        isp_prm.ltm_prms.out_bits = j_root["ltm"]["out_bit"];
        isp_prm.ltm_prms.kernel_size = j_root["ltm"]["kernel_size"];
        if (isp_prm.ltm_prms.kernel_size > kMaxLtmKenerlSize) {
            isp_prm.ltm_prms.kernel_size = kMaxLtmKenerlSize;
        }
        isp_prm.ltm_prms.center = isp_prm.ltm_prms.kernel_size >> 1;
        isp_prm.ltm_prms.range_sigma = j_root["ltm"]["range_sigma"];
        isp_prm.ltm_prms.space_sigma = j_root["ltm"]["space_sigma"];

        isp_prm.rgb_gamma.nums = j_root["rgbgamma"]["gammalut_nums"];
        isp_prm.rgb_gamma.in_bits = j_root["rgbgamma"]["in_bit"];
        isp_prm.rgb_gamma.out_bits = j_root["rgbgamma"]["out_bit"];
        auto gamma_curve = j_root["rgbgamma"]["gammalut"];
        if (gamma_curve.size() != isp_prm.rgb_gamma.nums)
        {
            LOG(ERROR) << "rgb gamma input prms error";
            return HdrIspErrCode::ERR_PRM_INVAILED;
        }

        for (int i = 0; i < (int)gamma_curve.size(); ++i)
        {
            isp_prm.rgb_gamma.curve[i] = gamma_curve[i];
        }

        isp_prm.y_gamma.nums = j_root["ygamma"]["gammalut_nums"];
        isp_prm.y_gamma.in_bits = j_root["ygamma"]["in_bit"];
        isp_prm.y_gamma.out_bits = j_root["ygamma"]["out_bit"];
        gamma_curve = j_root["ygamma"]["gammalut"];
        if ((int)gamma_curve.size() != isp_prm.y_gamma.nums)
        {
            LOG(ERROR) << "y gamma input prms error";
            return HdrIspErrCode::ERR_PRM_INVAILED;
        }

        for (int i = 0; i < (int)gamma_curve.size(); ++i)
        {
            isp_prm.y_gamma.curve[i] = gamma_curve[i];
        }

        isp_prm.sat_prms.rotate_angle = j_root["saturation"]["rotate_angle"];

        isp_prm.contrast_prms.ratio = j_root["contrast"]["ratio"];

        isp_prm.sharpen_prms.ratio = j_root["sharpen"]["ratio"];

        isp_prm.dpc_prms.thres = j_root["dpc"]["thres"];

        std::string dpc_mode =  j_root["dpc"]["mode"];
        if (dpc_mode == "mean") {
            isp_prm.dpc_prms.mode = DpcMode::MEAN;
        } else {
            isp_prm.dpc_prms.mode = DpcMode::GRADIENT;
        }

        if ((j_root["lsc"]["mesh_width_nums"] != kLscMeshPointHNums)
            || (j_root["lsc"]["mesh_height_nums"] != kLscMeshPointVNums)) {
            LOG(ERROR) << "lsc config prms error";
            return HdrIspErrCode::ERR_PRM_INVAILED;
        }


        auto lsc_data_arr = j_root["lsc"]["data"];
        if (lsc_data_arr.size() != kLscMeshPointVNums) {
            LOG(ERROR) << "lsc config lsc_data_arr error";
            return HdrIspErrCode::ERR_PRM_INVAILED;
        }

        for (int idy = 0; idy < kLscMeshPointVNums; ++idy) {
            auto arr = lsc_data_arr[idy];
            if (arr.size() != kLscMeshPointHNums) {
                LOG(ERROR) << "lsc config lsc_data_arr error";
                return HdrIspErrCode::ERR_PRM_INVAILED;
            }
            for (int idx = 0; idx < kLscMeshPointHNums; ++idx) {
                //first verison, use one prm
                isp_prm.lsc_prms.mesh_b[idy][idx] = arr[idx];
                isp_prm.lsc_prms.mesh_gr[idy][idx] = arr[idx];
                isp_prm.lsc_prms.mesh_gb[idy][idx] = arr[idx];
                isp_prm.lsc_prms.mesh_r[idy][idx] = arr[idx];
            }
        }
    }
    catch (std::exception& e) 
    {
        LOG(ERROR) << e.what();
        fs.close();
        exit(1);
    }


    fs.close();
    LOG(INFO) << "parse exit";
    return HdrIspErrCode::SUCCESS;
}