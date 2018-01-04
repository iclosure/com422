
#ifndef _COM422_PCOMM_BUS_VERSION_H_
#define _COM422_PCOMM_BUS_VERSION_H_

#define PCOMM_BUS_V1 1
#define PCOMM_BUS_V2 0
#define PCOMM_BUS_V3 1
#define PCOMM_BUS_V4 0

#define MK_VERSION_STR1(V1, V2, V3, V4) #V1 "." #V2 "." #V3 "." #V4
#define MK_VERSION_STR(V1, V2, V3, V4) MK_VERSION_STR1(V1, V2, V3, V4)

#define PCOMM_VERSION_STR MK_VERSION_STR(PCOMM_BUS_V1, PCOMM_BUS_V2, PCOMM_BUS_V3, PCOMM_BUS_V4)

#endif /* _COM422_PCOMM_BUS_VERSION_H_ */