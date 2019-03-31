#include <jni.h>
#include <string>
#include <android/log.h>
#include "jiaozheng.hpp"

#include <iostream>
#include "zxing/core/src/zxing/common/Counted.h"
#include "zxing/core/src/zxing/common/Array.h"
#include "zxing/core/src/zxing/Exception.h"
#include "zxing/core/src/zxing/LuminanceSource.h"
#include "zxing/core/src/zxing/Binarizer.h"
#include "zxing/core/src/zxing/BinaryBitmap.h"
#include "zxing/core/src/zxing/common/GlobalHistogramBinarizer.h"
#include "zxing/core/src/zxing/DecodeHints.h"
#include "zxing/core/src/zxing/Result.h"
#include "zxing/core/src/zxing/qrcode/QRCodeReader.h"
#include "CBufferReaderSource.h"

using namespace std;
using namespace zxing;
using namespace qrcode;
#define LOG_TAG "img-proc"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,__VA_ARGS__)

extern "C" JNIEXPORT jint
JNICALL
Java_com_trans_ChangeBuffer_mapInit(JNIEnv *env, jobject thi) {
    return MapInit();
}

extern "C" JNIEXPORT jbyteArray
JNICALL
Java_com_trans_ChangeBuffer_changeImgBuffer(JNIEnv *env, jobject thi, jbyteArray buffer,
                                            int w, int h) {
    jbyte *data = env->GetByteArrayElements(buffer, NULL);
    int len = env->GetArrayLength(buffer);
    int outLen = 1280 * 720 * 1.5;
    const jbyte *output = static_cast<const jbyte *>(malloc(outLen));

    int ulen = len / 3;
    unsigned char *yData = static_cast<unsigned char *>(malloc(ulen * 2));
    unsigned char *uvData = static_cast<unsigned char *>(malloc(ulen));
    memcpy(yData, data, ulen * 2);
    memcpy(uvData, data + ulen * 2, ulen);

    int outputUlen = outLen / 3;

    jbyteArray array = env->NewByteArray(outLen);
    MapImg(yData, uvData, (unsigned char *) output, (unsigned char *) (output + outputUlen * 2),
           (unsigned char *) (output + outputUlen * 2 + outputUlen / 2));
    env->ReleaseByteArrayElements(buffer, data, 0);
    env->SetByteArrayRegion(array, 0, outLen, output);
    delete (output);
    return array;
}

extern "C" JNIEXPORT jstring
JNICALL
Java_com_trans_ChangeBuffer_changeImgBufferNative(JNIEnv *env,jobject thi, jbyteArray buffer, int width, int height) {
    jbyte *dataBuffer = env->GetByteArrayElements(buffer, NULL);
    try {
        // Convert the buffer to something that the library understands.
        ArrayRef<char> data((char *) dataBuffer, width * height);
        Ref<LuminanceSource> source(new CBufferReaderSource(width, height, data));
        // Turn it into a binary image.
        Ref<Binarizer> binarizer(new GlobalHistogramBinarizer(source));
        Ref<BinaryBitmap> image(new BinaryBitmap(binarizer));

        // Tell the decoder to try as hard as possible.
        DecodeHints hints(DecodeHints::DEFAULT_HINT);
        hints.setTryHarder(true);

        // Perform the decoding.
        QRCodeReader reader;
        Ref<Result> result(reader.decode(image, hints));

        // Output the result.
        cout << result->getText()->getText() << endl;
        LOGE("-------> %s",result->getText()->getText().c_str());
        env->ReleaseByteArrayElements(buffer, dataBuffer, 0);
        return env->NewStringUTF(result->getText()->getText().c_str());
    } catch (zxing::Exception &e) {
        cerr << "Error: " << e.what() << endl;
        env->ReleaseByteArrayElements(buffer, dataBuffer, 0);
        return env->NewStringUTF("");
    }
}
