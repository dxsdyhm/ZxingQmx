#ifndef __JIAO_ZHENG__
#define __JIAO_ZHENG__




#define   SOURCE_PIC_WIDTH  1440
#define   SOURCE_PIC_HEIGHT 1440

#define   SOURCE_PIC_CUT_WIDTH    1440
#define   SOURCE_PIC_CUT_V_OFFSET 544
#define   SOURCE_PIC_CUT_HEIGHT   744


#define  OUTPUT_PIC_WIDTH  1280
#define  OUTPUT_PIC_HEIGHT  720

#define QMX_STATIC

QMX_STATIC int   MapInit(void);

//input image must be: SOURCE_PIC_CUT_WIDTH,SOURCE_PIC_CUT_HEIGHT
//
QMX_STATIC void  MapImg(unsigned char *pSrcY, unsigned char *pSrcUV, unsigned char *pDesY, unsigned char *pDesU, unsigned char *pDesV );
QMX_STATIC void  resizeImgTo1_4(unsigned char *pSrcY, unsigned char *pSrcU, unsigned char *pSrcV, int width, int height, unsigned char *pDesY, unsigned char *pDesU, unsigned char *pDesV);
QMX_STATIC void  resizeImgChannel_1_2(unsigned char *pImageInput, unsigned char *pImageOutput, int width, int height);

#endif

