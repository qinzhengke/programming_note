### cmake 添加源文件
aux_source_directories(. a)本来就是追加的形式，不需要额外操作。
```cmake
aux_source_directories(. a)
aux_source_directories(.. a)
aux_source_directories(../.. a)
add_executable(exe_a ${a})
```

### cmake设置编译选项
cmake中设置了编译选项
```cmake
set(CMAKE_C_FLAGS "-std=c99")
```
但是对应的Makefile里怎么都找不到对应的改动。

后来通过git来查看，才知道，CMake生成的Makefile中不直接包含flag，flag相关的设置都在CMakeFiles/xxx.dir/flags.make文件中。

这里不得不说git真是神器，让蛛丝马迹无处可藏。
