#ifndef HDR_ISP_TYPE_H
#define HDR_ISP_TYPE_H

#include <stdint.h>
#include <string>


enum class HdrIspErrCode
{
    SUCCESS,
    ERR_PRM_INVAILED,
    ERR_FILE_INVAILD,
    ERR_UNIMPLEMENT
};

enum class CfaTypes
{
    RGGB,
    GRBG,
    BGGR,
    GBRG,

    CFA_MAX
};

enum class PixelCfaTypes
{
    GR,
    GB,
    R,
    B
};

enum class ColorDomains
{
    RAW,
    BGR,
    YUV,
    HSV,
};

enum class DataPtrTypes
{
    TYPE_INT8,
    TYPE_UINT8,
    TYPE_UINT16,
    TYPE_INT16,
    TYPE_INT32,
    TYPE_UINT32,
    TYPE_FLOAT32,
    TYPE_MAX
};

enum class RawDataTypes
{
    RAW8 = 0x22,
    RAW10 = 0x2a,
    RAW12 = 0x2C,
    RAW14 = 0x2D,
    RAW16 = 0x2E,
    RAW20 = 0x2F,
    RAW24 = 0x30,
};

enum class YuvTypes
{
    YUV422_8,
    YUV422_10,
};

enum class DeviceType : uint8_t
{
	HOST_CPU,
	SLAVE_GPU
};

enum class AppMode : uint8_t
{
	OFFLINE_MODE,
	ONLINE_MODE
};

struct ImageInfo
{
    /* data */
    int width;
    int height;

    int bpp;
    int max_val;

    bool mipi_packed;

    CfaTypes cfa;
    ColorDomains domain;
    RawDataTypes dt;
    YuvTypes yuv_type;
};

struct Memory
{
    void* addr;
    int size;
    Memory() {
        addr = nullptr;
        size = 0;
    }
    ~Memory() {
        
    }
};


struct ImageMem
{
    /* all data is double frame, enable pipeline in gpu */
    Memory *raw_u8_i;
    Memory *raw_u16_i;
    Memory *raw_u16_o;
    Memory *raw_s32_i;
    Memory *raw_s32_o;
    Memory *bgr_s32_i;
    Memory *bgr_s32_o;
    Memory *bgr_u8_o;
    struct YuvMem
    {
        Memory *y;
        Memory *u;
        Memory *v;
    } yuv_f32_i, yuv_f32_o, yuv_u8_i, yuv_u8_o;

    // ltm
    // void *y_log_f32_;
};

static constexpr int kCfaNums = static_cast<int>(CfaTypes::CFA_MAX);
static constexpr PixelCfaTypes kPixelCfaLut[kCfaNums][2][2] = {
    {
        {PixelCfaTypes::R, PixelCfaTypes::GR},
        {PixelCfaTypes::GB, PixelCfaTypes::B},
    },
    {
        {PixelCfaTypes::GR, PixelCfaTypes::R},
        {PixelCfaTypes::B, PixelCfaTypes::GB},
    },
    {
        {PixelCfaTypes::B, PixelCfaTypes::GB},
        {PixelCfaTypes::GR, PixelCfaTypes::R},
    },
    {
        {PixelCfaTypes::GB, PixelCfaTypes::B},
        {PixelCfaTypes::R, PixelCfaTypes::GR},
    }
};
/**
 * @brief for depwl use
 */
#define MAX_PWL_NUMS 24
#define MAX_GAMMA_NUMS 21
struct DePwlPrms
{
    int pwl_nums;
    bool pedestal_before_pwl;
    int pedestal;
    int x_cood[MAX_PWL_NUMS];
    int y_cood[MAX_PWL_NUMS];
    int slope[MAX_PWL_NUMS];
};

struct CcmPrms
{
    float ccm[3][3] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}};
};

struct WbGain
{
    // R G G B
    float d65_gain[4];
    float d50_gain[4];
    float f11_gain[4];
    float f12_gain[4];
};

struct GammmaCurve
{
    // 0-1
    int nums;
    int in_bits;
    int out_bits;
    float curve[MAX_GAMMA_NUMS];
};

static constexpr int kMaxLtmKenerlSize = 9;
//#define kMaxLtmKenerlSize 9

struct LtmPrms
{
    int kernel_size;
    int center;
    int gauss_kernel[kMaxLtmKenerlSize][kMaxLtmKenerlSize];
    float range_kernel[kMaxLtmKenerlSize][kMaxLtmKenerlSize];
    int gauss_kernel_sum;
    float range_sigma;
    float space_sigma;
    float constrast;
    int in_bits;
    int out_bits;
};

struct SaturationPrms
{
    float rotate_angle;
};

struct ContrastPrms
{
    float ratio;
};

struct SharpenPrms
{
    float ratio;
};


static constexpr int kLscMeshBoxHNums = 10;
static constexpr int kLscMeshBoxVNums = 9;
static constexpr int kLscMeshPointHNums = kLscMeshBoxHNums + 1;
static constexpr int kLscMeshPointVNums = kLscMeshBoxVNums + 1;

struct LscPrms
{
    float mesh_r[kLscMeshPointVNums][kLscMeshPointHNums];
    float mesh_gr[kLscMeshPointVNums][kLscMeshPointHNums];
    float mesh_gb[kLscMeshPointVNums][kLscMeshPointHNums];
    float mesh_b[kLscMeshPointVNums][kLscMeshPointHNums];

    //LscPrms()
    //{
    //    for (int idy = 0; idy < kLscMeshPointVNums; ++idy) {
    //        for (int idx = 0; idx < kLscMeshPointHNums; ++idx) {
    //            mesh_r[idy][idx] = 1;
    //            mesh_gr[idy][idx] = 1;
    //            mesh_gb[idy][idx] = 1;
    //            mesh_b[idy][idx] = 1;
    //        }
    //    }
    //}
};

enum class DpcMode { 
    MEAN,
    GRADIENT
}; 
struct DpcPrms
{
    int thres = 30;
    DpcMode mode = DpcMode::GRADIENT;
};

#endif