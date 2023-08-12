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
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

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

int ParseCfgFile(const std::string cfg_file_path, IspPrms &isp_prm)
{

    cv::FileStorage fs(cfg_file_path, cv::FileStorage::READ);

    if (!fs.isOpened())
    {
        LOG(ERROR) << cfg_file_path << " open failed";
        return -1;
    }

    cv::FileNode root;

    // raw path
    isp_prm.raw_file = fs["raw_file"].string();
    isp_prm.out_file_path = fs["out_file_path"].string();
    // sensor info
    root = fs["info"];
    isp_prm.sensor_name = root["sensor_name"].string();
    LOG(INFO) << "Sensor Name: " << isp_prm.sensor_name;

    auto cfa_str = root["cfa"].string();
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

    auto raw_type_str = root["data_type"].string();
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

    isp_prm.info.bpp = static_cast<int>(root["bpp"].real());
    int max_bit = int(root["max_bit"].real());
    isp_prm.info.max_val = (1 << max_bit) - 1;
    isp_prm.info.width = static_cast<int>(root["width"].real());
    isp_prm.info.height = static_cast<int>(root["height"].real());
    isp_prm.info.mipi_packed = static_cast<int>(root["mipi_packed"].real());
    LOG(INFO) << "Sensor Resolution: " << isp_prm.info.width << "*" << isp_prm.info.height;
    //
    root = fs["pipe"];
    isp_prm.pipe = std::move(split(root.string(), "|"));
    // blc
    isp_prm.blc = static_cast<int>(fs["blc"].real());
    // wbgain
    root = fs["wb_gain"];
    auto d65_gains = root["d65_gain"];
    if (d65_gains.size() != 4)
    {
        LOG(ERROR) << "d65 gains size error";
        return -1;
    }
    for (int i = 0; i < d65_gains.size(); ++i)
    {
        isp_prm.wb_gains.d65_gain[i] = d65_gains[i].real();
        // LOG(INFO) << "d65" << isp_prm.wb_gains.d65_gain[i];
    }
    auto d50_gains = root["d50_gain"];
    if (d50_gains.size() != 4)
    {
        LOG(ERROR) << "d50 gains size error";
        return -1;
    }
    for (int i = 0; i < d50_gains.size(); ++i)
    {
        isp_prm.wb_gains.d50_gain[i] = d50_gains[i].real();
    }
    // pwl
    root = fs["depwl"];
    isp_prm.depwl_prm.pedestal = root["pedestal"].real();
    isp_prm.depwl_prm.pwl_nums = root["pwl_nums"].real();
    auto pwl_x = root["pwl_x"];
    auto pwl_y = root["pwl_y"];
    auto pwl_slope = root["slope"];
    if ((pwl_x.size() != isp_prm.depwl_prm.pwl_nums) || (pwl_y.size() != isp_prm.depwl_prm.pwl_nums) || (pwl_slope.size() != isp_prm.depwl_prm.pwl_nums))
    {
        LOG(ERROR) << "pwl input prms error";
        return -1;
    }

    for (int i = 0; i < isp_prm.depwl_prm.pwl_nums; ++i)
    {
        isp_prm.depwl_prm.x_cood[i] = pwl_x[i].real();
        isp_prm.depwl_prm.y_cood[i] = pwl_y[i].real();
        isp_prm.depwl_prm.slope[i] = pwl_slope[i].real();
    }

    // pwl
    root = fs["ltm"];
    isp_prm.ltm_prms.constrast = root["constrast"].real();
    isp_prm.ltm_prms.in_bits = root["in_bit"].real();
    isp_prm.ltm_prms.out_bits = root["out_bit"].real();

    root = fs["rgbgamma"];
    isp_prm.rgb_gamma.nums = root["gammalut_nums"].real();
    isp_prm.rgb_gamma.in_bits = root["in_bit"].real();
    isp_prm.rgb_gamma.out_bits = root["out_bit"].real();
    auto gamma_curve = root["gammalut"];
    if (gamma_curve.size() != isp_prm.rgb_gamma.nums)
    {
        LOG(ERROR) << "rgb gamma input prms error";
        return -1;
    }

    for (int i = 0; i < gamma_curve.size(); ++i)
    {
        isp_prm.rgb_gamma.curve[i] = gamma_curve[i].real();
    }

    root = fs["ygamma"];
    isp_prm.y_gamma.nums = root["gammalut_nums"].real();
    isp_prm.y_gamma.in_bits = root["in_bit"].real();
    isp_prm.y_gamma.out_bits = root["out_bit"].real();
    gamma_curve = root["gammalut"];
    if (gamma_curve.size() != isp_prm.y_gamma.nums)
    {
        LOG(ERROR) << "y gamma input prms error";
        return -1;
    }

    for (int i = 0; i < gamma_curve.size(); ++i)
    {
        isp_prm.y_gamma.curve[i] = gamma_curve[i].real();
    }

    root = fs["saturation"];
    isp_prm.sat_prms.rotate_angle = root["rotate_angle"].real();


    root = fs["contrast"];
    isp_prm.contrast_prms.ratio = root["ratio"].real();

    root = fs["sharpen"];
    isp_prm.sharpen_prms.ratio = root["ratio"].real();

    fs.release();
    LOG(INFO) << "parse exit";
    return 0;
}