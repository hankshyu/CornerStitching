#include <assert.h>
#include "floorplan.h"

Floorplan::Floorplan()
    : mChipContour(Rectangle(0, 0, 0, 0)) , mAllRectilinearCount(0), mSoftRectilinearCount(0), mHardRectilinearCount(0), mPreplacedRectilinearCount(0), mConnectionCount(0) {
}

Floorplan::Floorplan(GlobalResult gr)
    :mChipContour(Rectangle(0, 0, gr.chipWidth, gr.chipHeight)), mAllRectilinearCount(gr.blockCount), mConnectionCount(gr.connectionCount) {
    
    assert(mAllRectilinearCount == gr.blocks.size());
    assert(mConnectionCount == gr.connections.size());

    

}

Rectangle Floorplan::getChipContour() const {
    return this->mChipContour;
}

int Floorplan::getAllRectilinearCount() const {
    return this->mAllRectilinearCount;
}

int Floorplan::getSoftRectilinearCount() const {
    return this->mSoftRectilinearCount;
}

int Floorplan::getHardRectilinearCount() const {
    return this->mHardRectilinearCount;
}

int Floorplan::getConnectionCount() const {
    return this->mConnectionCount;
}


int Floorplan::getGlobalAspectRatioMin() const {
    return this->mGlobalAspectRatioMin;
}
int Floorplan::getGlobalAspectRatioMax() const {
    return this->mGlobalAspectRatioMax;
}
int Floorplan::getGlobalUtilizationMin() const {
    return this->mGlobalUtilizationMin;
}

int Floorplan::setGlobalAspectRatioMin(int globalAspectRatioMin){
    this->mGlobalAspectRatioMin = globalAspectRatioMin;
}
int Floorplan::setGlobalAspectRatioMax(int globalAspectRatioMax){
    this->mGlobalAspectRatioMax = globalAspectRatioMax;

}
int Floorplan::setGlobalUtilizationMin(int globalUtilizationMin){
    this->mGlobalUtilizationMin = globalUtilizationMin;
}




double Floorplan::calculateHPWL(){
    double floorplanHPWL = 0;
    for(Connection c : this->allConnections){
        floorplanHPWL += c.calculateCost();
    }
    
    return floorplanHPWL;
}