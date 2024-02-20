#ifndef __FLOORPLAN_H__
#define __FLOORPLAN_H__

#include <string.h>
#include <unordered_map>

#include "tile.h"
#include "rectangle.h"
#include "rectilinear.h"
#include "connection.h"
#include "cornerStitching.h"

class Floorplan{
private:
    std::string mName;
    Rectangle mContour;
    
public:
    CornerStitching cs;
    std::vector<Rectilinear> allRectilinears;
    std::vector<Connection> allConnections;
    
    std::unordered_map<Tile *, Rectilinear*> blockTilePayload;
    std::unordered_map<Tile *, std::vector<Rectilinear*>> overlapTilePayload;


    Floorplan();
    Floorplan(std::string name, Rectangle contour);
    Floorplan(const Floorplan &other);

    Floorplan& operator = (const Floorplan &other);
    bool operator == (const Floorplan &comp) const;

    std::string getName() const;
    Rectangle getContour() const;

    void setName(std::string name);
    void setContour(Rectangle contour);

    double calculateHPWL();
    
};

#endif // #define __FLOORPLAN_H__