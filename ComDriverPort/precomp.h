
#ifndef _COM422_PRECOMP_H_
#define _COM422_PRECOMP_H_

#pragma warning(push, 3)

#include <ntifs.h>
#include <Ndis.h>
#include <krnlcom.h>

HR_EXTERN_C_START

#include <ntddk.h>
#include <wdm.h>
#include <ntddser.h>
#include <ntstrsafe.h>
#include <wmilib.h>
#include <wmidata.h>

HR_EXTERN_C_END

#pragma warning(pop)

#include "..\ComDriverBus\common.h"
#include "common.h"
#include "version.h"

#endif /* _COM422_PRECOMP_H_ */
