#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif /* !NOMINMAX */

#include <Windows.h>
#include <Windowsx.h>

#include <windows.h>
#include <windowsx.h>

#include <Uxtheme.h>
#include <Vsstyle.h>
#include <Vssym32.h>

#include <wingdi.h>
#include <commctrl.h>

#include <mmreg.h>
#include <wmsdk.h>

#include <d2d1_2.h>
#include <d2d1helper.h>
#include <d2d1_2helper.h>
#include <dwrite_2.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#pragma comment(lib, "UxTheme.lib")

#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "comctl32.lib")

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wmvcore.lib")

#pragma comment(lib, "Rpcrt4.lib")

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifdef min
#undef min
#endif /* min */

#ifdef max
#undef max
#endif /* max */
