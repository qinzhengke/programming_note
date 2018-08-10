aux_source_directories(. a)本来就是追加的形式，不需要额外操作。
```cmake
aux_source_directories(. a)
aux_source_directories(.. a)
aux_source_directories(../.. a)
add_executable(exe_a ${a})
```
