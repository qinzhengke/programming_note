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

**教训：**使用qtcreator构建工程时一定要清楚常用的几个参数的含义以及应用，pro文件并不是啥都不用写就能正确配置的。
