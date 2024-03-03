#ifndef __DOUGHNUTPOLYGON_H__
#define __DOUGHNUTPOLYGON_H__

#include "boost/polygon/polygon.hpp"
#include "units.h"

typedef boost::polygon::polygon_90_with_holes_data<len_t> DoughnutPolygon;

namespace std{
    template<>
    struct hash<DoughnutPolygon>{
        size_t operator()(const DoughnutPolygon &key) const;
    };
}

std::ostream &operator << (std::ostream &os, const DoughnutPolygon &dp);

#endif  // #define __DOUGHNUTPOLYGON_H__