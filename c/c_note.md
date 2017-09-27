[TOC]



## 结构体初始化

C99标准后，C语言有一个我没怎么见过的结构体初始化方法，就是“.+变量=目标的方式”，使用这种方式，这种方式可以在数组+结构体嵌套情况下下使用，据说很多操作系统内核用了这样的代码。
赋值的顺序可以改变的，例如a2=0.0可以在a1=0之前，有点像python。

```cpp
type struct
{
  int a1;
  float a2;
}A;

typedef struct
{
  A sa;
  int b1;
  float b2;
}B;

B bs[2] = 
{
  {
    {
      a1 = 0,
      a2 = 0.0,
    }
    b1 = 0,
    b2 = 0.0,
  },
  {
    {
      a1 = 1,
      a2 = 1.0,
    }
    b1 = 1,
    b2 = 1.0,
  }
}
```


### 下采样拷贝时间
下采样拷贝虽然点数少，但是需要多次拷贝操作，那么这种方式和全拷贝速度差异如何呢？

```cpp
uint32_t W=640, H=480, WH=W*H;
FILE *f = fopen("img","rb");
uint8_t* buf = malloc(WH);
fread(buf, WH, 1, f);

int64_t t = cv::getTickCount();
uint8_t * img = (uint8_t*) malloc(WH);
memcpy(img, buf, WH);
t = cv::getTickCount() - t;
cout<<t/cv::getTickFrenquency()<<endl;

t = cv::getTickCount();
uint32_t sd=5, Wd=W/sd, Hd=H/sd;
uint8_t *img_d = (uint8_t*)malloc(Wd*Hd);
for(uint32_t r=0; r<Hd; r++)
  for(uint32_t c=0; c<Wd; c++)
    img_d[r*Wd+c] = buf[r*sd*W+c*sd];
t = cv::getTickCount()-t;
cout<<t/cv::getTickFrequency()<<endl;
```
PC上运行的结果，t1是全拷贝的时间，t2是下采样拷贝的时间，可以看到在debug版本中，2倍采样和3倍采样时间都要长于全采样，直到4倍采样开始时间才小于全采样，这是因为下采样需要设计循环，依次赋值，全拷贝肯定在汇编层面做了优化的。
有意思的是在Release版本中，2倍采样的时间就小于全拷贝，这就说明debug版本对于循环有着更深的影响，下采样最大运行时间相差了接近6倍。

Debug版本

 sd | 1    | 2    | 3    | 4    | 5    | 6    
 -- | ---- | ---- | ---- | ---- | ---- |----
 t1 | 91.5 | 90.5 | 91.7 | 91.2 | 90.4 | 90.6 
 t2 | 840  | 219  | 102  | 60.2 | 39.1 | 27.6 

Release版本

| sd | 1    | 2    | 3    | 4    | 5    | 6    |
| -- | ---- | ---- | ---- | ---- | ---- | ---- |
| t1 | 74.4 | 72.7 | 74.3 | 74.6 | 77.3 | 75.1 |
| t2 | 158  | 62.6 | 30.5 | 18.5 | 12.3 | 8.96 |
 
ARM
| sd | 1    | 2    | 3    | 4    | 5    | 6    |
| -- | ---- | ---- | ---- | ---- | ---- | ---- |
| t1 | 644  | 654 | 652  | 645 | 670 | 685 |
| t2 | 667  | 208 | 97.6 | 28.2 | 69.0 | 48.4 |


### 图像浏览工具
做图像算法的时候经常要处理图像buffer，就是最原始指针那种，因为程序运行在嵌入式平台，所以有的时候调试起来看图不是特别方便，很多时候我们从板子上存下来的图是原始的raw格式，或者我们想传入我们自己的图像进行测试，这时候就需要某种图像转换和浏览工具。

下面是代码，其实思路很简单，使用opencv将raw格式的文件打开，然后转换成bmp，再使用bat调用图片浏览工具，实现双击就能浏览的效果。
这里的bin格式是我自己定义的，bin表示单通道，bin3表示3通道，通常是BGR。
代码中转换单通道的函数没有填写，这里只填写了3通道的。

```cpp
#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

#define PRINT_AND_RETURN(x) do{cout<<x<<endl; return 1;}while(0)
#define PRINT_USAGE_AND_RETURN() PRINT_AND_RETURN("Usage: bin_viewer.exe <input file> <output file>")

int convert_bin_to_bmp(string in_path, string out_path)
{
    return 0;
}

int convert_bin3_to_bmp(string in_path, string out_path)
{
    ifstream f;
    f.open(in_path, ios::binary);
    uint32_t W,H;
    f.read((char*)&W, sizeof(uint32_t));
    f.read((char*)&H, sizeof(uint32_t));
    Mat save(H,W, CV_8UC3);
    f.read((char*)save.data, 3*W*H);
    imwrite(out_path, save);
    return 0;
}

int main(char argc, char **argv)
{
    if(argc<3)
        PRINT_USAGE_AND_RETURN();

    string in_path = string(argv[1]);
    if(in_path.substr(in_path.size()-3, 3) == "bin")
        convert_bin_to_bmp(string(argv[1]), string(argv[2]));
    else if(in_path.substr(in_path.size()-4, 4) == "bin3")
        convert_bin3_to_bmp(string(argv[1]), string(argv[2]));
    else
        PRINT_AND_RETURN("only open bin or bin3 file!");

    cout<<"done!"<<endl;
    return 0;
}

```
接下来就是编写bat文件，调用exe、传入参数并且调用图片浏览工具。
其中%1就是传入的参数，对，没错，就是双击的文件的路径，不要怀疑，微软爸爸已经给你做好了！
给bin文件和bin3文件选择默认的打开方式，设置成bat文件，对，就是右键选择默认程序！
ImageGlass是一个网上下载的轻量级的浏览工具。

```shell
bin_viewer.exe %1 out.bmp
"C:\Program Files\ImageGlass\ImageGlass.exe" out.bmp
```

### 遇到unresolved external symbol怎么办？
- 如果这个symbol在源文件里：
- 看看这个源文件有没有被加到工程中。
- symbol在源文件中，并且已经添加到了工程，还会报这个错？看看是不是.c文件和.cpp文件互相调用了，如果互相调用，记得用EXTERN C封装。
- 都是cpp文件或者c文件，或者已经用EXTERN C封装了，还会出错？看看是不是不同源文件字符编码的问题。
- 以上条件都满足，但在qtcreator中刚添加的函数还是报这个错？那就试着显式地qmake一下，然后再rebuild。
- 如果还有问题，那我也不知道怎么办了，就把电脑砸了吧。

- 如果这个symbol在库文件中：
- 看看这个库有没有添加到工程。
- 库已经添加到工程，但是仍然报这个错？那就将库的版本和当前编译器版本保持一致，包括平台（x86或者x64）和版本（vc08，vc10，vc12，vc14）以及是否调试（debug和release）。
