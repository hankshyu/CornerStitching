#include "rectilinear.h"
#include "cSException.h"
#include "limits.h"
#include "doughnutPolygon.h"
#include "doughnutPolygonSet.h"

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

bool Rectilinear::isLegalNoOverlap() const {
    using namespace boost::polygon::operators;

    doughnutPolygonSet dpSet, unionSet;

    for(Tile *t : this->blockTiles){
        Rectangle rt = t->getRectangle();
        assign(unionSet, dpSet&rt);
        if(!unionSet.empty()) return false;
        dpSet += rt;
    }
    for(Tile *t : this->overlapTiles){
        Rectangle rt = t->getRectangle();
        assign(unionSet, dpSet&rt);
        if(!unionSet.empty()) return false;
        dpSet += rt;
    }

    return true;
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
    using namespace boost::polygon::operators;
    doughnutPolygonSet curRectSet;

    for(Tile *t : this->blockTiles){
        curRectSet += t->getRectangle();
    }
    for(Tile *t : this->overlapTiles){
        curRectSet += t->getRectangle();
    }

    return dps::noHole(curRectSet);

}

bool Rectilinear::isLegalOneShape() const {
    using namespace boost::polygon::operators;
    doughnutPolygonSet curRectSet;

    for(Tile *t : this->blockTiles){
        curRectSet += t->getRectangle();
    }
    for(Tile *t : this->overlapTiles){
        curRectSet += t->getRectangle();
    }

    return dps::oneShape(curRectSet);
}

bool Rectilinear::isLegal(rectilinearIllegalType &illegalCode) const {

    // check if any tiles overlap
    using namespace boost::polygon::operators;
    doughnutPolygonSet curRectSet, unionSet;

    for(Tile *t : this->blockTiles){
        Rectangle rt = t->getRectangle();
        assign(unionSet, curRectSet&rt);
        if(!unionSet.empty()){
            illegalCode = rectilinearIllegalType::OVERLAP;
            return false;
        }
        curRectSet += rt;
    }
    for(Tile *t : this->overlapTiles){
        Rectangle rt = t->getRectangle();
        assign(unionSet, curRectSet&rt);
        if(!unionSet.empty()){
            illegalCode = rectilinearIllegalType::OVERLAP;
            return false;
        }
        curRectSet += rt;
    }
    
    // check if rectilinear is in one shape
    if(!dps::oneShape(curRectSet)){
        illegalCode = rectilinearIllegalType::TWO_SHAPE;
        return false;
    }

    // check if hole exist in rectilinear
    if(!dps::noHole(curRectSet)){
        illegalCode = rectilinearIllegalType::HOLE;
        return false;
    }

    // check area
    area_t actualArea = calculateActualArea();
    if(actualArea < this->mLegalArea){
        illegalCode = rectilinearIllegalType::AREA;
        return false;
    }

    // check bounding box aspect ratio
    Rectangle boundingBox = calculateBoundingBox();
    double boundingBoxAspectRatio = rec::calculateAspectRatio(boundingBox);
    if((boundingBoxAspectRatio < mAspectRatioMin) || (boundingBoxAspectRatio > mAspectRatioMax)){
        illegalCode = rectilinearIllegalType::ASPECT_RATIO;
        return false;
    }

    // check bounding box utilization
    double minUtilizationArea = double(rec::getArea(boundingBox)) * mUtilizationMin;
    if(double(actualArea) < minUtilizationArea){
        illegalCode = rectilinearIllegalType::UTILIZATION;
        return false;
    }

    // error free
    illegalCode = rectilinearIllegalType::LEGAL;
    return true;

}

void Rectilinear::acquireWinding(std::vector<Cord> &winding, windingDirection wd) const {
    if((blockTiles.empty())&&(overlapTiles.empty())){
        throw CSException("RECTILINEAR_02");
    }

    using namespace boost::polygon::operators;
    doughnutPolygonSet curRectSet;

    for(Tile *t : this->blockTiles){
        curRectSet += t->getRectangle();
    }
    for(Tile *t : this->overlapTiles){
        curRectSet += t->getRectangle();
    }

    assert(dps::oneShape(curRectSet));
    assert(dps::noHole(curRectSet));

    DoughnutPolygon rectilinearShape = curRectSet[0];
    boost::polygon::direction_1d direction = boost::polygon::winding(rectilinearShape);
    
    bool reverse_iterator;
    if(wd == windingDirection::CLOCKWISE){
        reverse_iterator = (direction == boost::polygon::direction_1d_enum::CLOCKWISE)? false : true;
    }else{ // wd == windingDirection::ANTICLOCKWISE
        reverse_iterator = (direction == boost::polygon::direction_1d_enum::COUNTERCLOCKWISE)? false : true;
    }
    
    if(!reverse_iterator){
        for(auto it = rectilinearShape.begin(); it != rectilinearShape.end(); ++it){
            winding.push_back(*it);
        }
    }else{
        std::vector<Cord> buffer;
        for(auto it = rectilinearShape.begin(); it != rectilinearShape.end(); ++it){
            buffer.push_back(*it);
        }
        for(std::vector<Cord>::reverse_iterator it = buffer.rbegin(); it != buffer.rend(); ++it){
            winding.push_back(*it);
        }
    }
}

std::ostream &operator << (std::ostream &os, const Rectilinear &r){
    os << "ID = " << r.mId << " Name = " << r.mName << " Type = " << r.mType;
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

    os << " Global Placement = " << r.mGlobalPlacement << std::endl;
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

std::ostream &operator << (std::ostream &os, const rectilinearType &t){
    switch (t)
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
        throw CSException("RECTILINEAR_05");
        break;
    }

    return os;

}

std::ostream &operator << (std::ostream &os, const rectilinearIllegalType &t){
    switch (t)
    {
    case rectilinearIllegalType::LEGAL:
        os << "LEGAL";
        break;
    case rectilinearIllegalType::OVERLAP:
        os << "OVERLAP";
        break;
    case rectilinearIllegalType::AREA:
        os << "AREA";
        break;
    case rectilinearIllegalType::ASPECT_RATIO:
        os << "ASPECT_RATIO";
        break;
    case rectilinearIllegalType::UTILIZATION:
        os << "UTILIZATION";
        break;
    case rectilinearIllegalType::HOLE:
        os << "HOLE";
        break;
    case rectilinearIllegalType::TWO_SHAPE:
        os << "TWO_SHAPE";
        break;
    case rectilinearIllegalType::MIN_CLEARANCE:
        os << "MIN_CLEARANCE";
        break;
    default:
        throw CSException("RECTILINEAR_06");
        break;
    }

    return os;

}

std::ostream &operator << (std::ostream &os, const windingDirection &w){
    switch (w)
    {
    case windingDirection::CLOCKWISE:
        os << "CLOCKWISE";
        break;
    case windingDirection::ANTICLOCKWISE:
        os << "ANTICLOCKWISE";
        break;
    default:
        throw CSException("RECTILINEAR_07");
        break;
    }

    return os;

}