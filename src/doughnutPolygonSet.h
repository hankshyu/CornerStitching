#ifndef __DOUGHNUTPOLYGONSET_H__
#define __DOUGHNUTPOLYGONSET_H__

#include <vector>

#include "boost/polygon/polygon.hpp"
#include "units.h"
#include "rectangle.h"
#include "doughnutPolygon.h"

typedef std::vector<DoughnutPolygon> DoughnutPolygonSet;

namespace dps{

    inline bool oneShape(const DoughnutPolygonSet &dpSet){
        return (dpSet.size() == 1);
    }

    inline bool noHole(const DoughnutPolygonSet &dpSet){

        for(int i = 0; i < dpSet.size(); ++i){
            DoughnutPolygon curSegment = dpSet[i];
            if(curSegment.begin_holes() != curSegment.end_holes()) return false;
        }

        return true;
    }

    inline void diceIntoRectangles(const DoughnutPolygonSet &dpSet, std::vector<Rectangle> &fragments){
        boost::polygon::get_rectangles(fragments, dpSet);
    }
    
}


#endif // __DOUGHNUTPOLYGONSET_H__