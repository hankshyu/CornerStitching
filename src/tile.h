#ifndef __TILE_H__
#define __TILE_H__

#include "boost/polygon/polygon.hpp"
#include "units.h"
#include "cord.h"
#include "line.h"
#include "rectangle.h"

namespace gtl = boost::polygon;

enum class tileType{
    BLOCK, BLANK, OVERLAP
};

class Tile{
private:
    tileType mType;
    Rectangle mRectangle;

    /* data payload */
    
    // int mTessIndex;
    // void *mTess;

public:

    Tile *rt, *tr, *bl, *lb;

    std::vector <int> OverlapFixedTesseraeIdx;
    std::vector <int> OverlapSoftTesseraeIdx;
    
    Tile();
    Tile(tileType t, Rectangle rect);
    Tile(tileType t, Cord ll, len_t w, len_t h);
    Tile(tileType t, Cord ll, Cord ur);
    Tile(const Tile &other);

    Tile& operator = (const Tile &other);
    bool operator == (const Tile &comp) const;
    friend std::ostream &operator << (std::ostream &os, const Tile &t);
    
    tileType getType() const;
    Rectangle getRectangle() const;

    len_t getWidth() const;
    len_t getHeight() const;

     
    Cord getLowerLeft() const;
    // The "contours" of the rectange, containing the upper,right row of non-owning blocks.
    // More often as representation of the entire tile 
    Cord getUpperLeft() const;
    Cord getLowerRight() const;
    Cord getUpperRight() const;
    // Are the exact blocks that the tile contains
    Cord getContainedUpperLeft() const;
    Cord getContainedLowerRight() const;
    Cord getContainedUpperRight() const;
    
    void setType(tileType type);
    void setWidth(len_t width);
    void setHeight(len_t height);
    void setLowerLeft(Cord lowerLeft);

    double calAspectRatio() const;
    area_t calArea() const;
    
    // checks if a point is contained inside the tile
    bool checkXCordInTile(const Cord &point) const;
    bool checkYCordInTile(const Cord &point) const;
    bool checkCordInTile(const Cord &point) const;

    // if input Tile's lower-left touches the right edge of current tile (used in Directed Area Enumeration)
    // bool checkTRLLTouch(Tile *right) const;
    // bool cutHeight (len_t cut) const;

};

// Implement hash function for map and set data structure
namespace std{
    template<>
    struct hash<Tile>{
        size_t operator()(const Tile &key) const;
    };
}

std::ostream &operator << (std::ostream &os, const Tile &t);
std::ostream &operator << (std::ostream &os, const enum tileType &t);

std::vector<Tile> mergeCutTiles(std::vector<Tile> toMerge, std::vector<Tile> toCut);
#endif // __TILE_H__