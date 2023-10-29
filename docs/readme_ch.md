# Image Signal Process For HDR CMOS Image Sensor

**[EN](../readme.md) | [CH](./readme_ch.md)**

# 项目描述


**一个使用C++编写的、用于HDR相机的ISP Pipeline**

- ISP模块使用C风格编写
- 没有其他库依赖
- ~~可以在任意嵌入式平台上部署~~
- 可以在支持Cuda的设备上运行，如PC、英伟达Jeston系列嵌入式芯片
- 可以自定义你的pipeline通过json配置
- 可以通过修改配置使ISP运行在CPU或者GPU上
- 支持ROS2在线处理图像以及离线处理RAW图像

# 默认Pipeline

![Pipeline](pipeline.png)

# 支持的ISP模块列表(CPU以及GPU均支持)

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

# 如何编译与运行

## Linux系统
**开发环境 :** 
* nvcc
* cmake
* g++

**编译**

- **不使用ROS2**
```bash
git clone https://github.com/JokerEyeAdas/HDR-ISP
cd HDR-ISP/
git checkout gpu_dev
mkdir build
cmake ..
make -j12
```
- **使用ROS2编译**
1. 克隆仓库
```bash
git clone https://github.com/JokerEyeAdas/HDR-ISP
cd HDR-ISP/
git checkout gpu_dev
```

2. 编辑 CMakeLists.txt，设置ROS2_ENABLE成true
```CMakeLists.txt
set(ROS2_ENABLE true)
```

3. 编译

```bash
cd ${ros2_wk}
colcon build
```

## Windows

**开发环境 (x64):** 
- vs code
- cmake
- nvcc
- vs2019 c++ gen tool

![build tool](./compile.png) 

**编译**

- **不使用 ROS2**

```bash
git clone https://github.com/JokerEyeAdas/HDR-ISP
code HDR-ISP
git checkout gpu_dev
#cmake choose Debug or Release
#compiler choose xxx-amd64
#build all
```

- **使用ROS2**

1. 克隆仓库
```bash
git clone https://github.com/JokerEyeAdas/HDR-ISP
cd HDR-ISP/
git checkout gpu_dev
```
2. 编辑CMakeLists.txt设置ROS2_ENABLE为true
```CMakeLists.txt
set(ROS2_ENABLE true)
```
3. 编译

```bash
cd ${ros2_wk}
colcon build --merge-install
```

## 运行

```bash
cd build
#cp cfgs and data
cp -r ../data/ ./
cp -r ../cfgs/ ./
#run isp
./HDR_ISP ./cfgs/isp_config_cannon.json
```

## 改变模式以及运行设备

- 使用GPU加速

通过Json进行配置:
```json
    "device":"gpu",
```

- 使用CPU加速

通过Json进行配置:
```json
    "device":"cpu",
```

- ROS2实时处理模式以及修改Topic
```json
    "mode":"online",
    "topic":"/raw/connan",
```

- 离线处理RAW图
```json
    "mode":"offline",
    "raw_file": "./data/connan_raw14.raw",
    "out_file_path": "./",
```

## 如何调试参数

**通过修改json配置来配置ISP模块的基本参数**

如，修改sensor基本参数：
```json
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

修改rgb gamma参数如下：

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

# 结果

## Pipeline运行结果

|备注|图像|
|-------|-------|
|Raw|![raw](ISP/connan_raw14.png)|
|ISP Result(Ours)|![ISP](ISP/isp_result.png)|
|FastOpenISP Result|![Fast ISP](ISP/color_checker.png)|

------

## 细节对比

|ISP|图像1|图像2|备注|
|-------|-------|--------|-------|
|Ours|![Our Detail](ISP/our_detail.png)|![Our Detail](ISP/our_sharpen.png)|细节与边界明显保留|
|Fast Open Isp|![Open Isp](ISP/fast_detail.png)|![Open Isp](ISP/others_sharpen.png)|细节丢失以及Color banding|


## 使用ROS以及GPU

- GPU性能(不发布ISP 处理结果)

|Index|Resolution|Fps|
|----|-----|----|
|1|6080*4044|15|
|2|3840*2160|40|
|3|1920*1080|125|

- ROS2 Online mode
  
![ROS2](./ros2_online.png)


# 后续工作

## TBD

* 编写Python或者GUI调试工具

## 资料获取
- 如果想学习源码、获取模块说明，请加入星球，我们共同开发！
  
![knowledge](./knowledge.png)

- 加入星球:

![知识星球](./xingqiu.jpg)


## 项目支持

- 感谢您的支持，希望我的项目对您有帮助~

![Appreciation Code](AppreciationCode.png)

# 参考仓库

感谢下述仓库以及仓库作者！！

|序号|仓库|开发语言|备注|
|-----|-----|-----|-----|
|0|[OpenISP](https://github.com/cruxopen/openISP)|Python|image signal process in C style|
|1|[fast-openIsp](https://github.com/QiuJueqin/fast-openISP)|Python|open isp 快速版本|
|2|[ISP Lab](https://github.com/yuqing-liu-dut/ISPLab)|C++|C++版本开源ISP|
|3|[xk-ISP](https://github.com/openasic-org/xkISP)|C++|复旦大学开源HLS ISP|


# 关注我


**知乎主页：[ADAS之眼](https://www.zhihu.com/people/fen-shi-qing-nian-29) | 个人网站：[ADAS之眼](https://jokereyeadas.github.io/) | 公众号：**

**![公众号](wechat.png)**


**ADAS之眼权利保留@2023，未经允许禁止商业用途**

-----