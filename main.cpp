/**
 * @file main.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief
 * @version 0.1
 * @date 2023-07-28
 *
 * Copyright (c) of ADAS_EYES 2023
 *
 */

#include "easylogging++.h"
#include "EasyBMP.h"
#include "common/pipeline.h"

//#include <opencv2/opencv.hpp>
//#include <opencv2/core.hpp>

INITIALIZE_EASYLOGGINGPP

extern int ParseIspCfgFile(const std::string cfg_file_path, IspPrms &isp_prm);


int main(int argc, char *argv[])
{
    LOG(INFO) << "APP Start Running";

    if (argc < 2)
    {
        LOG(ERROR) << "usage :\n\t ./app isp_cfg.json\n";
        return 0;
    }

    IspPipeline pipeline;
    IspPrms isp_prms;


    auto ret = ParseIspCfgFile(argv[1], isp_prms);
    if (ret)
    {
        LOG(ERROR) << argv[1] << " parse failed\n";
        return -1;
    }

    auto width = isp_prms.info.width;
    auto height = isp_prms.info.height;

    Frame frame(isp_prms.info);

    frame.ReadFileToFrame(isp_prms.raw_file, width * height * isp_prms.info.bpp / 8);

    pipeline.MakePipe(isp_prms.pipe);
    pipeline.PrintPipe();

    ret = pipeline.RunPipe(&frame, &isp_prms);

    if (!ret)
    {
        //cv::Mat isp_result(height, width, CV_8UC3, frame.data.bgr_u8_o);
        //cv::imwrite(isp_prms.out_file_path + "isp_result.png", isp_result);
        WriteBgrMemToBmp((isp_prms.out_file_path + "isp_result.bmp").c_str(), (char *)frame.data.bgr_u8_o, width, height, 24);
        WriteMemToFile(isp_prms.out_file_path + "isp_result_bgr.raw", frame.data.bgr_u8_o, width * height * 3);
        LOG(INFO) << "APP Common Exit";
    }
    else
    {
        LOG(ERROR) << "Pipe run Error Exit";
    }
    return 0;
}