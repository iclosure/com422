
#ifndef _COM422_PRECOMP_H_
#define _COM422_PRECOMP_H_

#pragma warning(push, 3)

#include <ntifs.h>
#include <krnlcom.h>

HR_EXTERN_C_START

// break point
#define COM422_BREAK_POINT HR_BREAK_POINT

#include <ntddk.h>
#include <wdf.h>
#include <ntddser.h>
#include <ntintsafe.h>
#include <initguid.h> // required for GUID definitions
#include <wmilib.h>
#include <wmidata.h>

HR_EXTERN_C_END

#pragma warning(pop)

#include "common.h"
#include "version.h"

#endif // _COM422_PRECOMP_H_
