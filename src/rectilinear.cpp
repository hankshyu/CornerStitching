#include "rectilinear.h"
#include "cSException.h"
#include "limits.h"

Rectilinear::Rectilinear()
    : mId(-1), mName(""), mType(rectilinearType::EMPTY), mGlobalPlacement(Rectangle(0, 0, 0, 0)),
    mLegalArea(0), mAspectRatioMin(0), mAspectRatioMax(std::numeric_limits<double>::max()), mUtilizationMin(0){
}

Rectilinear::Rectilinear(int id, std::string name, rectilinearType type, Rectangle initPlacement,
                        area_t legalArea, double aspectRatioMin, double aspectRatioMax, double utilizationMin)
    : mId(id), mName(name), mType(type), mGlobalPlacement(initPlacement),
    mLegalArea(legalArea), mAspectRatioMin(aspectRatioMin), mAspectRatioMax(aspectRatioMax), mUtilizationMin(utilizationMin){
}

Rectilinear::Rectilinear(const Rectilinear &other)
    : mId(other.mId), mName(other.mName), mType(other.mType), mGlobalPlacement(other.mGlobalPlacement),
    mLegalArea(other.mLegalArea), mAspectRatioMin(other.mAspectRatioMin), mAspectRatioMax(other.mAspectRatioMax), mUtilizationMin(other.mUtilizationMin){
        this->blockTiles = std::unordered_set<Tile *>(other.blockTiles.begin(), other.blockTiles.end());
        this->overlapTiles = std::unordered_set<Tile *>(other.overlapTiles.begin(), other.overlapTiles.end());
}

Rectilinear &Rectilinear::operator = (const Rectilinear &other) {
    if(this == &other) return (*this);

    this->mId = other.mId;
    this->mName = other.mName;
    this->mType = other.mType;
    this->mGlobalPlacement = other.mGlobalPlacement;
    this->mLegalArea = other.mLegalArea;
    this->mAspectRatioMin = other.mAspectRatioMin;
    this->mAspectRatioMax = other.mAspectRatioMax;
    this->mUtilizationMin = other.mUtilizationMin;

    this->blockTiles = std::unordered_set<Tile *>(other.blockTiles);
    this->overlapTiles = std::unordered_set<Tile*>(other.overlapTiles);

    return (*this);
}

bool Rectilinear::operator == (const Rectilinear &comp) const {
    bool sameId = (this->mId == comp.mId);
    bool sameName = (this->mName == comp.mName);
    bool sameType = (this->mType == comp.mType);
    bool sameLegalArea = (this->mLegalArea == comp.mLegalArea);
    bool sameGlobalPlacement = (this->mGlobalPlacement == comp.mGlobalPlacement);

    bool sameBlockTiles = (this->blockTiles == comp.blockTiles);
    bool sameOverlapTiles = (this->overlapTiles == comp.overlapTiles);

    return (sameId && sameName && sameType && sameLegalArea && sameGlobalPlacement && sameBlockTiles && sameOverlapTiles);
}

int Rectilinear::getId() const {
    return this->mId;
}
std::string Rectilinear::getName() const {
    return this->mName;
}
rectilinearType Rectilinear::getType() const {
    return this->mType;
}
Rectangle Rectilinear::getGlboalPlacement() const {
    return this->mGlobalPlacement;
}
area_t Rectilinear::getLegalArea() const {
    return this->mLegalArea;
}
double Rectilinear::getAspectRatioMin() const {
    return this->mAspectRatioMin;
}
double Rectilinear::getAspectRatioMax() const {
    return this->mAspectRatioMax;
}
double Rectilinear::getUtilizationMin() const {
    return this->mUtilizationMin;
}

void Rectilinear::setId(int id){
    this->mId = id;
}
void Rectilinear::setName(std::string name){
    this->mName = name;
}
void Rectilinear::setType(rectilinearType type){
    this->mType = type;
}
void Rectilinear::setGlobalPlacement(const Rectangle &globalPlacement){
    this->mGlobalPlacement = globalPlacement;
}
void Rectilinear::setLegalArea(area_t legalArea){
    this->mLegalArea = legalArea;
}
void Rectilinear::setAspectRatioMin(double aspectRatioMin){
    this->mAspectRatioMin = aspectRatioMin;
}
void Rectilinear::setAspectRatioMax(double aspectRatioMax){
    this->mAspectRatioMax = aspectRatioMax;
}
void Rectilinear::setUtilizationMin(double utilizationMin){
    this->mUtilizationMin = utilizationMin;
}

Rectangle Rectilinear::calculateBoundingBox() const {
    if(blockTiles.empty() && overlapTiles.empty()){
        throw CSException("RECTILINEAR_01");
    }

    len_t BBXL, BBYL, BBXH, BBYH;
    Tile *randomTile = (blockTiles.empty())? (*(overlapTiles.begin())) : (*(blockTiles.begin()));

    BBXL = (randomTile)->getXLow();
    BBYL = (randomTile)->getYLow();
    BBXH = (randomTile)->getXHigh();
    BBYH = (randomTile)->getYHigh();

    for(Tile *t : blockTiles){
        len_t xl = t->getXLow();
        len_t yl = t->getYLow();
        len_t xh = t->getXHigh();
        len_t yh = t->getYHigh();
        
        if(xl < BBXL) BBXL = xl;
        if(yl < BBYL) BBYL = yl;
        if(xh > BBXH) BBXH = xh;
        if(yh > BBYH) BBYH = yh;
    }
    for(Tile *t : overlapTiles){
        len_t xl = t->getXLow();
        len_t yl = t->getYLow();
        len_t xh = t->getXHigh();
        len_t yh = t->getYHigh();
        
        if(xl < BBXL) BBXL = xl;
        if(yl < BBYL) BBYL = yl;
        if(xh > BBXH) BBXH = xh;
        if(yh > BBYH) BBYH = yh;
    }

    return Rectangle(BBXL, BBYL, BBXH, BBYH);
}

area_t Rectilinear::calculateActualArea() const {
    area_t actualArea = 0;
    for(Tile *t : blockTiles){
        actualArea += t->getArea();
    }
    for(Tile *t : overlapTiles){
        actualArea += t->getArea();
    }
    return actualArea;
}

double Rectilinear::calculateUtilization() const {
    return double(calculateActualArea())/double(rec::getArea(calculateBoundingBox()));
}

bool Rectilinear::isLegalEnoughArea() const {
    return (calculateActualArea() >= this->mLegalArea);
}

bool Rectilinear::isLegalAspectRatio() const {
    double aspectRatio = rec::calculateAspectRatio(calculateBoundingBox());
    return (aspectRatio > mAspectRatioMin) && (aspectRatio < mAspectRatioMax);
}

bool Rectilinear::isLegalUtilization() const {
    double minLegalArea = rec::getArea(calculateBoundingBox()) * mUtilizationMin;
    return (double(calculateActualArea()) >= minLegalArea);
}

bool Rectilinear::isLegalNoHole() const {
    return true;
}

std::ostream &operator << (std::ostream &os, const Rectilinear &r){
    os << "ID = " << r.mId << " Name = " << r.mName << " Type = ";
    switch (r.mType)
    {
    case rectilinearType::EMPTY:
        os << "EMPTY";
        break;
    case rectilinearType::SOFT:
        os << "SOFT";
        break;
    case rectilinearType::HARD:
        os << "HARD";
        break;
    case rectilinearType::PREPLACED:
        os << "PREPLACED";
        break;
    default:
        break;
    }

    os << "Global Placement = " << r.mGlobalPlacement << std::endl;
    os << "Aspect Ratio: " << r.mAspectRatioMin << " ~ " << r.mAspectRatioMax << ", Utilization Min = " << r.mUtilizationMin << std::endl; 

    os << "BLOCK Tiles (" << r.blockTiles.size() << ")" << std::endl;
    for(Tile *t : r.blockTiles){
        os << *t << std::endl;
    }

    os << "OVERLAP Tiles (" << r.overlapTiles.size() << ")";
    for(Tile *t : r.overlapTiles){
        os << std::endl << *t; 
    }
    return os;
}
