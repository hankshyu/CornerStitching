#ifndef __FLOORPLAN_H__
#define __FLOORPLAN_H__

#include <string.h>
#include <unordered_map>

#include "tile.h"
#include "rectilinear.h"
#include "rectangle.h"

class Floorplan{
private:
    std::string mName;
    Rectangle mContour;
    
public:

    std::unordered_map<Tile *, Rectilinear*> blockTilePayload;
    std::unordered_map<Tile *, std::vector<Rectilinear*>> overlapTilePayload;

    std::unordered_set<Rectilinear*> allRectilinears;
    std::unordered_set<Tile *> allTiles;


    Floorplan();
    Floorplan(std::string name, Rectangle contour);

    std::string getName() const;
    Rectangle getContour() const;

    void setName(std::string name);
    void setContour(Rectangle contour);


    double calculateHPWL();
    
};

#endif // #define __FLOORPLAN_H__