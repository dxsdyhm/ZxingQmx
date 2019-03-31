//
// Created by USER on 2019/3/31.
//
#ifndef ZXINGQMX_CBUFFERREADERSOURCE_H
#define ZXINGQMX_CBUFFERREADERSOURCE_H
#include <zxing/LuminanceSource.h>
#include <stdio.h>
#include <stdlib.h>
using namespace zxing;

class CBufferReaderSource :
        public zxing::LuminanceSource
{
private:
    typedef LuminanceSource Super;
    int width, height;
    ArrayRef<char> buffer;
public:
    CBufferReaderSource(int inWidth, int inHeight, ArrayRef<char> inBuffer);
    ~CBufferReaderSource(void);
    int getWidth() const;
    int getHeight() const;
    ArrayRef<char> getRow(int y, ArrayRef<char> row) const;
    ArrayRef<char> getMatrix() const;
};
#endif //ZXINGQMX_CBUFFERREADERSOURCE_H
