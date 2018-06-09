// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WINDOWS
	#include "targetver.h"

	#define NOMINMAX
	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	// Windows Header Files:
	#include <windows.h>
#endif

#include <exception>
#include <assert.h>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <memory>

#pragma warning( disable : 4251 )
#pragma warning( disable : 26429 )
