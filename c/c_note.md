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
PC上运行的结果

Debug版本

| sd | 1    | 2    | 3    | 4    | 5    | 6    |
| t1 | 91.5 | 90.5 | 91.7 | 91.2 | 90.4 | 90.6 |
| t2 | 840  | 219  | 102  | 60.2 | 39.1 | 27.6 |

Release版本

| sd | 1    | 2    | 3    | 4    | 5    | 6    |
| t1 | 74.4 | 72.7 | 74.3 | 74.6 | 77.3 | 75.1 |
| t2 | 158  | 62.6 | 30.5 | 18.5 | 12.3 | 8.96 |


