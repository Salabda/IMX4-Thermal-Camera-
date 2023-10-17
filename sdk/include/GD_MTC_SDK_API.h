#pragma once
#include "PlatformType.h"

#if USE_PLATFORM_TYPE_WINDOWS

#if defined(MEASURECORENET_LIBRARY)
#  define GD_MTC_SDK_API __declspec(dllexport)
#else
#  define GD_MTC_SDK_API __declspec(dllimport)
#endif

#define GD_MTC_SDK_API_IMPORT __declspec(dllimport)
#define GD_MTC_SDK_API_EXPORT __declspec(dllexport)

#elif USE_PLATFORM_TYPE_LINUX

#define Q_DECL_EXPORT     __attribute__((visibility("default")))
#define Q_DECL_IMPORT     __attribute__((visibility("default")))
#define Q_DECL_HIDDEN     __attribute__((visibility("hidden")))

#define MEASURECORENET_LIBRARY

#if defined(MEASURECORENET_LIBRARY)
#  define GD_MTC_SDK_API Q_DECL_EXPORT
#else
#  define GD_MTC_SDK_API Q_DECL_IMPORT
#endif

#define GD_MTC_SDK_API_IMPORT Q_DECL_IMPORT
#define GD_MTC_SDK_API_EXPORT Q_DECL_EXPORT

#endif

