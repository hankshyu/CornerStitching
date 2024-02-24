#ifndef __TESSERA_H__
#define __TESSERA_H__

#include <string.h>
#include <vector>
#include "rectangle.h"
#include "tile.h"
#include "cord.h"
enum class tesseraType{
    EMPTY ,SOFT, HARD, OVERLAP
};
class Tessera{
public: 
    std::string name;
    tesseraType type;
    Rectangle origBox;

    std::vector <Tile *> TileArr;
    std::vector <Tile *> OverlapArr; 

    Cord mBBLowerLeft;
    Cord mBBUpperRight;

    int insertTiles(Tile *tile);
    void calBoundingBox();
    void splitRectliearDueToOverlap();
};


#endif // __TESSERA_H__