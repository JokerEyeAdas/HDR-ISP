#ifndef HDR_ISP_FRAME_H
#define HDR_ISP_FRAME_H

#include "common.h"

#define SAFE_FREE(arr)     \
    do                     \
    {                      \
        if (arr)           \
        {                  \
            delete[] arr;  \
            arr = nullptr; \
        }                  \
    } while (0)

class Frame
{
private:
    int  FrameMemMalloc(int width, int height);
    void FrameMemFree();

    int  GpuFrameMemMalloc(int width, int height);
    void GpuFrameMemFree();
    
public:
    bool IsSupportGpu() {return gpu_enable_;};
    cudaStream_t GetStream() {return stream_;};

private:
    cudaStream_t stream_ = nullptr;
    bool gpu_enable_ = false;


public:
    ImageInfo info;
    ImageMem data;
    ImageMem gpu_data;

public:
    Frame(ImageInfo &img_info, bool gpu_enable_ = false);
    ~Frame();

public:
    HdrIspErrCode ReadFileToFrame(std::string file, int size, bool to_gpu = true);
    HdrIspErrCode CpMemToFrame(void *src, int len, DeviceType to_dev);
};

#endif // !ADAS_ISP_FRAME_H
