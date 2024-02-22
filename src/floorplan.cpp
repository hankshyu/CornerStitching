#include <assert.h>
#include "floorplan.h"
#include "cSException.h"

Floorplan::Floorplan()
    : mChipContour(Rectangle(0, 0, 0, 0)) , mAllRectilinearCount(0), mSoftRectilinearCount(0), mHardRectilinearCount(0), mPreplacedRectilinearCount(0), mConnectionCount(0) {
}

Floorplan::Floorplan(GlobalResult gr)
    :mChipContour(Rectangle(0, 0, gr.chipWidth, gr.chipHeight)), mAllRectilinearCount(gr.blockCount), mConnectionCount(gr.connectionCount) {
    
    assert(mAllRectilinearCount == gr.blocks.size());
    assert(mConnectionCount == gr.connections.size());

    // map for create connections
    std::unordered_map<std::string, Rectilinear*> nameToRectilinear;

    // create Rectilinears
    for(int i = 0; i < mAllRectilinearCount; ++i){
        GlobalResultBlock grb = gr.blocks[i];
        rectilinearType rType;
        if(grb.type == "SOFT"){
            rType = rectilinearType::SOFT;
        }else if(grb.type == "FIXED"){
            rType = rectilinearType::HARD;
        }else if(grb.type == "PREPLACED"){
            rType = rectilinearType::PREPLACED;
        }else{
            throw CSException("FLOORPLAN_01");
        }

        Rectilinear *newRect = placeRectilinear(grb.name, rType, Rectangle(grb.llx, grb.lly, (grb.llx + grb.width), (grb.lly + grb.height)),
                        grb.legalArea, this->mGlobalAspectRatioMin, this->mGlobalAspectRatioMax, this->mGlobalUtilizationMin);

        nameToRectilinear[grb.name] = newRect;
    }

    // create Connections
    for(int i = 0; i < mConnectionCount; ++i){
        GlobalResultConnection grc = gr.connections[i];
        std::vector<Rectilinear *> connVertices;
        for(std::string str : grc.vertices){
            connVertices.push_back(nameToRectilinear[str]);
        }

        this->allConnections.push_back(Connection(connVertices, grc.weight));
    }
}

Floorplan::Floorplan(const Floorplan &other){

    this->mChipContour = Rectangle(other.mChipContour);
    this->mAllRectilinearCount = other.mAllRectilinearCount;
    this->mSoftRectilinearCount = other.mSoftRectilinearCount;
    this->mHardRectilinearCount = other.mHardRectilinearCount;
    this->mPreplacedRectilinearCount = other.mPreplacedRectilinearCount;

    this->mConnectionCount = other.mConnectionCount;

    this->mGlobalAspectRatioMin = other.mGlobalAspectRatioMin;
    this->mGlobalAspectRatioMax = other.mGlobalAspectRatioMax;
    this->mGlobalUtilizationMin = other.mGlobalUtilizationMin;

    this->cs = CornerStitching(other.cs);

    // build maps to assist copy
    std::unordered_map<Rectilinear *, Rectilinear*> rectMap;
    std::unordered_map<Tile *, Tile *> tileMap;

    for(Rectilinear *oldRect : other.allRectilinears){
        Rectilinear *nR = new Rectilinear(*oldRect);

        // re-consruct the block tiles pointers using the new CornerStitching System
        nR->blockTiles.clear();
        for(Tile *oldT : oldRect->blockTiles){
            Tile *newT = this->cs.findPoint(oldT->getLowerLeft());
            tileMap[oldT] = newT;
            nR->blockTiles.insert(newT);
        }

        // re-consruct the overlap tiles pointers using the new CornerStitching System
        nR->overlapTiles.clear();
        for(Tile *oldT : oldRect->overlapTiles){
            Tile *newT = this->cs.findPoint(oldT->getLowerLeft());
            tileMap[oldT] = newT;
            nR->overlapTiles.insert(newT);
        }

        rectMap[oldRect] = nR;
    }
    // rework pointers for rectilinear vectors to point to new CornerStitching System
    this->allRectilinears.clear();
    this->softRectilinears.clear();
    this->hardRectilinears.clear();
    this->preplacedRectilinears.clear();

    for(Rectilinear *oldR : other.allRectilinears){
        Rectilinear *newR = rectMap[oldR];
        this->allRectilinears.push_back(rectMap[oldR]);
        
        // categorize types
        switch (newR->getType()){
        case rectilinearType::SOFT:
            this->softRectilinears.push_back(newR);
            break;
        case rectilinearType::HARD:
            this->hardRectilinears.push_back(newR);
            break;
        case rectilinearType::PREPLACED:
            this->preplacedRectilinears.push_back(newR);
            break;
        default:
            break;
        }
    }

    // rebuild connections
    this->allConnections.clear();
    for(Connection cn : other.allConnections){
        Connection newCN = Connection(cn);
        newCN.vertices.clear();
        for(Rectilinear *oldRT : cn.vertices){
            newCN.vertices.push_back(rectMap[oldRT]);
        }
        this->allConnections.push_back(newCN);
    }

    // rebuid Tile payloads section  
    this->blockTilePayload.clear();
    for(std::unordered_map<Tile *, Rectilinear *>::const_iterator it = other.blockTilePayload.begin(); it != other.blockTilePayload.end(); ++it){
        Tile *nT = tileMap[it->first];
        Rectilinear *nR = rectMap[it->second];

        this->blockTilePayload[nT] = nR;
    }

    this->overlapTilePayload.clear();
    for(std::unordered_map<Tile *, std::vector<Rectilinear *>>::const_iterator it = other.overlapTilePayload.begin(); it != other.overlapTilePayload.end(); ++it){
        Tile *nT = tileMap[it->first];
        std::vector<Rectilinear *> nRectVec;
        for(Rectilinear *oldR : it->second){
            nRectVec.push_back(rectMap[oldR]);
        }
        
        this->overlapTilePayload[nT] = nRectVec;
    }
    
}

Floorplan::~Floorplan(){
    for(Rectilinear *rt : this->allRectilinears){
        delete(rt);
    }
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

void Floorplan::setGlobalAspectRatioMin(int globalAspectRatioMin){
    this->mGlobalAspectRatioMin = globalAspectRatioMin;
}
void Floorplan::setGlobalAspectRatioMax(int globalAspectRatioMax){
    this->mGlobalAspectRatioMax = globalAspectRatioMax;

}
void Floorplan::setGlobalUtilizationMin(int globalUtilizationMin){
    this->mGlobalUtilizationMin = globalUtilizationMin;
}




double Floorplan::calculateHPWL(){
    double floorplanHPWL = 0;
    for(Connection c : this->allConnections){
        floorplanHPWL += c.calculateCost();
    }
    
    return floorplanHPWL;
}