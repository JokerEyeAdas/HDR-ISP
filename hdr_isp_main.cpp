/**
 * @file main.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief
 * @version 1.1
 * @date 2023-07-28
 *
 * Copyright (c) of ADAS_EYES 2023
 *
 */

#include "easylogging++.h"
#include "EasyBMP.h"
#include "common/pipeline.h"


INITIALIZE_EASYLOGGINGPP

extern HdrIspErrCode ParseIspCfgFile(const std::string cfg_file_path, IspPrms &isp_prm);
extern HdrIspErrCode RosIspOnlineRun(int argc, char** argv, Frame& frame, IspPrms& isp_prm);

HdrIspErrCode RunOffLine(Frame& frame, IspPrms& isp_prm, bool is_enable_gpu)
{
    LOG(INFO) << "========APP RUN OFFLINE MODE=======";
    auto width = isp_prm.info.width;
    auto height = isp_prm.info.height;
    auto ret = frame.ReadFileToFrame(isp_prm.raw_file, width * height * isp_prm.info.bpp / 8, is_enable_gpu);
    if (ret != HdrIspErrCode::SUCCESS) {
        LOG(ERROR) << "frame read raw file " << isp_prm.raw_file << " faild";
        return HdrIspErrCode::ERR_FILE_INVAILD;
    }
    auto pipeline = std::move(GetIspPipeImplFromDevice(isp_prm.device, isp_prm.pipe));
    if (!pipeline->IsPipeVaild()) {
        LOG(ERROR) << "pipeline is invailed";
        return HdrIspErrCode::ERR_PRM_INVAILED;
    }
    pipeline->LoadPrms(&isp_prm);
    auto isp_ret = pipeline->RunPipe(&frame, &isp_prm);

    if (isp_ret == 0)
    {
        LOG(INFO) << "out image dir: " << (isp_prm.out_file_path + "isp_result.bmp").c_str();
        WriteBgrMemToBmp((isp_prm.out_file_path + "isp_result.bmp").c_str(), (char *)frame.data.bgr_u8_o->addr, width, height, 24);
        LOG(INFO) << "====[OK]===APP Common Exit=======";
    }
    else
    {
        LOG(ERROR) << "---[FAILED]---Pipe run Error Exit------";
    }
    return HdrIspErrCode::SUCCESS;
}

int main(int argc, char *argv[])
{
    LOG(INFO) << "APP Start Running";

    if (argc < 2)
    {
        LOG(ERROR) << "usage :\n\t ./app isp_cfg.json\n";
        return 0;
    }

    IspPrms isp_prm;

    auto ret = ParseIspCfgFile(argv[1], isp_prm);
    if (ret != HdrIspErrCode::SUCCESS)
    {
        LOG(ERROR) << argv[1] << " parse failed\n";
        return -1;
    }

    bool is_enable_gpu = isp_prm.device == DeviceType::SLAVE_GPU;
    if (is_enable_gpu) {
        int count;
        cudaGetDeviceCount(&count);
        LOG(INFO) << "device gpu nums: " << count;
        if (count <= 0) {
            LOG(ERROR) << "device gpu is not online";
            return -1;
        }
    }
    
    Frame frame(isp_prm.info,  is_enable_gpu);

    if (isp_prm.mode == AppMode::OFFLINE_MODE) {
        RunOffLine(frame, isp_prm, is_enable_gpu);
    } else {
        RosIspOnlineRun(argc, argv, frame, isp_prm);
    }

    return 0;
}