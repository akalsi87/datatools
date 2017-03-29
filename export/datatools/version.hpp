/*! version.hpp */
/* Copyright (c) 2017 Aaditya Kalsi */

#ifndef _DATATOOLS_VERSION_HPP_
#define _DATATOOLS_VERSION_HPP_

#if defined(_WIN32) && !defined(__GCC__)
#  ifdef BUILDING_DATATOOLS
#    define DT_API __declspec(dllexport)
#  else
#    define DT_API __declspec(dllimport)
#  endif
#  ifndef _CRT_SECURE_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
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

using CString = char const*;

DT_API
/*!
 * Get the version as a C string.
 */
CString version();

} // namespace dt

#include <cstddef>
#include <cstdint>

#endif//_DATATOOLS_VERSION_HPP_
