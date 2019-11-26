# Lotta

A multi-threaded C++ network library based on the **reactor** pattern. The project was implemented with reading the famous book which exhaustively introduces [muduo](https://github.com/chenshuo/muduo) by [Shuo Chen](https://github.com/chenshuo). I really learned a lot from it.

The project is designed to run with C++17 and on linux machines only. To build it up, just run `mkdir build && cd build; cmake ..; make -j 4`, then you can get the unittest(gtest) executation program and the static library.

