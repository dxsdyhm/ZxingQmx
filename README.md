# ZxingQmx
java与C通用的扫码示例工程
# 区别
```java
decode(byte[] data,int w,int h);
decodeByNative(byte[] data,int w,int h);
```
# 编译
去掉```jni```中报错的代码，直接编译即可，删除```ChangeBuffer```中的```changeImgBuffer```
```java
public class ChangeBuffer {
    static {
        System.loadLibrary("native-lib");
    }
    public native static int mapInit();
    //删除此方法
    public native static byte[] changeImgBuffer(byte[] buffer,int w,int h);
    public native static String changeImgBufferNative(byte[] buffer,int w,int h);
}
```
