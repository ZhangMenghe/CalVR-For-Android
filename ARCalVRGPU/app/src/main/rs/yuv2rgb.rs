#pragma version(1)
#pragma rs java_package_name(com.samsung.arcalvr_gpu)
#pragma rs_fp_relaxed

rs_allocation gYUV;
uint32_t gW;
uint32_t gH;

uchar4 __attribute__((kernel)) YUV2RGB(uint32_t x,uint32_t y)
{
    uchar yps = rsGetElementAt_uchar(gYUV, x, y);
    uchar u = rsGetElementAt_uchar(gYUV,(x & ~1),gH + (y>>1));
    uchar v = rsGetElementAt_uchar(gYUV,(x & ~1)+1,gH + (y>>1));
    uchar4 rgb = rsYuvToRGBA_uchar4(yps, u, v);
    return rgb;
}