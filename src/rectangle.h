#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__

#include <ostream>

#include "boost/polygon/polygon.hpp"
#include "units.h"
#include "cord.h"
#include "line.h"

typedef boost::polygon::rectangle_data<len_t> Rectangle;

namespace rec{

    inline len_t getWidth(Rectangle rec){
        return boost::polygon::delta(boost::polygon::horizontal(rec));
    }
    inline len_t getHeight(Rectangle rec){
        return boost::polygon::delta(boost::polygon::vertical(rec));
    }
}

// Implement hash function for map and set data structure
namespace std{
    template<>
    struct hash<Rectangle>{
        size_t operator()(const Rectangle &key) const;
    };
}

std::ostream &operator<<(std::ostream &os, const Rectangle &c);

#endif  // #define __RECTANGLE_H__