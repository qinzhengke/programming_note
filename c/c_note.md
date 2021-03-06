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
如果这个symbol在源文件里：
- 看看这个源文件有没有被加到工程中。
- symbol在源文件中，并且已经添加到了工程，还会报这个错？看看是不是.c文件和.cpp文件互相调用了，如果互相调用，记得用EXTERN C封装。
- 都是cpp文件或者c文件，或者已经用EXTERN C封装了，还会出错？看看是不是不同源文件字符编码的问题。
- 以上条件都满足，但在qtcreator中刚添加的函数还是报这个错？那就试着显式地qmake一下，然后再rebuild。
- 如果还有问题，那我也不知道怎么办了，就把电脑砸了吧。

如果这个symbol在库文件中：
- 看看这个库有没有添加到工程。
- 库已经添加到工程，但是仍然报这个错？那就将库的版本和当前编译器版本保持一致，包括平台（x86或者x64）和版本（vc08，vc10，vc12，vc14）以及是否调试（debug和release）。

### 工程内外的头文件区别？
C语言头文件不管是放在工程内部还是工程外部都可以include，那么这两种方式有什么区别呢？
目前发现的区别有一点，就是工程内部的头文件内部再include别的头文件的时候可以享用工程文件已经添加的路径。
举个例子，a.h是工程部内的，b.h是工程外部的，main.cpp里面把这两个头文件都include了，工程pro（qtcreator）文件里包含了opencv的库目录。
那么打开a.h时输入“include <opencv.....”，此时creator会自动补全，但是在b.h中输入则没有任何反应，这就是目前发现的一个区别。
但是我估计实际编译的时候应该没有问题，这只是IDE的识别问题而已。

### core.hpp header must be compiled as c++
在c文件中include Opencv的hpp头文件就会出现这个问题，因为编译c文件时是使用C编译器编译的，而.c文件是不认识.hpp文件的，实现的方式就是c编译器并未定义__cplusplus宏。按照道理说c文件不应该包含h文件，最好把c文件改成cpp文件。如果硬是不改，在VisualStudio中可以设置compile as c++。在别的环境就不知道怎么搞了。

### 浮点转定点中的四舍五入
浮点数转定点数不能直接截取，必须四舍五入，否则精度会丢失，有时候会丢失很多！
例如下面的代码，将浮点数转换成16Q8的定点数，当输入0.035时，0.035\*256.0=8.96，直接截取就变成了8，然而8对应着0.03125，9对应着0.03515625，很明显应该是9更合适。
我们自然而然地想到+0.5，例如第二行代码所示，但是这样仍然是有问题的，就是在使用负数的时候。
例如输入-0.6，那么照理说-0.6更接近-1，然而-0.6+0.5=-0.1会截取成0，没有错(int)(-0.1)=0，在VS下是这样的。

```cpp
#define Q168_FROM_FLOAT(x) ( (int16_t)((x)*256.0) ) // 丢失精度
#define Q168_FROM_FLOAT(x) ( (int16_t)((x)*256.0+0.5) ) // 负数不准
#define Q168_FROM_FLOAT(x) ( (int16_t)((x)>=0?(x)*256.0+0.5:(x)*256.0-0.5) ) //完美

// 测试
printf("%d,%d\n", Q168_FROM_FLOAT(0.35), Q168_FROM_FLOAT(-0.35))
//结果为 9和-9，正确！
```

### printf 中的uint64整数
使用printf过程中，一定要写对%字符串，之前遇到过一个问题，使用%d来打印一个int64的数是错误的，例如下列代码

```cpp
uint64_t a = 1000;
printf("%d\n",a); // 错误
```

实际上，%u，%ld，%lu都不对，在曾经一个ARM平台编译器上，正确的结果是%llu，因为在该平台上，sizeof(long)=sizeof(int)=4，而uint64_t真正的类型是long long，在不同平台时，要注意类型字节数可能不一样。


### 多周期算法
两种结构：
1. 启动计算独立
2. 启动计算不独立

### linux-arm-gcc 中的char默认是unsigned char!!!
好可怕，给char类型变量赋值一个负数，会得到错误的结果，而且编译器不会报错或者警告
只有使用==判别时才提出警告。

```cp
char a = -1;
printf("%d\n",a );  //打印出 255
if(a == -10)
  printf("error") // 编译报错
```

### C语言算法中的数学参数放在哪里？
程序运行过程中始终不变的数学常量称为参数，做算法的代码会有特别多的参数，要怎么放置这些参数呢？
1. 直接在调用的地方用数字常量表示，例如
```cpp
x = 0.01 *y;
```
这种方式被称为magic number, 不太好。
第一，不好理解，如果只有一个number，还勉强可以注释，如果一行代码有多个number，就真的不好注释了。
第二，如果这个参数在很多地方用到，参数变动的时候，需要多处修改，非常容易有漏网之鱼。

2.直接用宏来define数字，在调用的地方使用宏。
```cpp
#define SLAM_PARA_XXXX 0.01
```
这种方式比直接用数字要好，define单个数字是没问题的，但是在定义数组型参数的时候就不太方便了，例如
```cpp
float para_xxx[2][3] = {{1.0000, 2.0000, 3.0000},
                        {4.0000, 5.0000, 6.0000}};
```
一个很勉强的方式就是把宏定义成花括号里的内容，然后就在调用的地方新建一个变量=宏，但是这样很怪。。。
而且如果这个参数表很大的话，例如10x10的静态查找表，每次调用的地方新建变量并赋值一次会有很多额外消耗。

3. 用全局变量直接定义
这种方式算是一种比较整洁的解决方案，但是全局变量用多了之后

### 一个float不够用的例子
x = {3137995, 3137895, 3137978}
A = {{x0\*x0, x0, 1},{x1\*x1, x1, 1},{x2\*x2, x2, 1}}

### 头文件重复包含
我们都知道头文件一开始两行都是#ifndef和#define，是为了避免头文件重复包含，从逻辑上和减少编译时间看，都需要消除这种重复包含，
那么消除重复包含的工作为什么不丢给程序员做呢？
1. 这样的工作非常枯燥而繁琐，包含的链路太长的话，光是找到公共头文件节点就很麻烦；
2.更重要的是，部分模块常常需要独立运行，这种冗余保证了模块也能够独立地运行。

### extern 声明
默认的全局变量都开放成全局可用变量，其他模块想用该变量只需要extern声明即可。
例如a.c文件中定义int a = 0; b.c文件中声明extern int a;
那么b文件中使用的变量其实就是a文件中的变量
**a.c文件和b.c文件不需要什么相互依赖的关系，也和头文件无关**。
但是一般来说，在a.h中声明extern int a;然后b.c文件再include a.h文件，这样的用法会比较规范。



### 全局变量用处
1.静态表格常量
2.调试系统

调试系统是反封装的，调试系统往往需要获取某些很深的函数内部的某些中间变量，如果通过传参获取，
（1）那么函数参数就会很多，
（2）调试系统需要查看的变量集可能会经常变化，层层递归修改函数形参

### \_Bool变量
将在线代码分离成离线代码的时候，发现有些文件里面定义了_Bool类型，而gcc编译器居然不识别。
Google后发现，_Bool类型是C语言的布尔类型，从C99才开始引入的。
C99以前的C语言都是用int或者枚举的野路子来表示布尔型。。。
我们平时用的bool都是C++的东西。。。
也就是说如果在 extern "C"中间使用bool，是不被支持的，除非引用了头文件stdbool.h


### 将指针赋值给uint32_t
很多32位平台的程序喜欢使将指针赋值给整数，这样就不需要二级指针，例如

```cpp
int func(uint32_t *addr)
{
  void * data = malloc (1000);
  *addr = (uint32_t) data;
}
```
如果需要在函数里面开内存，输出内存地址，那么要么使用二级指针，要么用这种转地址的方式。
但是，这种方式其实并不好，在64位系统中，指针的大小可能是64位的，这样赋值就会出错。
仍然像想用这种方式的话，使用intptr_t类型，兼容32和64位，在C99中支持，在<stdint.h>中定义。

### designated initializer
C语言中结构体在C99中的一种新的初始化语法，叫做designated initializer，即指定初始化，之前我一直找不到正式的名称，我一直称之为点变量初始化。。。

```cpp
MY_TYPE a = { .flag = true, .value = 123, .stuff = 0.456 };
```
这个东西与编译器无关，是C99的标准，gcc想用的话必须开启选项 -std=c99

如果使用qtcreator构建工程，那么默认使用g++编译，然而如果designated initializer语句是在cpp文件里，g++是不支持的，不管CXX_FLAG 是不是加了c++11，或者C_FLAG 加了99，或者加入extern "C"，都不行， extern "C"只是指示c++编译器把函数编成C语言可识别的符号表，并不支持这种赋值语法。

但是我发现了一种方法，就是强行改cpp后缀为c，这样g++就会调用gcc来编译c文件。

###代码区分不同操作系统
```cpp
#ifdef __linux__ 
    //linux code goes here
#elif _WIN32
    // windows code goes here
#else
    cout<<"OS not supported!"<<endl;
#endif
```

### utf-8文件的bom字节
很蛋疼，读utf-8文件前面3个字节都不是文件中的内容，而是0xef,0xbb,0xbf，后来一查，发现这是utf-8文件的bom字节，就是byte order mark,
读utf-8文件时，一定要去掉前面三个字节。

### utf-8文件中的行结尾'\r'
很蛋疼+1，读utf-8文件的时候，行结尾都是以'\r'结束，
当文件内容是另一个文件的路径的时候，例如
```
/mnt/cc/随便
```
其实这一行读出来是这样的
```
'/','m',','t','/','c','c','/','随,'便',\r
```
我一般喜欢在文件路径后面加上'/'，方便后面读取。
但是如果在\r后面加上'/'就会搞错。。

### qtcreator中开启gcc的c99支持
```
QMAKE_CFLAGS += -std=c99
```

### printf 如何输出红色字符串？
```cpp
#include<stdarg.h>
...
void printf_red(const char *cmd, ...)  
{  
    printf("\x1B[31m");  //设置输出为红色
    va_list args;       //定义一个va_list类型的变量，用来储存单个参数  
    va_start(args,cmd); //使args指向可变参数的第一个参数  
    vprintf(cmd,args);  //必须用vprintf等带V的  
    va_end(args);       //结束可变参数的获取  
    printf("\x1B[0m")   // 回到默认设置
}  
```


### 规范打印

```c
#indef PRINT_ERROR
#define PRINT_ERROR(...)    \
  do{                       \
    printf("\x1B[41;37m");  \
    printf("[ERROR]");      \
    printf("\x1B[0m");      \
    printf(__VA_ARGS__);    \
    }                       \
  while(0)
#endif
  
#ifndef PRINT_INFO
#define PRINT_INFO(...)     \
  do{                       \
    printf("\1xB[42;37m");  \
    printf("[INFO] ");      \
    printf(__VA_ARGS__);    \
    }                       \
  while(0)
#endif
    

```

### 重复定义
如果发现某个变量明明只有一处，但是编译器老是提出重复定义，就要看看头文件是否没有写#ifndef #define #endif三连。

### .o文件里发现重复定义
很有可能的原因是：
在头文件中定义了变量和函数，这会导致头文件被多个c文件引用，h文件里的变量和函数就会出现在多个.o文件中，最后链接的时候出现错误。
