# Image Signal Process For HDR CMOS Image Sensor

**[CH](docs/readme_ch.md) | [EN](./readme.md)**


# Project Description


**An ISP pipeline written in C++ for HDR cameras**

- The ISP modules are written in C style
- Without other library dependencies
- Can be deployed various embedded platforms
- Customize your pipeline through json configuration

# Default Pipeline

![Pipeline](docs/pipeline.png)
# Support ISP Module Lists

- Raw Domain
    - [x] MipiUnPack: mipi raw data unpack to raw16
    - [x] DePwl: decode the pwl curve
    - [x] Dpc: dynamic pixel correct
    - [x] Lsc: lens shding correct
    - [x] Blc: black level correct
    - [ ] Rns: raw noise filter
    - [x] WbGain: white balance gain
    - [x] Demoasic: change raw to rgb
- RGB Domain
    - [x] Ltm: local tone mapping
    - [x] RgbGamma: rgb gamma curve
    - [x] Ccm: color correct matrix
    - [x] Rgb2Yuv: rgb domain to yuv domain
- YUV Domain
    - [x] YGamma: gray gamma curve
    - [x] Contrast: gray contrast adjust
    - [x] Sharpen: gray usm sharpen
    - [x] Cns: chrome noise filter
    - [x] Saturation: chrome saturation
    - [x] Yuv2Rgb: yuv domain to rgb domain

# How To Build and Run

## Linux
**Develop Env :** 
```bash
#dependencies install(cmake and opencv)
sudo apt update
sudo apt install cmake
```

**Build**

```bash
git clone https://github.com/JokerEyeAdas/HDR-ISP
cd HDR-ISP/
mkdir build
cmake ..
make -j12
```
## Windows

**Develop Env (x64):** 
- vs code
- cmake
- vs2019 c++ gen tool

![build tool](docs/compile.png) 

**Build**

```bash
git clone https://github.com/JokerEyeAdas/HDR-ISP
code HDR-ISP
#cmake choose Debug or Release
#compiler choose xxx-amd64
#build all
```
## How To Run

```bash
cd build
#cp cfgs and data
cp -r ../data/ ./
cp -r ../cfgs/ ./
#run isp
./HDR_ISP ./cfgs/isp_config_cannon.json
```
## How To Tunning Params

**By changing Json Config**

example1, change sensor params:
```json
    "raw_file": "./data/connan_raw14.raw",
    "out_file_path": "./",
    "info": {
        "sensor_name": "cannon",
        "cfa": "RGGB",
        "data_type": "RAW16",
        "bpp": 16,
        "max_bit": 14,
        "width": 6080,
        "height": 4044,
        "mipi_packed": 0
    },
```
example2, change rgb gamma params:
```json
"rgbgamma": {
        "enable": true,
        "in_bit": 10,
        "out_bit": 8,
        "gammalut_nums": 11,
        "gammalut": [
            0,
            0.3504950718773984,
            0.48243595264750255,
            0.57750428843709,
            0.6596458942714417,
            0.731034378464739,
            0.7925580792857235,
            0.8509817015104557,
            0.9029435754464383,
            0.9534255851019492,
            1.0
        ]
    }
```

# Result

## ISP Result

|Notes|Image|
|-------|-------|
|Raw|![raw](/docs/ISP/connan_raw14.png)|
|ISP Result(Ours)|![ISP](docs/ISP/isp_result.png)|
|FastOpenISP Result|![Fast ISP](docs/ISP/color_checker.png)|

------

## Detail Compare

|ISP|Image1|Image2|Notes|
|-------|-------|-------|--------|
|Ours|![Our Detail](docs/ISP/our_detail.png)|![Our Detail](docs/ISP/our_sharpen.png)|Detail and boundaries is clear|
|Fast Open Isp|![Open Isp](docs/ISP/fast_detail.png)|![Open Isp](docs/ISP/others_sharpen.png)|Color banding and detail lost|



# Follow-Up

* Support Dpc, Rns, Cns and other ISP modules;
* Write ISP tunning GUI tools.

# Project Support

- Thanks for your support, we hope that my project can help your work

![Appreciation Code](docs/AppreciationCode.png)

# Reference Repo

Thanks for the following code repository!

|Index|Repo|Language|Notes|
|-----|-----|------|-----|
|0|[OpenISP](https://github.com/cruxopen/openISP)|Python|image signal process in C style|
|1|[fast-openIsp](https://github.com/QiuJueqin/fast-openISP)|Python|open isp speed up verison in python|
|2|[ISP Lab](https://github.com/yuqing-liu-dut/ISPLab)|C++|Isp realized by c++|
|3|[xk-ISP](https://github.com/openasic-org/xkISP)|C++|C++ ISP For HLS on FPGA|



# Follow Me

**Zhihu：[EYES OF ADAS](https://www.zhihu.com/people/fen-shi-qing-nian-29) | Blog：[EYES OF ADAS](https://jokereyeadas.github.io/) | WeChat：**

**![WeChat](docs/wechat.png)**



**Eyes of Adas Copyright Reserved @2023, No commercial use without permission**

-----