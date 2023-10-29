#ifndef HDR_ISP_COMMON_H
#define HDR_ISP_COMMON_H

#include "easylogging++.h"
#include "types.h"
#include "devices.h"
#include "hw.h"

#define FOR_ITER(index, max) for (int index = 0; index < max; ++index)
#define GET_PIXEL_INDEX(iw, ih, width) ((ih) * (width) + (iw))

#define CLIP_MIN(x, min) do {  \
	if (x < min) x = min;  \
} while (0);

#define CLIP_MAX(x, max) do {  \
	if (x > max) x = max;  \
} while (0);

#define CLIP_MAX_MIN(x, max, min) do {  \
	if (x > max) x = max;  \
	else if (x < min) x = min; \
} while (0);


template <typename _T>
static inline void SwapMem(_T* &a, _T* &b)
{
    _T* tmp = b;
    b = a;
    a = tmp;
}

template <class _T>
static inline _T Min(_T a, _T b)
{
    return (a > b ? b : a);
}


template <class _T>
static inline void ClipMinMax(_T &a, _T max, _T min)
{
    if (a > max)
    {
        a = max;
    }
    else if (a < min)
    {
        a = min;
    }
}



size_t ReadFileToMem(std::string file_name, void *mem, int size);
size_t WriteMemToFile(std::string file_name, void *mem, int size);

#endif /* ISP_COMMON_H */
