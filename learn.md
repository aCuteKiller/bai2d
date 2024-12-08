## 学习笔记
+ 关于CLion中导入EasyX的步骤
  1. 下载支持Mingw的easyx4mingw
  2. 配置CMakeLists.txt  
  ``
  set(EasyX_INC "D:\\Users\\22190\\Desktop\\BaiAdventure\\EasyX\\include")
  set(EasyX_LINK "D:\\Users\\22190\\Desktop\\BaiAdventure\\EasyX\\lib64")
  include_directories(${EasyX_INC})
  link_directories(${EasyX_LINK})
  target_link_libraries(BaiAdventure ${EasyX_LINK}/libeasyx.a)
  ``
  3. 刷新CMake