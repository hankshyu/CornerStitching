#ifndef __RECTILINEAR_H__
#define __RECTILINEAR_H__

#include <string.h>
#include <unordered_set>
#include "rectangle.h"
#include "tile.h"

enum class rectilinearType{
    EMPTY, SOFT, HARD, PREPLACED
};

enum class rectilinearIllegalType{
    LEGAL, AREA, ASPECT_RATIO, UTILIZATION, HOLE, TWO_SHAPE
};

class Rectilinear{
private:
    int mId;
    std::string mName;
    rectilinearType mType;
    Rectangle mGlobalPlacement;

    area_t mLegalArea;

    double mAspectRatioMin;
    double mAspectRatioMax;

    double mUtilizationMin;


public: 

    std::unordered_set<Tile *> blockTiles;
    std::unordered_set<Tile *> overlapTiles;

    Rectilinear();
    Rectilinear(int id, std::string name, rectilinearType type, Rectangle globalPlacement,
                area_t legalArea, double aspectRatioMin, double aspectRatioMax, double utilizationMin);
    Rectilinear(const Rectilinear &other);

    Rectilinear &operator = (const Rectilinear &other);
    bool operator == (const Rectilinear &other) const;
    friend std::ostream &operator << (std::ostream &os, const Rectilinear &recti);

    int getId() const;
    std::string getName() const;
    rectilinearType getType() const;
    Rectangle getGlboalPlacement() const;
    area_t getLegalArea() const;
    double getAspectRatioMin() const;
    double getAspectRatioMax() const;
    double getUtilizationMin() const;


    void setId(int id);
    void setName(std::string name);
    void setType(rectilinearType type);
    void setGlobalPlacement(const Rectangle &globalPlacement);
    void setLegalArea(area_t legalArea);
    void setAspectRatioMin(double aspectRatioMin);
    void setAspectRatioMax(double aspectRatioMax);
    void setUtilizationMin(double utilizationMin);
    

    Rectangle calculateBoundingBox() const;
    area_t calculateActualArea() const;
    double calculateUtilization() const;

    bool isLegalEnoughArea() const;
    bool isLegalAspectRatio() const;
    bool isLegalUtilization() const;
    bool isLegalNoHole() const;
    bool isLegalOneShape() const;

    bool isLegalNoOverlap() const;

    bool isLegal(rectilinearIllegalType &illegalCode) const;

};

std::ostream &operator << (std::ostream &os, const Rectilinear &t);

#endif // __RECTILINEAR_H__