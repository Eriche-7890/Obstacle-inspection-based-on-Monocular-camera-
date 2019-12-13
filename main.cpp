#include <iostream>
#include <opencv2/highgui/highgui.hpp>  //highgui模块头文件
#include <opencv2/imgproc/imgproc.hpp>  //OpenCV 图像处理头文件
using namespace cv;   //包含cv命名空间
using namespace std;
void MedianFiltering(int* _Psrc, int* _Pdest, int _index, int _size);
int Least_Square_Method(int *srcData, int head, int index);
void chao_fillHole(const Mat srcimage, Mat &dstimage);




int main()
{
   Mat dst;
   Mat src=imread("/home/qie/zhuitong/1.jpg");
   resize(src,src,Size(src.cols/5,src.rows/5));
   imshow( "src",src);  //显示图片
   waitKey(0);
   imwrite("0.jpg",src);
  dst = Mat::zeros(src.rows, src.cols, CV_8U);
  Mat srcc;
     cvtColor(src, srcc, COLOR_BGR2HSV);  // RGB --> HSV
    Vec3i HSV(0, 0, 0);     // used to store HSV vector

     for (int i = srcc.rows - 1; i > 0; i--)
       {
           for (int j = 0; j < srcc.cols-1; j++)
           {
               HSV = srcc.at<Vec3b>(i, j);
               // Blue in HSV space is within a range
               if ((HSV.val[0] >= 70 && HSV.val[0] <= 120))
               {
                 if (HSV.val[2] > 250 && ((HSV.val[1]<110 && HSV.val[0]<=92) || (HSV.val[1] > 110 && HSV.val[0] > 92) ))    // 两大两小：反光
                       dst.at<uchar>(i, j) = 255;
                   else if((HSV.val[1] >= 80 && HSV.val[1] <= 255) &&(HSV.val[2] >= 100 && HSV.val[2] <= 255))
                       dst.at<uchar>(i, j) = 255;

               }
             }
        }

   // imshow("1",dst);
   // waitKey(0);
    chao_fillHole(dst,dst);
    imshow("tian",dst);
    waitKey(0);
    int EleSize=12;
    Mat ele = getStructuringElement(MORPH_ELLIPSE, Size(EleSize, EleSize));    // 设置形态学运算内核
    morphologyEx(dst, dst, MORPH_CLOSE, ele);         // 闭运算
    imshow("close",dst);
  imwrite("close.jpg",dst);
    waitKey(0);
    ele = getStructuringElement(MORPH_ELLIPSE, Size(EleSize,EleSize));    // 设置形态学运算内核
    morphologyEx(dst, dst, MORPH_OPEN, ele);          // 开运算
    imshow("open",dst);
    imwrite("open.jpg",dst);
    waitKey(0);
    Mat dstimg=dst.clone();
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
      findContours(dst, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);   // 轮廓查找
       Mat drawing = Mat::zeros(dstimg.size(), CV_8UC3);
       for (unsigned long i = 0;i< contours.size(); i++)          // contours.size()得到contours的数组长度( 轮廓数量 )
       {

      Scalar color = Scalar(rand() , rand(), rand() );//任意值
      drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
       }
       imshow("",drawing);
       imwrite("findcontours.jpg",drawing);
       waitKey(0);

       int ImageHigh = dst.rows;     // get the image high information
       int ImageWidth = dst.cols;    // get the image width information
       int CountColDest[1500] = { 0 };
       int CountCol[1500] = { 0 };       // used to record the number of red pixels per column after processing ( 垂直投影 )
       int coor, max = 0;
       int i = 0, j = 0;
       for (i=0; i < ImageHigh; i++)
       {
           for (j = 0; j < ImageWidth; j++)
           {
               if (dst.at<uchar>(i, j) != 0)
               {
                   //CountRow[i]++;
                   CountCol[j]++;
               }
           }
       }
       for (i = 0; i < ImageWidth; i++)
       {
           if (max < CountCol[i])
           {
               max = CountCol[i];
               coor = i;
           }
       }
       MedianFiltering(CountCol, CountColDest, ImageWidth, 7);
       vector<int> ercheng;
       for (i = 0; i < ImageWidth; i++)
           ercheng.push_back(Least_Square_Method(CountColDest, i, 4));
       for (i = 0; i < coor-4; i++)
           if (ercheng[i] < -10)
               return 0;
       for (i = coor + 6; i < ercheng.size();i++)
           if (ercheng[i] > 10)



       return 0;





}
int Least_Square_Method(int *srcData, int head, int index)
{
    int sumxy = 0;
    int sumx = 0;
    int sumy = 0;
    int sumxx = 0;
    int Numerator = 0;
    int Denominator = 0;
    if (head < index)
        return 0;
    for (int i = 0;i < index;i++)
    {
        sumxy = sumxy + (i*srcData[head-index+i]);
        sumx = sumx + i;
        sumy = sumy + srcData[head-index+i];
        sumxx = sumxx + (i*i);
    }
    Numerator = (index*sumxy - sumx * sumy);
    Denominator = (index*sumxx - sumx * sumx);
    if (Denominator == 0)
        Denominator = 1;
    int dest = 10*Numerator / Denominator;
    return dest;
}

void MedianFiltering(int* _Psrc, int* _Pdest, int _index, int _size)
{
    int location = _size / 2;   // get the location of beginning
    int ArrTemp[100] = { 0 };
    int NumTemp = 0;
    int k = 0;
    for (int i = location; i < _index - location; i++)    // sliding window
    {   // InsertSort
        ArrTemp[0] = _Psrc[i - location];
        for (int j = 1; j < _size; j++)
        {
            NumTemp = _Psrc[i - location + j];  // get next data
            k = j;
            while (k > 0 && NumTemp < ArrTemp[k - 1])  // core realization
            {
                ArrTemp[k] = ArrTemp[k - 1];
                k--;
            }
            //
            ArrTemp[k] = NumTemp;
        }
        _Pdest[i] = ArrTemp[location];  // get the middle element
    }
}

void chao_fillHole(const Mat srcimage, Mat &dstimage)
{
    Size m_Size = srcimage.size();
    Mat temimage = Mat::zeros(m_Size.height + 2, m_Size.width + 2, srcimage.type());//延展图像
    srcimage.copyTo(temimage(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));
    floodFill(temimage, Point(0, 0), Scalar(255));
    Mat cutImg;//裁剪延展的图像
    temimage(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);
    dstimage = srcimage | (~cutImg);
}
