#include "cFileStream.h"

#ifndef LINUX
  #include "cFileStream_win.cpp"
#else
  #include "cFileStream_lnx.cpp"
#endif
