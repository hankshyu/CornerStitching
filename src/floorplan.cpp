#include <assert.h>
#include <algorithm>
#include <fstream>

#include "boost/polygon/polygon.hpp"
#include "floorplan.h"
#include "cSException.h"
#include "doughnutPolygon.h"
#include "doughnutPolygonSet.h"

Rectilinear *Floorplan::placeRectilinear(std::string name, rectilinearType type, Rectangle placement, area_t legalArea, double aspectRatioMin, double aspectRatioMax, double mUtilizationMin){
    if(!rec::isContained(mChipContour,placement)){
        throw CSException("FLOORPLAN_16");
    }

    // register the Rectilinear container into the floorplan data structure
    Rectilinear *newRect = new Rectilinear(mIDCounter++, name, type, placement, legalArea, aspectRatioMin, aspectRatioMax, mUtilizationMin); 
    this->allRectilinears.push_back(newRect);
    switch (type){
    case rectilinearType::SOFT:
        this->softRectilinears.push_back(newRect);
        break;
    case rectilinearType::PREPLACED:
        this->preplacedRectilinears.push_back(newRect);
        break;
    default:
        break;
    }

    std::vector<Tile *> lappingTiles;
    cs->enumerateDirectedArea(placement, lappingTiles);
    if(lappingTiles.empty()){
        // placement of the rectilinear is in an area whether no other tiles are present
        addBlockTile(placement, newRect);
    }else{

        using namespace boost::polygon::operators;
        DoughnutPolygonSet insertSet;
        insertSet += placement;

        // process all overlap parts with other tles
        for(int i = 0; i < lappingTiles.size(); ++i){
            Tile *lapTile = lappingTiles[i];

            DoughnutPolygonSet origTileSet;
            origTileSet += lapTile->getRectangle();

            DoughnutPolygonSet overlapSet;
            boost::polygon::assign(overlapSet, insertSet & origTileSet);

            if(boost::polygon::equivalence(origTileSet, overlapSet)){
                // the insertion of such piece would not cause new fragments
                increaseTileOverlap(lapTile, newRect);
            }else{
                // the insertion has overlap with some existing tiles
                // 1. remove the original whole block 
                // 2. place the intersection part
                // 3. place the rest part, dice into small rectangles if necessary

                tileType lapTileType = lapTile->getType();
                if(lapTileType == tileType::BLOCK){
                    Rectilinear *origPayload = this->blockTilePayload[lapTile];

                    deleteTile(lapTile);

                    // add the intersection part first
                    std::vector<Rectangle> intersectRect;
                    dps::diceIntoRectangles(overlapSet, intersectRect);
                    assert(intersectRect.size() == 1);
                    addOverlapTile(intersectRect[0], std::vector<Rectilinear *>({origPayload, newRect}));

                    // process the remains of the overlapSet
                    origTileSet -= overlapSet;
                    std::vector<Rectangle> restRect;
                    dps::diceIntoRectangles(origTileSet, restRect);
                    for(Rectangle const &rt : restRect){
                        addBlockTile(rt, origPayload);
                    }
                }else if(lapTileType == tileType::OVERLAP){
                    std::vector<Rectilinear *> origPaylaod = this->overlapTilePayload[lapTile];

                    deleteTile(lapTile);

                    // add the intersection part first
                    std::vector<Rectangle> intersectRect;
                    dps::diceIntoRectangles(overlapSet, intersectRect);
                    assert(intersectRect.size() == 1);
                    std::vector<Rectilinear *> newPayload(origPaylaod);
                    newPayload.push_back(newRect);
                    addOverlapTile(intersectRect[0], newPayload);

                    // process the remains of the overlapSet
                    origTileSet -= overlapSet;
                    std::vector<Rectangle> restRect;
                    dps::diceIntoRectangles(origTileSet, restRect);
                    for(Rectangle const &rt : restRect){
                        addOverlapTile(rt, origPaylaod);
                    }

                }else{
                    throw CSException("FLOORPLAN_16");
                }
                
            }

            // removed the processed part from insertSet
            insertSet -= overlapSet;
        }

        std::vector<Rectangle> remainInsertRect;
        dps::diceIntoRectangles(insertSet, remainInsertRect);
        for(const Rectangle &rt : remainInsertRect){
            addBlockTile(rt, newRect);
        }
    }

    return newRect;
}

Floorplan::Floorplan()
    : mChipContour(Rectangle(0, 0, 0, 0)) , mAllRectilinearCount(0), mSoftRectilinearCount(0), mPreplacedRectilinearCount(0), mConnectionCount(0) {
}

Floorplan::Floorplan(GlobalResult gr, double aspectRatioMin, double aspectRatioMax, double utilizationMin)
    : mGlobalAspectRatioMin(aspectRatioMin), mGlobalAspectRatioMax(aspectRatioMax), mGlobalUtilizationMin(utilizationMin) {

    mChipContour = Rectangle(0, 0, gr.chipWidth, gr.chipHeight);
    mAllRectilinearCount = gr.blockCount;
    mConnectionCount = gr.connectionCount;

    cs = new CornerStitching(gr.chipWidth, gr.chipHeight);
    
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
            rType = rectilinearType::PREPLACED;
        }else{
            throw CSException("FLOORPLAN_01");
        }

        Rectilinear *newRect = placeRectilinear(grb.name, rType, Rectangle(grb.llx, grb.lly, (grb.llx + grb.width), (grb.lly + grb.height)),
                        grb.legalArea, this->mGlobalAspectRatioMin, this->mGlobalAspectRatioMax, this->mGlobalUtilizationMin);

        nameToRectilinear[grb.name] = newRect;
    }
    // Reshape all Rectilinears after insertion complete, order of insertion causes excessive splitting
    for(Rectilinear* const &rect : this->allRectilinears){
        reshapeRectilinear(rect);
    }
    
    // create Connections
    for(int i = 0; i < mConnectionCount; ++i){
        GlobalResultConnection grc = gr.connections[i];
        std::vector<Rectilinear *> connVertices;
        for(std::string const &str : grc.vertices){
            connVertices.push_back(nameToRectilinear[str]);
        }

        this->allConnections.push_back(Connection(connVertices, grc.weight));
    }
}

Floorplan::Floorplan(const Floorplan &other){

    // copy basic attributes
    this->mChipContour = Rectangle(other.mChipContour);
    this->mAllRectilinearCount = other.mAllRectilinearCount;
    this->mSoftRectilinearCount = other.mSoftRectilinearCount;
    // this->mHardRectilinearCount = other.mHardRectilinearCount;
    this->mPreplacedRectilinearCount = other.mPreplacedRectilinearCount;

    this->mConnectionCount = other.mConnectionCount;

    this->mGlobalAspectRatioMin = other.mGlobalAspectRatioMin;
    this->mGlobalAspectRatioMax = other.mGlobalAspectRatioMax;
    this->mGlobalUtilizationMin = other.mGlobalUtilizationMin;

    this->cs = new CornerStitching(*other.cs);

    // build maps to assist copy
    std::unordered_map<Rectilinear *, Rectilinear*> rectMap;
    std::unordered_map<Tile *, Tile *> tileMap;

    for(Rectilinear *const &oldRect : other.allRectilinears){
        Rectilinear *nR = new Rectilinear(*oldRect);

        // re-consruct the block tiles pointers using the new CornerStitching System
        nR->blockTiles.clear();
        for(Tile *const &oldT : oldRect->blockTiles){
            Tile *newT = this->cs->findPoint(oldT->getLowerLeft());
            tileMap[oldT] = newT;
            nR->blockTiles.insert(newT);
        }

        // re-consruct the overlap tiles pointers using the new CornerStitching System
        nR->overlapTiles.clear();
        for(Tile *const &oldT : oldRect->overlapTiles){
            Tile *newT = this->cs->findPoint(oldT->getLowerLeft());
            tileMap[oldT] = newT;
            nR->overlapTiles.insert(newT);
        }

        rectMap[oldRect] = nR;
    }
    // rework pointers for rectilinear vectors to point to new CornerStitching System
    this->allRectilinears.clear();
    this->softRectilinears.clear();
    this->preplacedRectilinears.clear();

    for(Rectilinear *const &oldR : other.allRectilinears){
        Rectilinear *newR = rectMap[oldR];
        this->allRectilinears.push_back(rectMap[oldR]);
        
        // categorize types
        switch (newR->getType()){
        case rectilinearType::SOFT:
            this->softRectilinears.push_back(newR);
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
    for(Connection const &cn : other.allConnections){
        Connection newCN = Connection(cn);
        newCN.vertices.clear();
        for(Rectilinear *const &oldRT : cn.vertices){
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
        for(Rectilinear *const &oldR : it->second){
            nRectVec.push_back(rectMap[oldR]);
        }
        
        this->overlapTilePayload[nT] = nRectVec;
    }
    
}

Floorplan::~Floorplan(){
    for(Rectilinear *&rt : this->allRectilinears){
        delete(rt);
    }

    delete(cs);
}

Floorplan &Floorplan::operator = (const Floorplan &other){
    // copy basic attributes
    this->mChipContour = Rectangle(other.mChipContour);
    this->mAllRectilinearCount = other.mAllRectilinearCount;
    this->mSoftRectilinearCount = other.mSoftRectilinearCount;
    // this->mHardRectilinearCount = other.mHardRectilinearCount;
    this->mPreplacedRectilinearCount = other.mPreplacedRectilinearCount;

    this->mConnectionCount = other.mConnectionCount;

    this->mGlobalAspectRatioMin = other.mGlobalAspectRatioMin;
    this->mGlobalAspectRatioMax = other.mGlobalAspectRatioMax;
    this->mGlobalUtilizationMin = other.mGlobalUtilizationMin;

    this->cs = new CornerStitching(*other.cs);

    // build maps to assist copy
    std::unordered_map<Rectilinear *, Rectilinear*> rectMap;
    std::unordered_map<Tile *, Tile *> tileMap;

    for(Rectilinear *const &oldRect : other.allRectilinears){
        Rectilinear *nR = new Rectilinear(*oldRect);

        // re-consruct the block tiles pointers using the new CornerStitching System
        nR->blockTiles.clear();
        for(Tile *const &oldT : oldRect->blockTiles){
            Tile *newT = this->cs->findPoint(oldT->getLowerLeft());
            tileMap[oldT] = newT;
            nR->blockTiles.insert(newT);
        }

        // re-consruct the overlap tiles pointers using the new CornerStitching System
        nR->overlapTiles.clear();
        for(Tile *const &oldT : oldRect->overlapTiles){
            Tile *newT = this->cs->findPoint(oldT->getLowerLeft());
            tileMap[oldT] = newT;
            nR->overlapTiles.insert(newT);
        }

        rectMap[oldRect] = nR;
    }
    // rework pointers for rectilinear vectors to point to new CornerStitching System
    this->allRectilinears.clear();
    this->softRectilinears.clear();
    this->preplacedRectilinears.clear();

    for(Rectilinear *const &oldR : other.allRectilinears){
        Rectilinear *newR = rectMap[oldR];
        this->allRectilinears.push_back(rectMap[oldR]);
        
        // categorize types
        switch (newR->getType()){
        case rectilinearType::SOFT:
            this->softRectilinears.push_back(newR);
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
    for(Connection const &cn : other.allConnections){
        Connection newCN = Connection(cn);
        newCN.vertices.clear();
        for(Rectilinear *const &oldRT : cn.vertices){
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
        for(Rectilinear *const &oldR : it->second){
            nRectVec.push_back(rectMap[oldR]);
        }
        
        this->overlapTilePayload[nT] = nRectVec;
    }

    return (*this);
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

int Floorplan::getPreplacedRectilinearCount() const {
    return this->mPreplacedRectilinearCount;
}

int Floorplan::getConnectionCount() const {
    return this->mConnectionCount;
}


double Floorplan::getGlobalAspectRatioMin() const {
    return this->mGlobalAspectRatioMin;
}
double Floorplan::getGlobalAspectRatioMax() const {
    return this->mGlobalAspectRatioMax;
}
double Floorplan::getGlobalUtilizationMin() const {
    return this->mGlobalUtilizationMin;
}

void Floorplan::setGlobalAspectRatioMin(double globalAspectRatioMin){
    this->mGlobalAspectRatioMin = globalAspectRatioMin;
}
void Floorplan::setGlobalAspectRatioMax(double globalAspectRatioMax){
    this->mGlobalAspectRatioMax = globalAspectRatioMax;

}
void Floorplan::setGlobalUtilizationMin(double globalUtilizationMin){
    this->mGlobalUtilizationMin = globalUtilizationMin;
}

Tile *Floorplan::addBlockTile(const Rectangle &tilePosition, Rectilinear *rt){
    if(!rec::isContained(this->mChipContour, tilePosition)){
        throw CSException("FLOORPLAN_02");
    }

    if(std::find(allRectilinears.begin(), allRectilinears.end(), rt) == allRectilinears.end()){
        throw CSException("FLOORPLAN_03");
    }

    // use the prototype to insert the tile onto the cornerstitching system, receive the actual pointer as return
    Tile tilePrototype(tileType::BLOCK, tilePosition);
    Tile *newTile = cs->insertTile(tilePrototype);
    // register the pointer to the rectilienar system 
    rt->blockTiles.insert(newTile);
    // connect tile's payload as the rectilinear on the floorplan system 
    this->blockTilePayload[newTile] = rt;

    return newTile;
}

Tile *Floorplan::addOverlapTile(const Rectangle &tilePosition, const std::vector<Rectilinear*> &payload){
    if(!rec::isContained(this->mChipContour, tilePosition)){
        throw CSException("FLOORPLAN_04");
    }

    for(Rectilinear *const &rt : payload){
        if(std::find(allRectilinears.begin(), allRectilinears.end(), rt) == allRectilinears.end()){
            throw CSException("FLOORPLAN_05");
        }
    }

    // use the prototype to insert the tile onto the cornerstitching system, receive the actual pointer as return
    Tile tilePrototype(tileType::OVERLAP, tilePosition);
    Tile *newTile = cs->insertTile(tilePrototype);
    // register the pointer to all Rectilinears
    for(Rectilinear *const rt : payload){
        rt->overlapTiles.insert(newTile);
    }

    // connect tile's payload 
    this->overlapTilePayload[newTile] = std::vector<Rectilinear *>(payload);

    return newTile;
}

void Floorplan::deleteTile(Tile *tile){
    tileType toDeleteType = tile->getType();
    if(!((toDeleteType == tileType::BLOCK) || (toDeleteType == tileType::OVERLAP))){
        throw CSException("FLOORPLAN_06");
    }
    // clean-up the payload information stored inside the floorplan system
    if(toDeleteType == tileType::BLOCK){
        std::unordered_map<Tile *, Rectilinear *>::iterator blockIt= this->blockTilePayload.find(tile);
        if(blockIt == this->blockTilePayload.end()){
            throw CSException("FLOORPLAN_07");
        }
        // erase the tile from the rectilinear structure
        Rectilinear *rt = blockIt->second;
        rt->blockTiles.erase(tile);
        // erase the payload from the floorplan structure
        this->blockTilePayload.erase(tile);
    }else{
        std::unordered_map<Tile *, std::vector<Rectilinear *>>::iterator overlapIt = this->overlapTilePayload.find(tile);
        if(overlapIt == this->overlapTilePayload.end()){
            throw CSException("FLOORPLAN_08");
        }

        // erase the tiles from the rectiliear structures
        for(Rectilinear *const rt : overlapIt->second){
            rt->overlapTiles.erase(tile);
        }

        // erase the payload from the floorplan structure
        this->overlapTilePayload.erase(tile);
    }

    // remove the tile from the cornerStitching structure
    cs->removeTile(tile);
}

void Floorplan::increaseTileOverlap(Tile *tile, Rectilinear *newRect){
    tileType increaseTileType = tile->getType();
    if(increaseTileType == tileType::BLOCK){
        // check if the tile is present in the floorplan structure
        std::unordered_map<Tile *, Rectilinear *>::iterator blockIt = this->blockTilePayload.find(tile);
        if(blockIt == this->blockTilePayload.end()){
            throw CSException("FLOORPLAN_09");
        }

        if(blockIt->second == newRect){
            throw CSException("FLOORPLAN_20");
        }

        Rectilinear *oldRect = blockIt->second;
        // erase record at floorplan system and rectilinear system
        this->blockTilePayload.erase(tile);
        oldRect->blockTiles.erase(tile);

        // change the tile's type attribute 
        tile->setType(tileType::OVERLAP);

        // refill correct information for floorplan system and rectilinear
        this->overlapTilePayload[tile] = {oldRect, newRect};
        oldRect->overlapTiles.insert(tile);
        newRect->overlapTiles.insert(tile);

    }else if(increaseTileType == tileType::OVERLAP){
        std::unordered_map<Tile *, std::vector<Rectilinear *>>::iterator overlapIt = this->overlapTilePayload.find(tile);
        if(overlapIt == this->overlapTilePayload.end()){
            throw CSException("FLOORPLAN_10");
        }

        if(std::find(overlapIt->second.begin(), overlapIt->second.end(), newRect) != overlapIt->second.end()){
            throw CSException("FLOORPLAN_20");
        }
        
        // update newRect's rectilienar structure and register newRect as tile's payload at floorplan system
        newRect->overlapTiles.insert(tile);
        this->overlapTilePayload[tile].push_back(newRect);

    }else{
        throw CSException("FLOORPLAN_11");
    }
}

void Floorplan::decreaseTileOverlap(Tile *tile, Rectilinear *removeRect){
    if(tile->getType() != tileType::OVERLAP){
        throw CSException("FLOORPLAN_12");
    }

    if(this->overlapTilePayload.find(tile) == this->overlapTilePayload.end()){
        throw CSException("FLOORPLAN_13");
    }

    std::vector<Rectilinear *> *oldPayload = &(this->overlapTilePayload[tile]);
    if(std::find(oldPayload->begin(), oldPayload->end(), removeRect) == oldPayload->end()){
        throw CSException("FLOORPLAN_14");
    }
    int oldPayloadSize = oldPayload->size();
    if(oldPayloadSize == 2){
        // ready to change tile's type to tileType::BLOCK
        Rectilinear *solePayload = (((*oldPayload)[0]) == removeRect)? ((*oldPayload)[1]) : ((*oldPayload)[0]);
        // remove tile payload from floorplan structure
        this->overlapTilePayload.erase(tile);

        // remove from rectilinear structure
        removeRect->overlapTiles.erase(tile);
        solePayload->overlapTiles.erase(tile);

        // change type of the tile
        tile->setType(tileType::BLOCK);

        solePayload->blockTiles.insert(tile);
        this->blockTilePayload[tile] = solePayload;
    }else if(oldPayloadSize > 2){
        // the tile has 2 or more rectilinear after removal, keep type as tileType::OVERLAP

        std::vector<Rectilinear *> *toChange = &(this->overlapTilePayload[tile]);
        // apply erase-remove idiom to delete removeRect from the payload from floorplan 
        toChange->erase(std::remove(toChange->begin(), toChange->end(), removeRect));

        removeRect->overlapTiles.erase(tile);

    }else{
        throw CSException("FLOORPLAN_15");
    }
}

void Floorplan::reshapeRectilinear(Rectilinear *rt){
    using namespace boost::polygon::operators;
    DoughnutPolygonSet reshapePart;
    std::unordered_map<Rectangle, Tile*> rectanglesToDelete;

    for(Tile *const &blockTile : rt->blockTiles){
        Rectangle blockRectangle = blockTile->getRectangle();
        rectanglesToDelete[blockRectangle] = blockTile;
        reshapePart += blockRectangle;
    }
    
    std::vector<Rectangle> diceResult;
    dps::diceIntoRectangles(reshapePart, diceResult);
    std::unordered_set<Rectangle> rectanglesToAdd;

    // extract those rectangles that should be deleted and those that should stay
    for(Rectangle const &diceRect : diceResult){
        if(rectanglesToDelete.find(diceRect) != rectanglesToDelete.end()){
            // old Rectilinear already exist such tile
            rectanglesToDelete.erase(diceRect); 
        }else{
            // new tile for the rectilinear
            rectanglesToAdd.insert(diceRect);
        }
    }

    for(std::unordered_map<Rectangle, Tile*>::iterator it = rectanglesToDelete.begin(); it != rectanglesToDelete.end(); ++it){
        deleteTile(it->second);
    }

    for(Rectangle const &toAddRect : rectanglesToAdd){
        addBlockTile(toAddRect, rt);
    }

}

Tile *Floorplan::divideTileHorizontally(Tile *origTop, len_t newDownHeight){
    switch (origTop->getType()){
    case tileType::BLOCK:{
        Rectilinear *origTopBelongRect = this->blockTilePayload[origTop];
        Tile *newDown = cs->cutTileHorizontally(origTop, newDownHeight);
        origTopBelongRect->blockTiles.insert(newDown);
        this->blockTilePayload[newDown] = origTopBelongRect;
        return newDown;
        break;
    }
    case tileType::OVERLAP:{
        std::vector<Rectilinear *> origTopContainedRect(this->overlapTilePayload[origTop]);
        Tile *newDown = cs->cutTileHorizontally(origTop, newDownHeight);
        for(Rectilinear *const &rect : origTopContainedRect){
            rect->overlapTiles.insert(newDown);
        }
        this->overlapTilePayload[newDown] = origTopContainedRect;
        return newDown;
        break;
    }
    default:
        throw CSException("FLOORPLAN_18");
        break;
    }
}

Tile *Floorplan::divideTileVertically(Tile *origRight, len_t newLeftWidth){
    switch (origRight->getType()){
    case tileType::BLOCK:{
        Rectilinear *origTopBelongRect = this->blockTilePayload[origRight];
        Tile *newDown = cs->cutTileVertically(origRight, newLeftWidth);
        origTopBelongRect->blockTiles.insert(newDown);
        this->blockTilePayload[newDown] = origTopBelongRect;
        return newDown;
        break;
    }
    case tileType::OVERLAP:{
        std::vector<Rectilinear *> origTopContainedRect(this->overlapTilePayload[origRight]);
        Tile *newDown = cs->cutTileVertically(origRight, newLeftWidth);
        for(Rectilinear *const &rect : origTopContainedRect){
            rect->overlapTiles.insert(newDown);
        }
        this->overlapTilePayload[newDown] = origTopContainedRect;
        return newDown;
        break;
    }
    default:
        throw CSException("FLOORPLAN_19");
        break;
    }

}

double Floorplan::calculateHPWL(){
    double floorplanHPWL = 0;
    for(Connection const &c : this->allConnections){
        floorplanHPWL += c.calculateCost();
    }
    
    return floorplanHPWL;
}

void Floorplan::visualiseLegalFloorplan(const std::string &outputFileName) const {
    using namespace boost::polygon::operators;
	std::ofstream ofs;
	ofs.open(outputFileName, std::fstream::out);
	if(!ofs.is_open()){
        throw(CSException("CORNERSTITCHING_21"));
    } 

    ofs << "BLOCK " << mAllRectilinearCount << std::endl;
    ofs << rec::getWidth(mChipContour) << " " << rec::getHeight(mChipContour) << std::endl;

    ofs << "SOFTBLOCK " << mSoftRectilinearCount << std::endl;
    for(Rectilinear *const &rect : softRectilinears){
        DoughnutPolygonSet dps;
        if(!rect->overlapTiles.empty()){
            throw(CSException("CORNERSTITCHING_22"));
        }
        for(Tile *const &t : rect->blockTiles){
            dps += t->getRectangle();
        }

        if(dps.size() != 1){
            throw(CSException("CORNERSTITCHING_23"));
        }
        DoughnutPolygon dp = dps[0];

        boost::polygon::direction_1d direction = boost::polygon::winding(dp);
        ofs << rect->getName() << " " << dp.size() << std::endl;  
        
        if(direction == boost::polygon::direction_1d_enum::CLOCKWISE){
            for(auto it = dp.begin(); it != dp.end(); ++it){
                ofs << (*it).x() << (*it).y() << std::endl;
            }
        }else{
            std::vector<Cord> buffer;
            for(auto it = dp.begin(); it != dp.end(); ++it){
                buffer.push_back(*it);
            }
            for(std::vector<Cord>::reverse_iterator it = buffer.rbegin(); it != buffer.rend(); ++it){
                ofs << (*it).x() << (*it).y() << std::endl;
            }
        }
    }

    ofs << "PREPLACEDBLOCK " << mPreplacedRectilinearCount << std::endl;
    for(Rectilinear *const &rect : preplacedRectilinears){
        DoughnutPolygonSet dps;
        if(!rect->overlapTiles.empty()){
            throw(CSException("CORNERSTITCHING_22"));
        }
        for(Tile *const &t : rect->blockTiles){
            dps += t->getRectangle();
        }

        if(dps.size() != 1){
            throw(CSException("CORNERSTITCHING_23"));
        }
        DoughnutPolygon dp = dps[0];

        boost::polygon::direction_1d direction = boost::polygon::winding(dp);
        ofs << rect->getName() << " " << dp.size() << std::endl;  
        
        if(direction == boost::polygon::direction_1d_enum::CLOCKWISE){
            for(auto it = dp.begin(); it != dp.end(); ++it){
                ofs << (*it).x() << (*it).y() << std::endl;
            }
        }else{
            std::vector<Cord> buffer;
            for(auto it = dp.begin(); it != dp.end(); ++it){
                buffer.push_back(*it);
            }
            for(std::vector<Cord>::reverse_iterator it = buffer.rbegin(); it != buffer.rend(); ++it){
                ofs << (*it).x() << (*it).y() << std::endl;
            }
        }
    }

    ofs << "CONNECTION " << mConnectionCount << std::endl;
    for(Connection const &conn : allConnections){
        for(Rectilinear *const &rect : conn.vertices){
            ofs << rect->getName() << " ";
        }
        ofs << conn.weight << std::endl; 
    }
}

size_t std::hash<Floorplan>::operator()(const Floorplan&key) const {
    return std::hash<Rectangle>()(key.getChipContour()) ^ std::hash<int>()(key.getAllRectilinearCount()) ^ std::hash<int>()(key.getConnectionCount());
}

