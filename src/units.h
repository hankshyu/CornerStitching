#ifndef __UNITS_H__
#define __UNITS_H__

#include <limits.h>

typedef int32_t len_t;
typedef int64_t  area_t;

#define LEN_T_MAX std::numeric_limits<len_t>::max()
#define LEN_T_MIN std::numeric_limits<len_t>::min()
#define AREA_T_MAX std::numeric_limits<area_t>::max()
#define AREA_T_MIN std::numeric_limits<area_t>::min()

#endif // __UNITS_H__