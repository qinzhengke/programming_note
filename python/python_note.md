### 读取csv文件

读取csv文件的方式有很多种，标准库、numpy、panda都能提供，但是我觉得最方便的还是numpy提供的方法：

```python
from numpy import genfromtxt
my_data = genfromtxt('my_file.csv', delimiter=',')
```



### 读取csv文件

读取csv文件的方式有很多种，标准库、numpy、panda都能提供，但是我觉得最方便的还是numpy提供的方法：

```python
from numpy import genfromtxt
my_data = genfromtxt('my_file.csv', delimiter=',')
```



### No module name "xxx"

明明在解释器里能import的模块为什么在.py文件中无法找到呢？可能是因为python2和python3的原因，python2的解释器能找到对应模块，但是该模块没有安装python3的版本，如果用python3运行脚本自然无法找到。

### notepad++简单grep工具
有些嵌入式的程序无法在线debug，只能通过输出log的方式调试。但是如果一个程序有非常多的模块，每个模块都有自己输出的信息，那么这个log会非常的繁杂，一大堆和自己无关的信息，通过在linux下可以通过grep命令进行过滤，这种方式需要打一行命令，而且在Windows下无法直接使用。在这里我自己写了一个简单的notepad++“小插件”，只要一个快捷键就可以弹出新的标签页并且显示grep的内容。
下面是python代码：

```cpp
# -*- coding: utf-8 -*-
import sys
import os
fin = open(sys.argv[1], 'r')
lines = fin.readlines()
fin.close()
key = sys.argv[2]
fout = open(sys.argv[1]+'.find_'+key,'w')
for line in lines:
  if line.find(key) != -1:
    fout.write(line)
fout.close()
# 调用 notepad++ 显示处理好的文件。
os.system('D:\\softs\\npp.7.4.2.bin.x64\\notepad++.exe ' + sys.argv[1]+'.find_'+key)
```
然后在notepad++的‘run’菜单中添加运行脚本，其中当前标签页文件对应的目录由$(FULL_CURRENT_PATH)提供，别忘记还有第二个参数，就是要查找的关键字，这里是qzk，设定好快捷键，例如F6之后，就可以一键grep啦！

```shell
python D:\tools\np_grep\np_grep.py $(FULL_CURRENT_PATH) qzk
```