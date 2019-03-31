#include "CBufferReaderSource.h"

CBufferReaderSource::CBufferReaderSource(int inWidth, int inHeight, ArrayRef<char> inBuffer)
        :Super(inWidth,inHeight),buffer(inBuffer)
{
    width = inWidth;
    height = inHeight;
    buffer = inBuffer;
}

CBufferReaderSource::~CBufferReaderSource(void)
{
}

int CBufferReaderSource::getWidth() const
{
    return width;
}

int CBufferReaderSource::getHeight() const
{
    return height;
}

ArrayRef<char> CBufferReaderSource::getRow(int y, ArrayRef<char> row) const
{
    if (y < 0 || y >= height)
    {
        fprintf(stderr, "ERROR, attempted to read row %d of a %d height image.\n", y, height);
        return NULL;
    }
    // WARNING: NO ERROR CHECKING! You will want to add some in your code.
    if (row == NULL) row = ArrayRef<char>(getWidth());
    for (int x = 0; x < width; x ++)
    {
        row[x] = buffer[y*width+x];
    }
    return row;
}

ArrayRef<char> CBufferReaderSource::getMatrix() const
{
    return buffer;
}

