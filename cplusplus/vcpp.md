[TOC]



## 隐藏致命：函数明明存在却报unresolved symbol！

在VS下，函数明明存在为什么还是报出unresolved symbol错误?可能是字符集问题！特别是将不同操作系统下的文件进行混用的时候！通通，通通，通通改成多字节字符。

这个问题怎么google？一查unresolved symbol全都是确实没有添加函数定义的。
回答：搜索关键词：function do exist unresolved symbol

在vs下如果是main.cpp、a.h和a.c这种组合一定会出问题，可以试一试，注意，这里a.c是C语言写的，用于板子上的程序，不能有c++的特性，但是main.cpp可以，因为会用到第三方库进行显示，例如用Opencv读取、显示和保存图片。
解决方法，在project property的C++/Advanced中，Complie as一项从default改成as c++。
在QtCreator下也是一样的，需要设置编译方式为C++







