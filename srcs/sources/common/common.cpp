/**
 * @file common.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-28
 * 
 * Copyright (c) of ADAS_EYES 2023
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "common/common.h"


size_t ReadFileToMem(std::string file_name, void* mem, int size)
{
    FILE* fp;

    fp = fopen(file_name.c_str(), "rb");

    if (!fp) {
        LOG(ERROR) << file_name << " open failed";
        return -1;
    }

    auto rd = fread(mem, size, 1, fp);

    fclose(fp);

    return rd;
}


size_t WriteMemToFile(std::string file_name, void* mem, int size)
{
    FILE* fp;

    fp = fopen(file_name.c_str(), "wb+");

    if (!fp) {
        LOG(ERROR) << file_name << " open failed";
        return -1;
    }

    int rd = fwrite(mem, size, 1, fp);

    fclose(fp);

    return rd;
}