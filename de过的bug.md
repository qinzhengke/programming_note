[TOC]

### debug和release的不要混用！
debug时间，2017-09-25 20:00-22：00

一个de了快**两个小时**的bug！
一段将二进制单通道图像转换成png图像的代码，文件前8个字节是W和H，接着是W\*H长度的图像数据。
在imwrite的时候一直出错，W和H读出来的是正确的，搞了半天一直没有头绪，非常的苦恼。
其中还怀疑过是不是QtCreator的问题，然后用VisualStudio试了一遍，发现VisualStudio居然可以！（VS一开始也有问题，但随即发现是自己文件路径不对）
然后仔细对比了一下，发现在VS下链接的OpenCV库文件是opencv_world310d.lib，并且运行的方式是DEBUG。
然而在Creator中我链接的库是opencv_world310.lib，并且运行的方式是DEBUG。
加上后缀"d"之后，终于可以运行了！或者将运行方式改成release，同样也可以运行！
这个bug在之前的imread函数是没有问题的，这进一步搞混淆了！

```cpp
void convert_bin_to_png(string in_path, string out_path)
{
  ifstream f;
  f.open(in_path, ios::binary);
  uint32_t W,H;
  f.read((char*)&W, sizeof(uint32_t));
  f.read((char*)&H, sizeof(uint32_t));
  char *img = new char[W*H];
  f.read(img, W*H);
  Mat save = Mat(H, W, CV8UC1);
  save.data = (uint8_t)img;
  imwrite(out_path, save); // 问题就出现在这一行
  delete img;
}
```

**教训：** lib库的引用一定要**遵循规范**！debug版本的代码要引用debug版本的lib，release版本的代码要引用release版本的库。
举一反三，x64的编译方式用x64的库，x86的编译方式用x86的库，vc08，vc10，vc12的库最好也一致！

### 使用Creator创建的控制台程序无任何输出？
debug时间，2017-09-25 22:20-22:30

如题，程序本身是能够正确运行的，例如我输出的文件是正确的。
但是在终端运行程序时，程序本身没有任何输出，不论是正确的信息还是错误的信息。
最后发现是使用Creator构建程序时，没有在pro文件中加入CONFIG+=console导致的，加入之后程序就能正常输出了。
加入之后记得qmake+rebuild一下，仅仅重新编译有时候不会更新的。

**教训：**使用qtcreator构建工程时一定要清楚常用的几个参数的含义以及应用，pro文件并不是啥都不用写就能正确配置的。

### 处理三通道图像出错？
2017-09-26, debug时间，10分钟。

下面一段代码将float图像转换成color_map形式，在处理每个通道的数据时，地址没有写对，W和c忘记乘上通道数了。
```cpp
int cvt2color(float *img, uint32_t W, uint32_t H, uint8_t **img_out)
{
    const uint32_t N = 511;
    uint8_t color_map[N][3];
    for(uint32_t i=0; i<N; i++)
    {
        color_map[i][0] = i<=255 ? 255-i : 0;
        color_map[i][1] = i<=255 ? i : 510-i;
        color_map[i][2] = i<=255 ? 0 : i-255;
    }

    *img_out = new uint8_t[3*W*H];
    for(uint32_t r=0; r<H; r++)
        for(uint32_t c=0; c<W; c++)
        {
            int32_t loc = (int32_t)img[r*W+c] + 255;
            for(int k=0; k<3; k++)
            {
                if(loc < 0)
                    loc = 0;
                else if(loc > 510)
                    loc = 510;
                // (*img_out)[r*W+c+k] = color_map[(uint32_t)loc][k]; // 错误
                (*img_out)[r*3*W+3*c+k] = color_map[(uint32_t)loc][k]; // 正确

            }
        }

    return 0;
}
```

**教训：**处理多通道图像一定要注意寻址和通道的关系。

### 定点化的程序反而更慢？
2017-10-11 debug时间：1天

使用定点小数进行优化，在PC上跑是有1/5的时间减少的，但是移植到ARM上，发现反而运行的时间还更长，差不多是1.5倍左右，很纳闷。

原因：最后查到了程序中有一个限制运算数量的宏常数写错了。。。。本来要限制在XXX_YYY=1500的，结果却成了XXX_APP_YYY=5000。
两个宏搞混了，原因就是在pc版只有XXX_YYY这个宏，而在ARM版本上却将XXX_YYY偷偷改成了XXX_APP_YYY。

**教训：**不能太相信develop中的程序，任何地方都有可能变更，要有强大的内心的面对这一切，也要有仔细观察的耐心。
