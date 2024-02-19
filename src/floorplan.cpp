#include "floorplan.h"

Floorplan::Floorplan()
    : mName(""), mContour(Rectangle(0, 0, 0, 0)) {
}

Floorplan::Floorplan(std::string name, Rectangle contour)
    : mName(name), mContour(contour) {
}

std::string Floorplan::getName() const {
    return this->mName;
}

Rectangle Floorplan::getContour() const {
    return this->mContour;
}

void Floorplan::setName(std::string name){
    this->mName = name;
}

void Floorplan::setContour(Rectangle contour){
    this->mContour = contour;
}