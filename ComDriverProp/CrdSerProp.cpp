
#include "stdafx.h"
#include "CrdSerProp.h"
#include <tchar.h>

HINSTANCE  _g_hModule;		// 

VOID WINAPI SetHInstance(HINSTANCE hModule)
{
	_g_hModule = hModule;

	HR_PRINT_SET_PREFIX("#> COM ");
}
