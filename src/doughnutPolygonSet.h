#ifndef __DOUGHNUTPOLYGONSET_H__
#define __DOUGHNUTPOLYGONSET_H__

#include <vector>

#include "boost/polygon/polygon.hpp"
#include "units.h"
#include "doughnutPolygon.h"

typedef std::vector<DoughnutPolygon> doughnutPolygonSet;

namespace dps{

    inline bool oneShape(const doughnutPolygonSet &dpSet){
        return (dpSet.size() == 1);
    }

    inline bool noHole(const doughnutPolygonSet &dpSet){

        for(int i = 0; i < dpSet.size(); ++i){
            DoughnutPolygon curSegment = dpSet[i];
            if(curSegment.begin_holes() != curSegment.end_holes()) return false;
        }

        return true;
    }
    
}




#endif // __DOUGHNUTPOLYGONSET_H__