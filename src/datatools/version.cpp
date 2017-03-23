/*! version.cpp */
/* Copyright (c) 2017 Aaditya Kalsi */

#include "datatools/version.hpp"

#define __stringify(x) #x
#define stringify(x) __stringify(x)

namespace dt
{

CString version()
{
    return stringify(DATATOOLS_VER_STRING);
}

} // namespace dt
