#ifndef __TESSERA_H__
#define __TESSERA_H__

#include <string.h>
#include <unordered_set>
#include "rectangle.h"
#include "tile.h"
#include "cord.h"
enum class tesseraType{
    SOFT, HARD, PREPLACED
};
class Tessera{
private:
    std::string mTessName;
    tesseraType mTessType;
    area_t mTessLegalArea;

    Rectangle mTessGlobalPhasePlacement;

public: 

    std::unordered_set <Tile *> blockTiles;
    std::unordered_set <Tile *> overlapTiles;

    Tessera();
    Tessera(tesseraType type, std::string name, area_t minArea, Rectangle initPlacement);
    Tessera(const Tessera &other);

    Tessera &operator = (const Tessera &other);
    bool operator == (const Tessera &other) const;
    friend std::ostream &operator << (std::ostream &os, const Tessera &t);

    std::string getName() const;
    tesseraType getType() const;
    area_t getLegalArea() const;
    Rectangle getGlobalPhasePlacement() const;

    Rectangle calculateBoundingBox() const;
    area_t calculateCurrentArea() const;

    bool isLegalNoHole();
    bool isLegalNoEnclave();
    bool isLegalEnoughArea();
    bool isLegalAspectRatio();
    bool isLegalUtilization();
    bool isLegal();

    void splitRectliearDueToOverlap();
};

std::ostream &operator << (std::ostream &os, const Tessera &t);


#endif // __TESSERA_H__