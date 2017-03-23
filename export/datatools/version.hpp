/*! version.hpp */
/* Copyright (c) 2017 Aaditya Kalsi */

#if defined(_WIN32) && !defined(__GCC__)
#  ifdef BUILDING_DATATOOLS
#    define DT_API __declspec(dllexport)
#  else
#    define DT_API __declspec(dllimport)
#  endif
#else
#  ifdef BUILDING_DATATOOLS
#    define DT_API __attribute__ ((visibility ("default")))
#  else
#    define DT_API 
#  endif
#endif

namespace dt
{

using cstring = char const*;

DT_API
/*!
 *
 */
cstring version();

} // namespace dt
