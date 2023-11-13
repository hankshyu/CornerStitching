#ifndef __CORD_H__
#define __CORD_H__

#include <ostream>

#include "boost/polygon/polygon.hpp"
#include "units.h"

typedef boost::polygon::point_data<len_t> Cord;

// Implement hash function for map and set data structure
namespace std {
template <>
struct hash<Cord> {
    size_t operator()(const Cord &key) const;
};
}  // namespace std

std::ostream &operator<<(std::ostream &os, const Cord &c);

inline len_t calL1Distance(Cord c1, Cord c2) {
    return boost::polygon::manhattan_distance<Cord, Cord>(c1, c2);
}

#endif  // #define __CORD_H__