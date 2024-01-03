#include "cornerStitching.h"
#include "cSException.h"

bool CornerStitching::checkPointInCanvas(const Cord &point) const{
    return mCanvasSizeBlankTile->checkCordInTile(point);
}

bool CornerStitching::checkRectangleInCanvas(const Rectangle &rect) const{

    bool realLLIn = mCanvasSizeBlankTile->checkCordInTile(boost::polygon::ll(rect));

    len_t containedURX = boost::polygon::xh(rect) - 1;
    len_t containedURY = boost::polygon::yh(rect) - 1;
    bool realURIn = mCanvasSizeBlankTile->checkCordInTile(Cord(containedURX, containedURY));

    return realLLIn && realURIn;
}

void CornerStitching::collectAllTiles(std::unordered_set<Tile *> &allTiles) const{
    allTiles.clear();
    if(allNonBlankTilesMap.empty()){
        allTiles.insert(this->mCanvasSizeBlankTile);
        return;
    }

    // call herlper function, Use DFS to travese the entire graph
    // and put a random tile as seed
    collectAllTilesDFS(allNonBlankTilesMap.begin()->second, allTiles);
}

void CornerStitching::collectAllTilesDFS(Tile *currentSearch, std::unordered_set<Tile *> &allTiles) const{
    
    allTiles.insert(currentSearch);
    
    if(currentSearch->rt != nullptr){
        if(allTiles.find(currentSearch) != allTiles.end()){
            collectAllTilesDFS(currentSearch->rt, allTiles);
        }
    }
    if(currentSearch->tr != nullptr){
        if(allTiles.find(currentSearch) != allTiles.end()){
            collectAllTilesDFS(currentSearch->tr, allTiles);
        }
    }
    if(currentSearch->bl != nullptr){
        if(allTiles.find(currentSearch) != allTiles.end()){
            collectAllTilesDFS(currentSearch->bl, allTiles);
        }
    }
    if(currentSearch->lb != nullptr){
        if(allTiles.find(currentSearch) != allTiles.end()){
            collectAllTilesDFS(currentSearch->lb, allTiles);
        }
    }
}

CornerStitching::CornerStitching(len_t chipWidth, len_t chipHeight)
    : mCanvasWidth(chipWidth), mCanvasHeight(chipHeight) 
    {
        mCanvasSizeBlankTile = new Tile(tileType::BLANK, Cord(0, 0), chipWidth, chipHeight);
    }

CornerStitching::CornerStitching(const CornerStitching &other){
    this->mCanvasWidth = other.mCanvasWidth;
    this->mCanvasHeight = other.mCanvasHeight;
    this->mCanvasSizeBlankTile = new Tile(*(other.mCanvasSizeBlankTile));
    
    if(other.allNonBlankTilesMap.size() == 0) return;

    std::unordered_set <Tile *> oldAllTiles;
    other.collectAllTiles(oldAllTiles);

    std::unordered_map <Tile *, Tile *> oldNewPairs;
    for(Tile *oldTile : oldAllTiles){
        oldNewPairs[oldTile] = new Tile(*oldTile);
    }

    // maintain the pointers of the new Tiles, while simultanuously maintain allNonBlankTilesMap
    for(auto pairs : oldNewPairs){

        Tile *father = pairs.first;
        Tile *son = pairs.second;

        // insert to allNonBlankTilesMap if tile is not BLANK
        if(son->getType() != tileType::BLANK){
            this->allNonBlankTilesMap[son->getLowerLeft()] = son;
        }

        // maintain the links using the map data-structure
        son->rt = oldNewPairs[father->rt];
        son->tr = oldNewPairs[father->tr];
        son->bl = oldNewPairs[father->bl];
        son->lb = oldNewPairs[father->lb];
    }

}

CornerStitching::~CornerStitching(){
    delete(mCanvasSizeBlankTile);

    // If there is no BLOCK or OVERLAP tiles, remove the only blank tile and return
    if(allNonBlankTilesMap.size() == 0) return;
    
    std::unordered_set<Tile *> allOldTiles;
    collectAllTiles(allOldTiles);
    for(Tile *oldTiles : allOldTiles){
        delete(oldTiles);
    }

}

len_t CornerStitching::getCanvasWidth() const{
    return this->mCanvasWidth;
}

len_t CornerStitching::getCanvasHeight() const{
    return this->mCanvasHeight;
}

Tile *CornerStitching::findPoint(const Cord &key) const{

    // throw exception if point finding (key) out of canvas range
    if(checkPointInCanvas(key)){
        throw CSException("CORNERSTITCHING_01");
    }

    // Find a seed to start, if empty just return the blank tile.
    if(allNonBlankTilesMap.empty()){
        return this->mCanvasSizeBlankTile;
    }

    Tile *index = allNonBlankTilesMap.begin()->second;

    while(!(index->checkCordInTile(key))){
        if(!index->checkYCordInTile(key)){
            // Adjust vertical range
            if(key.y() >= index->getLowerLeft().y()){
                assert(index->rt != nullptr);
                index = index->rt;
            }else{
                assert(index->lb != nullptr);
                index = index->lb;
            }
        }else{
            // Vertical range correct! adjust horizontal range
            if(key.x() >= index->getLowerLeft().x()){
                assert(index->tr != nullptr);
                index = index->tr;
            }else{
                assert(index->bl != nullptr);
                index = index->bl;
            }
        }
    }
    
    return index;
}   

void CornerStitching::findTopNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const{
    if(centre == nullptr) return;
    if(centre->rt == nullptr) return;

    Tile *n = centre->rt;
    while(n->getLowerLeft().x() > centre->getLowerLeft().x()){
        neighbors.push_back(n);
        n = n->bl;
    }
    neighbors.push_back(n);
}

void CornerStitching::findDownNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const{
    if(centre == nullptr) return;
    if(centre->lb == nullptr) return;

    Tile *n = centre->lb;
    while(n->getUpperRight().x() < centre->getUpperRight().x()){
        neighbors.push_back(n);
        n = n->tr;
    }
    neighbors.push_back(n);
}

void CornerStitching::findLeftNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const{
    if(centre == nullptr) return;
    if(centre->bl == nullptr) return;

    Tile *n = centre->bl;
    while(n->getUpperRight().y() < centre->getUpperRight().y()){
        neighbors.push_back(n);
        n = n->rt;
    }
    neighbors.push_back(n);
}

void CornerStitching::findRightNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const{
    if(centre == nullptr) return;
    if(centre->tr == nullptr) return;

    Tile *n = centre->tr;
    // the last neighbor is the first tile encountered whose lower y cord <= lower y cord of starting tile
    while(n->getLowerLeft().y() > centre->getLowerLeft().y()){
        neighbors.push_back(n);
        n = n->lb;
    }
    neighbors.push_back(n);
}

void CornerStitching::findAllNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const{
    if(centre == nullptr) return;

    findTopNeighbors(centre, neighbors);
    findLeftNeighbors(centre, neighbors);
    findDownNeighbors(centre, neighbors);
    findRightNeighbors(centre, neighbors);
}

bool CornerStitching::searchArea(Rectangle box) const{

    if(!checkRectangleInCanvas(box)){
        throw CSException("CORNERSTITCHING_02");
    }


    // Use point-finding algo to locate the tile containin the upperleft corner of AOI
    len_t searchRBorderHeight = boost::polygon::yh(box) - 1;
    Tile *currentFind = findPoint(Cord(boost::polygon::xl(box), searchRBorderHeight));

    while(currentFind->getUpperLeft().y() > boost::polygon::yl(box)){
        // check if the tile is solid
        if(currentFind->getType() != tileType::BLANK){
            // This is an edge of a solid tile
            return true;
        }else if(currentFind->getUpperRight().x() < boost::polygon::xh(box)){
            // See if the right edge within AOI, right must be a tile
            return true;
        }

        // Move down to the next tile touching the left edge of AOI
        if(currentFind->getLowerLeft().y() < 1) break;

        currentFind = findPoint(Cord(boost::polygon::xl(box), currentFind->getLowerLeft().y() - 1));
    }

    return false;

}

bool CornerStitching::searchArea(Rectangle box, Tile *target) const{

    if(!checkRectangleInCanvas(box)){
        throw CSException("CORNERSTITCHING_02");
    }

    target = nullptr;

    // Use point-finding algo to locate the tile containin the upperleft corner of AOI
    len_t searchRBorderHeight = boost::polygon::yh(box) - 1;
    Tile *currentFind = findPoint(Cord(boost::polygon::xl(box), searchRBorderHeight));

    while(currentFind->getUpperLeft().y() > boost::polygon::yl(box)){
        // check if the tile is solid
        if(currentFind->getType() != tileType::BLANK){
            // This is an edge of a solid tile
            target = currentFind;
            return true;
        }else if(currentFind->getUpperRight().x() < boost::polygon::xh(box)){
            // See if the right edge within AOI, right must be a tile
            target = (currentFind->tr);
            return true;
        }

        // Move down to the next tile touching the left edge of AOI
        if(currentFind->getLowerLeft().y() < 1) break;

        currentFind = findPoint(Cord(boost::polygon::xl(box), currentFind->getLowerLeft().y() - 1));
    }

    return false;

}

void CornerStitching::enumerateDirectArea(Rectangle box, std::vector <Tile *> &allTiles) const{

    if(!checkRectangleInCanvas(box)){
        throw CSException("CORNERSTITCHING_03");
    }

    // Use point-finding algo to locate the tile containin the upperleft corner of AOI
    len_t searchRBorderHeight = boost::polygon::yh(box) - 1;
    Tile *leftTouchTile = findPoint(Cord(boost::polygon::xl(box), searchRBorderHeight));

    while(leftTouchTile->getUpperLeft().y() > boost::polygon::yl(box)){
        enumerateDirectAreaRProcedure(box, allTiles, leftTouchTile);
        if(leftTouchTile->getLowerLeft().y() < 1) break;
        leftTouchTile = findPoint(Cord(boost::polygon::xl(box), leftTouchTile->getLowerLeft().y() - 1));
    }
}

void CornerStitching::enumerateDirectAreaRProcedure(Rectangle box, std::vector <Tile *> &allTiles, Tile *targetTile) const{
    
    // R1) Enumerate the tile
    if(targetTile->getType() == tileType::BLOCK || targetTile->getType() == tileType::OVERLAP){
        allTiles.push_back(targetTile);
    }

    // R2) If the right edge of the tile is outside of the seearch area, return
    if(targetTile->getLowerRight().x() >= boost::polygon::xh(box)) return;

    // R3) Use neighbor-finding algo to locate all the tiles that touch the right side of the current tile and also intersect the search area
    std::vector<Tile *> rightNeighbors;
    findRightNeighbors(targetTile, rightNeighbors);
    for(Tile *t : rightNeighbors){

        // R4) If bottom left corner of the neighbor touches the current tile
        Cord rightLL = t->getLowerLeft();
        Rectangle targetTileRect = targetTile->getRectangle();
        bool xAligned = (boost::polygon::xh(targetTileRect) == rightLL.x());
        bool yInRange = (rightLL.y() >= boost::polygon::yl(targetTileRect));
        bool R4 = (xAligned && yInRange);

        // R5) If the bottom edge ofthe search area cuts both the urrent tile and the neighbor
        bool R5 = (targetTile->cutHeight(lowerleft.y)) && (t->cutHeight(lowerleft.y));

        if(R4 || R5) enumerateDirectAreaRProcedure(box, allTiles, t);
    }

}