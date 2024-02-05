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

    inline area_t getArea(Rectangle rec){
        return boost::polygon::area(rec);
    }

    inline double calculateAspectRatio(Rectangle rec){
        return double(boost::polygon::delta(boost::polygon::horizontal(rec))) /
                double(boost::polygon::delta(boost::polygon::vertical(rec)));
    }
    inline len_t getXL(Rectangle rec){
        return boost::polygon::xl(rec);
    } 

    inline len_t getXH(Rectangle rec){
        return boost::polygon::xh(rec);
    } 

    inline len_t getYL(Rectangle rec){
        return boost::polygon::yl(rec);
    } 

    inline len_t getYH(Rectangle rec){
        return boost::polygon::yh(rec);
    } 

    inline Cord getLL(Rectangle rec){
        return boost::polygon::ll(rec);
    }

    inline Cord getLR(Rectangle rec){
        return boost::polygon::lr(rec);
    }

    inline Cord getUL(Rectangle rec){
        return boost::polygon::ul(rec);
    }

    inline Cord getUR(Rectangle rec){
        return boost::polygon::ur(rec);
    }
    // Returns true if two objects overlap, parameter considerTouch is true touching at the sides or corners is considered overlap.
    inline bool hasIntersect(const Rectangle &rec1, const Rectangle &rec2, bool considerTouch){
        return boost::polygon::intersects(rec1, rec2, considerTouch);
    }

    // Returns true if smallRec is contained (completely within) BigRec
    inline bool isContained(const Rectangle &BigRec, const Rectangle &smallRec){
        return boost::polygon::contains(BigRec, smallRec, true);
    }

    //Returns true if the point is contained inside the rectangle
    inline bool isContained(const Rectangle &rec, Cord point){
        Rectangle actualRectangle =  Rectangle(getXL(rec), getYL(rec), getXH(rec) - 1, getYH(rec) - 1);
        return boost::polygon::contains(actualRectangle, point, true);
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