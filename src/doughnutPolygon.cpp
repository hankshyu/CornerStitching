#include "doughnutPolygon.h"
#include "cord.h"

std::ostream &operator << (std::ostream &os, const DoughnutPolygon &dp){

    boost::polygon::direction_1d direction = boost::polygon::winding(dp);
    
    
    if(direction == boost::polygon::direction_1d_enum::CLOCKWISE){
        for(auto it = dp.begin(); it != dp.end(); ++it){
            std::cout << *it << " ";
        }
    }else{
        std::vector<Cord> buffer;
        for(auto it = dp.begin(); it != dp.end(); ++it){
            buffer.push_back(*it);
        }
        for(std::vector<Cord>::reverse_iterator it = buffer.rbegin(); it != buffer.rend(); ++it){
            std::cout << *it << " ";
        }
    }

    return os;

}