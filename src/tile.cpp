#include "tile.h"
#include "cSException.h"

#include <iostream>

Tile::Tile()
    : mType(tileType::BLANK), mRectangle(Rectangle(0, 0, 0, 0)), rt(nullptr), tr(nullptr), bl(nullptr), lb(nullptr) {
}

Tile::Tile(tileType t, Rectangle rect)
    : mType(t), mRectangle(rect), rt(nullptr), tr(nullptr), bl(nullptr), lb(nullptr) {
}

Tile::Tile(tileType t, Cord ll, len_t w, len_t h)
    : mType(t), mRectangle(Rectangle(ll.x(), ll.y(), (len_t)(ll.x() + w), (len_t)(ll.y() + h))), rt(nullptr), tr(nullptr), bl(nullptr), lb(nullptr) {
}

Tile::Tile(tileType t, Cord ll, Cord ur)
    : mType(t), mRectangle(Rectangle(ll.x(), ll.y(), ur.x(), ur.y())), rt(nullptr), tr(nullptr), bl(nullptr), lb(nullptr) {
}

Tile::Tile(const Tile &other)
    : mType(other.getType()), mRectangle(other.getRectangle()), rt(other.rt), tr(other.tr), bl(other.bl), lb(other.lb) {
}

Tile &Tile::operator=(const Tile &other) {
    if (this == &other) return (*this);

    this->mType = other.getType();
    this->mRectangle = other.getRectangle();

    this->rt = other.rt;
    this->tr = other.tr;
    this->bl = other.bl;
    this->lb = other.lb;

    return (*this);
}

bool Tile::operator==(const Tile &comp) const {
    return (mType == comp.getType()) && (mRectangle == comp.getRectangle()) &&
           ((rt == comp.rt) && (tr == comp.tr) && (bl == comp.bl) && (lb == comp.lb));
}

tileType Tile::getType() const {
    return this->mType;
}
Rectangle Tile::getRectangle() const {
    return this->mRectangle;
}

len_t Tile::getWidth() const{
    return rec::getWidth(this->mRectangle);
}
len_t Tile::getHeight() const{
    return rec::getHeight(this->mRectangle);
};

len_t Tile::getXLow() const{
    return rec::getXL(this->mRectangle);
};
len_t Tile::getXHigh() const{
    return rec::getXH(this->mRectangle);
};
len_t Tile::getYLow() const{
    return rec::getYL(this->mRectangle);
};
len_t Tile::getYHigh() const{
    return rec::getYH(this->mRectangle);
};

Cord Tile::getLowerLeft() const{
    return rec::getLL(this->mRectangle);
};
Cord Tile::getLowerRight() const{
    return rec::getLR(this->mRectangle);
};
Cord Tile::getUpperLeft() const{
    return rec::getUL(this->mRectangle);
};
Cord Tile::getUpperRight() const{
    return rec::getUR(this->mRectangle);
};

void Tile::setType(tileType type) {
    this->mType = type;
}

void Tile::setWidth(len_t width) {
    using namespace boost::polygon;
    this->mRectangle = Rectangle(xl(mRectangle), yl(mRectangle), (xl(mRectangle) + width), yh(mRectangle));
};
void Tile::setHeight(len_t height) {
    using namespace boost::polygon;
    this->mRectangle = Rectangle(xl(mRectangle), yl(mRectangle), xh(mRectangle), (yl(mRectangle) + height));
};

void Tile::setLowerLeft(Cord lowerLeft) {
    this->mRectangle = Rectangle(lowerLeft.x(), lowerLeft.y(), (lowerLeft.x() + getWidth()), (lowerLeft.y() + getHeight()));
};

double Tile::calAspectRatio() const {
    return double(getWidth()) / double(getHeight());
};

area_t Tile::calArea() const {
    return getWidth() * getHeight();
};

bool Tile::checkXCordInTile(const Cord &point) const{
    Rectangle thisRec = this->getRectangle();
    bool xLeftInRange = (point.x() >= boost::polygon::xl(thisRec));
    bool xRightInRange = (point.x() < boost::polygon::xh(thisRec));
    return(xLeftInRange && xRightInRange);
}

bool Tile::checkYCordInTile(const Cord &point) const{
    Rectangle thisRec = this->getRectangle();
    bool yDownInRange = (point.y() >= boost::polygon::yl(thisRec));
    bool yUpInRange = (point.y() < boost::polygon::yh(thisRec));
    return(yDownInRange && yUpInRange);
}

bool Tile::checkCordInTile(const Cord &point) const{
    return (checkXCordInTile(point) && checkYCordInTile(point));
}

size_t std::hash<Tile>::operator()(const Tile &key) const {
    return (std::hash<int>()((int)key.getType())) ^ (std::hash<Rectangle>()(key.getRectangle()));
}

std::ostream &operator<<(std::ostream &os, const Tile &tile) {
    os << "T[";
    os << tile.mType << ", " << tile.mRectangle;
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const enum tileType &t) {

    switch (t)
    {
        case tileType::BLOCK:
            os << "BLOCK";
            break;
        case tileType::BLANK:
            os << "BLANK";
            break;
        case tileType::OVERLAP:
            os << "OVERLAP";
            break;
        default:
            throw CSException("TILE_01");
    }
    return os;
}

std::vector<Tile> mergeCutTiles(std::vector<Tile> toMerge, std::vector<Tile> toCut) {

    using namespace boost::polygon::operators;

    namespace gtl = boost::polygon;

    typedef gtl::polygon_data<len_t>                 Polygon;
    typedef gtl::point_data<len_t> Point;
    typedef std::vector<Polygon>                     PolygonSet;

    PolygonSet manipPoly;
    for ( auto &tile : toMerge ) {
        Rectangle mergeRect(tile.getLowerLeft().x(), tile.getLowerLeft().y(), tile.getUpperRight().x(), tile.getUpperRight().y());
        manipPoly += mergeRect;
    }
    for ( auto &tile : toCut ) {
        Rectangle cutRect(tile.getLowerLeft().x(), tile.getLowerLeft().y(), tile.getUpperRight().x(), tile.getUpperRight().y());
        manipPoly -= cutRect;
    }

    std::vector<Tile> manipTiles;
    std::set<len_t> yCord;

    for ( Polygon poly : manipPoly ) {
        for ( const Point &point : poly ) {
            yCord.insert(point.y());
        }
        std::vector<len_t> yCordVec(yCord.begin(), yCord.end());
        std::sort(yCordVec.begin(), yCordVec.end());
        for ( int i = 0; i < yCordVec.size() - 1; i++ ) {
            int lowY = yCordVec[i];
            int highY = yCordVec[i + 1];
            Rectangle mask(0, lowY, 100000000, highY);
            PolygonSet maskedPoly;
            maskedPoly += poly & mask;

            Rectangle maskedRect;
            gtl::extents(maskedRect, maskedPoly);
            manipTiles.push_back(Tile(tileType::OVERLAP, Cord(gtl::xl(maskedRect), gtl::yl(maskedRect)), gtl::xh(maskedRect) - gtl::xl(maskedRect), gtl::yh(maskedRect) - gtl::yl(maskedRect)));
        }
    }

    return manipTiles;
}
