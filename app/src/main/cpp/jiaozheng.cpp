#if 1

#include <jni.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <android/log.h>
#include "jiaozheng.hpp"

#define LOG_TAG "img-proc"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,__VA_ARGS__)



#define MAX_THREAD_NS  4


using namespace std;
#include <math.h> 
#define BYTE unsigned char

#endif




typedef struct sPoint
{
    unsigned short  x;
	unsigned short  y;
	unsigned char   r1;
	unsigned char   r2;
	unsigned char   r3;
	unsigned char   r4;
}sPoint;



static struct sPoint PointArray[OUTPUT_PIC_HEIGHT][OUTPUT_PIC_WIDTH];

static void CalMapTable(void)
{
	int x, y;
	float u, v;

	float R = 720;//  img_R; 
	float Pi = 3.1415926;
	float x0 = 720;// center_x; // 圆心坐标
	float y0 = 720;// center_y;// 圆心坐标
	float a2333 = 0.00136;//0.001320;//0.00154;// Prm_a2333; 
	float a2233 = 0.556000;//0.53;// Prm_a2233; 
	//float a1133 = 1.0;// Prm_a1133;
	//float  a1333 = 0.0;// Prm_a1333;
	int H = 0;  // LeftRightOffset; 
	int V = -410;//-370; // UpDownOffset;  
	float  fbR = 0.455;//0.50900;//0.42;//  fR;

	float  k1 = 0.87;// k1_user; //0.87;  //
	float  k2 = 0.12; // k2_user; // 0.123; //
	float  k3 = 0.054; // k3_user; // 0.057; //
	float  k4 = -0.034;// k4_user; //-0.038;//




	for (y = 0; y < OUTPUT_PIC_HEIGHT; y++)
	{
		for (x = 0; x < OUTPUT_PIC_WIDTH; x++)
		{
			////////////////////坐标平移////////////////////
			//float x2 = x - OUTPUT_PIC_WIDTH / 2 + H;
			//float y2 = OUTPUT_PIC_HEIGHT  - y +  V;

			////翻转
			float x2 =  OUTPUT_PIC_WIDTH / 2 - x + H;
			float y2 = y +  V;

			//////////////////透视变换/////////////////////////
           // u = (a11 * x + a21 * y + a31) / (a13*x + a23 * y + a33);
           // v = (a12 * x + a22 * y + a32) / (a13*x + a23 * y + a33);

		    //(x=0,y=0) => (u=0,v=0)
			//a31 = 0;
			//a32 = 0;
		   // u = (a11 * x + a21 * y ) / (a13*x + a23 * y + a33);
		   // v = (a12 * x + a22 * y ) / (a13*x + a23 * y + a33);

			//y=0 => u=x 
			//a13 = 0;
			//a33 = a11;
		   // u = (a33 * x + a21 * y ) / (a23 * y  + a33);
		   // v = (a12 * x + a22 * y) / (a23 * y  + a33);

			//y=0 => v=0;
			//a12 = 0;
		   // u = (a33 * x + a21 * y ) / (a23 * y  + a33);
		   // v = ( a22 * y) / (a23 * y  + a33);

			//x=0 => u=0;
			//a21 = 0;
		   // u = ( a33 * x ) / (a23 * y  + a33);
		   // v = ( a22 * y ) / (a23 * y  + a33);

			//a33 = 1;
		   // u = ( x ) / (a2333 * y  + 1);
		   // v = (a2233 * y) / (a2333 * y  + 1);

			float x3 = ( x2 ) / ( a2333 * y2  + 1);
			float y3 = (a2233 * y2) / ( a2333 * y2 + 1);

			//////////////////////鱼眼投影////////////////////////
			if (x3 == 0)
			{
				//v = f * atan(y2 / 0.3 / R);
				float r2 = sqrt(x3 * x3 + y3 * y3);

				//非等距投影矫正
				float ss = atan(r2 / (R * fbR) );
				ss = k1 * ss + k2 * pow(ss,3) + k3 * pow(ss, 5) + k4 * pow(ss, 7);  //非等距投影矫正
				float r0 = R * 2  * ss / Pi;
				u = 0;
				v = r0;

				if (y3 < 0) v = -v;
			}
			else
			{
				float vbu = y3 / x3;
				float r2 = sqrt(x3 * x3 + y3 * y3);

				//非等距投影矫正
				float ss = atan(r2 / ( R * fbR) );
				ss = k1 * ss + k2 * pow(ss, 3) + k3 * pow(ss, 5) + k4 * pow(ss, 7); //非等距投影矫正

				float r0 = R * 2 * ss / Pi;
				u = r0 / sqrt(1 + vbu * vbu);
				v = vbu * u;

				///切向畸变矫正
				//float p1 = 0, p2 = 0 ;
				//float x4 = u;
				//float y4 = v;
				//u = x4 +(2 * p1 * y4 + p2 * (r0 * r0 + 2 * x4 * x4));
				//v = y4 +(2 * p2 * x4 + p1 * (r0 * r0 + 2 * y4 * y4));
				//////////////////////

				if (v < 0)  v = -v;
				if (x3 < 0) u = -u;
				if (y3 < 0) v = -v;
			}
			////////////////////////////

			
			////////////变换到原图////////////
			int uu = (int)(u + x0);
			int vv = (int)(y0 - v - 1) - SOURCE_PIC_CUT_V_OFFSET;

			////////////////////计算小数位///////////////////////////////////
			float dx = (u + x0) - (float)uu;
			float dy = (y0 - v - 1) - (float)vv;

			/////////////////////预防越过边界///////////////////////
			if (uu < 0)
			{
				uu = 0;
				dx = 0;
			}
			else if (uu > SOURCE_PIC_CUT_WIDTH - 2)
			{
				uu = SOURCE_PIC_CUT_WIDTH - 2;
				dx = 0;
			}

			if (vv < 0)
			{
				vv = 0;
				dy = 0;
			}
			else if (vv > SOURCE_PIC_CUT_HEIGHT - 2)
			{
				vv = SOURCE_PIC_CUT_HEIGHT - 2;
				dy = 0;
			}
			////////////////////////////////////////////////////

			///////////////////计算加权指数//////////////////////
			float r1 ;
			float r2 ;
			float r3 ;
			float r4 ;

			if (dx >= 0.66)
			{
				uu++;
				dx = 0;
			}
			else if (dx <= 0.33)
			{
				dx = 0;
			}
			if (dy >= 0.66)
			{
				vv++;
				dy = 0;
			}
			else if (dy <= 0.33)
			{
				dy = 0;
			}

			if (dx == 0 && dy == 0)
			{
				r1 = 255;
				r2 = 0;
				r3 = 0;
				r4 = 0;
			}
			else if (dx == 0)
			{
				r1 = round((1 - dy) * 255.0);
				r2 = 0;
				r3 = 0;
				r4 = round(dy * 255.0);
			}
			else if (dy == 0)
			{
				r1 = round((1 - dx) * 255.0);
				r2 = round(dx * 255.0);
				r3 = 0;
				r4 = 0;
			}
			else
			{
				r1 = 64;
				r2 = 64;
				r3 = 64;
				r4 = 64;
			}

			PointArray[y][x].r1 = r1;
			PointArray[y][x].r2 = r2;
			PointArray[y][x].r3 = r3;
			PointArray[y][x].r4 = r4;

			PointArray[y][x].x = uu;
			PointArray[y][x].y = vv;
#if 0
			/////////////////////预防越过边界///////////////////////
            if(uu < 0) uu = 0;
			if(uu > SOURCE_PIC_WIDTH - 2) uu = SOURCE_PIC_WIDTH - 2;
			if(vv < 0) vv = 0;
			if(vv > SOURCE_PIC_HEIGHT -2) vv = SOURCE_PIC_HEIGHT -2;

			//if (uu >= 0 && uu < SOURCE_PIC_WIDTH - 1 && vv >= 0 && vv < SOURCE_PIC_HEIGHT -1)
			{
#if 0
				PointArray[y][x].x = uu;
				PointArray[y][x].y = vv;

				float dx = u + x0 - (float)uu;
				float dy = y0 - v - (float)1 - (float)vv;
				
				float r1 = 1/(dx * dx + dy * dy);
				float r2 = 1/((1-dx) *(1- dx) + dy * dy);
				float r3 = 1/((1 - dx) *(1 - dx) + (1-dy) *(1- dy));
				float r4 = 1/(dx * dx + (1-dy) * (1-dy));

				float sum = r1 + r2 + r3 + r4;

				PointArray[y][x].r1 = (r1 * 255 / sum);
				PointArray[y][x].r2 = (r2 * 255 / sum);
				PointArray[y][x].r3 = (r3 * 255 / sum);
				PointArray[y][x].r4 = (r4 * 255 / sum);
#else
                PointArray[y][x].x = uu;
                PointArray[y][x].y = round(y0 - v - 1);
                
                float dx = u + x0 - (float)uu;

                PointArray[y][x].r1 = ((1-dx) * 255 );
                PointArray[y][x].r2 = ((dx) * 255 );
 

#endif
			}

#endif

		}
	}
	 

}

#if 0
void vTransImgAndNV21ToYV12(unsigned char *pSourceY, unsigned short *pSourceUV, unsigned char *pDesY, unsigned char *pDesU, unsigned char *pDesV, int InputLineSize )
{
	int x, y, mY, mUV, nY, nUV;
	int u, v;
	for (y = 0; y < OUTPUT_PIC_HEIGHT; y++)
	{
		mY = OUTPUT_PIC_WIDTH * y;
		mUV = OUTPUT_PIC_WIDTH / 4 * y;
		for (x = 0; x < OUTPUT_PIC_WIDTH; x++)
		{
			u = PointArray[y][x].x;
			v = PointArray[y][x].y;

			nY = v * InputLineSize;
			pDesY[mY + x] = (int)((int)pSourceY[nY + u] * PointArray[y][x].r1 + (int)pSourceY[nY + u + 1] * PointArray[y][x].r2 + (int)pSourceY[nY + u + 1 + InputLineSize] * PointArray[y][x].r3 + (int)pSourceY[nY + u + InputLineSize] * PointArray[y][x].r4) >> 8;

			if (y % 2 == 0 && x % 2 == 0)
			{
				pDesU[mUV + x / 2] = pSourceUV[v / 2 * InputLineSize / 2 + u / 2]>>8;
				pDesV[mUV + x / 2] = pSourceUV[v / 2 * InputLineSize / 2 + u / 2]&0xFF;
			}
		}
	}

}
#endif

static void vTransImgAndNV21ToYV12(unsigned char *pSrcY, unsigned char *pSrcUV, unsigned char *pDesY, unsigned char *pDesU, unsigned char *pDesV, int InputLineSize , int startY, int endY)
{
	register unsigned int x, y, mY;
	register unsigned int u, v;
	register unsigned int halfLineSize = (InputLineSize >> 1);
   // LOGD("vTransImgAndNV21ToYV12() ");
	mY = startY * OUTPUT_PIC_WIDTH;

	for (y = startY; y < endY; y++, mY += OUTPUT_PIC_WIDTH )
	{
		register unsigned int mUV = ( (y>>1) * (OUTPUT_PIC_WIDTH/2) );

		for (x = 0; x < OUTPUT_PIC_WIDTH; x++)
		{
			u = PointArray[y][x].x;
			v = PointArray[y][x].y;

			register unsigned int pos = v * InputLineSize + u;

			//pDesY[mY + x] = (int)((int)pSrcY[pos] * PointArray[y][x].r1 + (int)pSrcY[pos + 1] * PointArray[y][x].r2 )>>8;

			pDesY[mY + x] = (int)((int)pSrcY[pos] * PointArray[y][x].r1 + (int)pSrcY[pos + 1] * PointArray[y][x].r2 + (int)pSrcY[pos + 1 + InputLineSize] * PointArray[y][x].r3 + (int)pSrcY[pos + InputLineSize] * PointArray[y][x].r4) >> 8;
			if ((y & 0x01) == 0 && (x & 0x01) == 0)
			{
				register unsigned int nUV = ( (v>>1) * halfLineSize + (u>>1) )<<1;
				register unsigned int posUV = mUV + (x >> 1);

				pDesU[ posUV ] = pSrcUV[ nUV + 1 ];
				pDesV[ posUV ] = pSrcUV[ nUV ];
			}

		}
	}

  //  LOGD("vTransImgAndNV21ToYV12() finish");
}



typedef struct mapPrm
{
    unsigned char *pSrcY;
	unsigned char *pSrcUV;
    unsigned char *pDesY;
	unsigned char *pDesU;
	unsigned char *pDesV;
    int partNum;
}mapPrm;


static void *MapImgYProc(void *prm)
{
    struct mapPrm *pPrm = (struct mapPrm *)prm;
    int  partNum =pPrm->partNum;
	int  startY  = OUTPUT_PIC_HEIGHT * (partNum) / MAX_THREAD_NS;
    int  endY    = OUTPUT_PIC_HEIGHT * (partNum + 1) / MAX_THREAD_NS;
	

    vTransImgAndNV21ToYV12(pPrm->pSrcY, pPrm->pSrcUV, pPrm->pDesY, pPrm->pDesU, pPrm->pDesV,  SOURCE_PIC_CUT_WIDTH ,  startY,  endY);

    return NULL;
}


QMX_STATIC void  MapImg(unsigned char *pSrcY, unsigned char *pSrcUV, unsigned char *pDesY, unsigned char *pDesU, unsigned char *pDesV )
{
    pthread_t thread[MAX_THREAD_NS];
    struct mapPrm  PrmVal[MAX_THREAD_NS];
   
	//LOGD("MapImgMultiThread()");
    //time_t clockcount = GetTimeMS();

    for(int i=0;i<MAX_THREAD_NS;i++)
    {
        
        PrmVal[i].pSrcY = pSrcY;
		PrmVal[i].pSrcUV = pSrcUV;
		PrmVal[i].pDesY = pDesY;
		PrmVal[i].pDesU = pDesU;
		PrmVal[i].pDesV = pDesV;
        PrmVal[i].partNum = i;
        pthread_create(&(thread[i]), NULL, MapImgYProc, &(PrmVal[i]));
    }

    for(int i=0;i<MAX_THREAD_NS;i++)
    {
        pthread_join(thread[i], 0);
    }

	//clockcount = GetTimeMS() -	clockcount;
	//LOGD("MapImgMultiThread()  finish, %ldms",clockcount);
	//35mS

}


static void resizeImgChannel_1_4(unsigned char *pImageInput, unsigned char *pImageOutput, int width, int height)
{
	int offset[16];
	int linesize = width; //pImageInput->linesize;
	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 4; i++)
		{
			offset[j * 4 + i] = i + j * linesize;
		}
	}


	int outputWidth = width / 4;
	int outputHeight = height / 4;

	for (int j = 0; j < outputHeight; j++)
	{
		for (int i = 0; i < outputWidth; i++)
		{
			int oldOffset = j * 4 * linesize + i * 4;
			int avg = 0;

			for (int n = 0; n < 16; n++)
			{
				avg += (int)pImageInput[oldOffset + offset[n]];
			}
			avg >>= 4;
			pImageOutput[j*outputWidth + i] = (unsigned char)avg;
		}
	}


}

QMX_STATIC void resizeImgChannel_1_2(unsigned char *pImageInput, unsigned char *pImageOutput, int width, int height)
{
	int offset[4];
	int linesize = width; //pImageInput->linesize;
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 2; i++)
		{
			offset[j * 2 + i] = i + j * linesize;
		}
	}

	int outputWidth = width / 2;
	int outputHeight = height / 2;

	for (int j = 0; j < outputHeight; j++)
	{
		for (int i = 0; i < outputWidth; i++)
		{
			int oldOffset = j * 2 * linesize + i * 2;
			int avg = 0;

			for (int n = 0; n < 4; n++)
			{
				avg += (int)pImageInput[oldOffset + offset[n]];
			}
			avg >>= 2;
			pImageOutput[j*outputWidth + i] = (unsigned char)avg;
		}
	}

}




typedef struct resizePrm
{
    unsigned char *pSrc; 
    unsigned char *pDes; 
	int srcWidth;
	int srcHeight;
}resizePrm;


static void *resizeImg1_4Proc(void *prm)
{
    struct resizePrm *pPrm = (struct resizePrm *)prm;

    resizeImgChannel_1_4(pPrm->pSrc, pPrm->pDes, pPrm->srcWidth, pPrm->srcHeight);

    return NULL;
}

static void *resizeImg1_2Proc(void *prm)
{
    struct resizePrm *pPrm = (struct resizePrm *)prm;

    resizeImgChannel_1_2(pPrm->pSrc, pPrm->pDes, pPrm->srcWidth, pPrm->srcHeight);

    return NULL;
}


QMX_STATIC void  resizeImgTo1_4(unsigned char *pSrcY, unsigned char *pSrcU, unsigned char *pSrcV, int width, int height, unsigned char *pDesY, unsigned char *pDesU, unsigned char *pDesV)
{
    pthread_t      thread[3];
    struct resizePrm  PrmVal[3];
   
   // time_t clockcount = GetTimeMS();

    PrmVal[0].pSrc = pSrcY;
	PrmVal[0].pDes = pDesY;
    PrmVal[0].srcWidth = width;
	PrmVal[0].srcHeight = height;
    pthread_create(&(thread[0]), NULL, resizeImg1_4Proc, &(PrmVal[0]));
	
    PrmVal[1].pSrc = pSrcU;
	PrmVal[1].pDes = pDesU;
    PrmVal[1].srcWidth = width/2;
	PrmVal[1].srcHeight = height/2;
    pthread_create(&(thread[1]), NULL, resizeImg1_2Proc, &(PrmVal[1]));

    PrmVal[2].pSrc = pSrcV;
	PrmVal[2].pDes = pDesV;
    PrmVal[2].srcWidth = width/2;
	PrmVal[2].srcHeight = height/2;
    pthread_create(&(thread[2]), NULL, resizeImg1_2Proc, &(PrmVal[2]));
	
    pthread_join(thread[0], 0);
	pthread_join(thread[1], 0);
	pthread_join(thread[2], 0);

	//clockcount = GetTimeMS() -	clockcount;

	//LOGD("resizeImgTo1_4()  finish, %ldms",clockcount);
    //25mS
}


static int MapHasInit = 0;
QMX_STATIC int   MapInit(void)
{
   if(MapHasInit)
   {
      return 0;
   }
  
   CalMapTable();
   MapHasInit = 1;
   return 0;
}

