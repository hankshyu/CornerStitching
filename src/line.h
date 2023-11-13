#ifndef __LINE_H__
#define __LINE_H__

#include <ostream>

#include "boost/polygon/polygon.hpp"
#include "units.h"

typedef boost::polygon::interval_data<len_t> Line;

// Implement hash function for map and set data structure
namespace std{
    template<>
    struct hash<Line>{
        size_t operator()(const Line &key) const;
    };
}

std::ostream &operator<<(std::ostream &os, const Line &c);

#endif  // #define __LINE_H__