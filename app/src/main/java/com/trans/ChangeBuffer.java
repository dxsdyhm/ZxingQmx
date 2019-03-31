package com.trans;

/**
 * @Author: dxs
 * @time: 2019/3/27
 * @Email: duanxuesong12@126.com
 */
public class ChangeBuffer {
    static {
        System.loadLibrary("native-lib");
    }
    public native static int mapInit();
    public native static byte[] changeImgBuffer(byte[] buffer,int w,int h);
    public native static String changeImgBufferNative(byte[] buffer,int w,int h);
}
