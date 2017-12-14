### CV_EXPORT宏

```cpp
#if (defined _WIN32 || defined WINCE || defined __CYGWIN__) && defined CVAPI_EXPORTS
#  define CV_EXPORTS __declspec(dllexport)
#elif defined __GNUC__ && __GNUC__ >= 4
#  define CV_EXPORTS __attribute__ ((visibility ("default")))
#else
#  define CV_EXPORTS
#endif
```

那么，visibility属性又是什么呢？下面的网页解释的很好。

> https://www.ibm.com/developerworks/cn/aix/library/au-aix-symbol-visibility/index.html
