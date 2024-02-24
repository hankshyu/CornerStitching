#ifndef __UNITS_H__
#define __UNITS_H__

#include <cstdint>
#include <limits.h>

typedef int32_t len_t;
typedef int64_t  area_t;

typedef double flen_t;

#define LEN_T_MAX std::numeric_limits<len_t>::max()
#define LEN_T_MIN std::numeric_limits<len_t>::min()

#define FLEN_T_MAX std::numeric_limits<flen_t>::max()
#define FLEN_T_MIN std::numeric_limits<flen_t>::min()

#define AREA_T_MAX std::numeric_limits<area_t>::max()
#define AREA_T_MIN std::numeric_limits<area_t>::min()

#endif // __UNITS_H__