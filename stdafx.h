// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <process.h>
#include <Tchar.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

#define _cplusplus

// TODO: reference additional headers your program requires here
#include "mex.h"
#include "matrix.h"

#include "MPH16_SDK.h"

#pragma comment(lib, "libmx.lib")
#pragma comment(lib, "libmex.lib")
#pragma comment(lib, "libmat.lib")

#pragma comment(lib, "libut.lib")

#pragma comment(lib, "ThorPinholeStepper.lib")