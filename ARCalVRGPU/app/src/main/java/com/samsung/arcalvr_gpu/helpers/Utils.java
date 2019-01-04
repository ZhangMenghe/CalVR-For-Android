package com.samsung.arcalvr_gpu.helpers;

import android.graphics.Bitmap;

import org.opencv.core.Mat;

public class Utils {
    public static void bitmapToMat(Bitmap bmp, Mat mat, boolean unPremultiplyAlpha) {
        if (bmp == null)
            throw new java.lang.IllegalArgumentException("bmp == null");
        if (mat == null)
            throw new java.lang.IllegalArgumentException("mat == null");
//        nBitmapToMat2(bmp, mat.nativeObj, unPremultiplyAlpha);
    }
}
