#include <fstream>
#include "cornerStitching.h"
#include "cSException.h"

bool CornerStitching::checkPointInCanvas(const Cord &point) const{
    return rec::isContained(mCanvasSizeBlankTile->getRectangle(), point);
}

bool CornerStitching::checkRectangleInCanvas(const Rectangle &rect) const{

    bool realLLIn = rec::isContained(mCanvasSizeBlankTile->getRectangle(), rec::getLL(rect));

    len_t containedURX = rec::getXH(rect) - 1;
    len_t containedURY = rec::getYH(rect) - 1;
    Cord containedUR (containedURX, containedURY);
    bool realURIn = rec::isContained(mCanvasSizeBlankTile->getRectangle(), containedUR);
    
    return realLLIn && realURIn;
}

void CornerStitching::collectAllTiles(std::unordered_set<Tile *> &allTiles) const{
    // If there are no tiles, just return the blank tile
    if(mAllNonBlankTilesMap.empty()){
        allTiles.insert(this->mCanvasSizeBlankTile);
        return;
    }

    // call herlper function, Use DFS to travese the entire graph, this funciton uses a random tile as seed.
    collectAllTilesDFS(mAllNonBlankTilesMap.begin()->second, allTiles);
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

// this function is not yet rewritten done
void CornerStitching::enumerateDirectedAreaRProcedure(Rectangle box, std::vector <Tile *> &allTiles, Tile *targetTile) const{

    // R1) Enumerate the tile
    tileType targetTileType = targetTile->getType();
    if((targetTileType == tileType::BLOCK) ||(targetTileType == tileType::OVERLAP)){
        allTiles.push_back(targetTile);
    }

    // R2) If the right edge of the tile is outside (or touch) the seearch area, return
    if(targetTile->getXHigh() >= rec::getXH(box)) return;

    // R3) Use neighbor-finding algo to locate all the tiles that touch the right side of the current tile and also intersect the search area
    std::vector<Tile *> rightNeighbors;
    findRightNeighbors(targetTile, rightNeighbors);

    Rectangle targetTileRect = targetTile->getRectangle();

    for(Tile *t : rightNeighbors){
        // make sure the tile is in the AOI
        if(!rec::hasIntersect(t->getRectangle(), box, false)) continue;

        // R4) If bottom left corner of the neighbor touches the current tile
        bool R4 = (t->getYLow() >= targetTile->getYLow());

        // R5) If the bottom edge ofthe search area cuts both the target tile and its neighbor
        len_t bottomEdge = targetTile->getYLow();
        bool cutTargetTile = (targetTile->getYLow() <= bottomEdge) && (targetTile->getYHigh() > bottomEdge);
        bool cutNeighbor = (t->getYLow() <= bottomEdge) && (t->getYHigh() > bottomEdge);
        bool R5 = cutTargetTile && cutNeighbor;

        if(R4 || R5) enumerateDirectedAreaRProcedure(box, allTiles, t);
    }

}

CornerStitching::CornerStitching(len_t chipWidth, len_t chipHeight)
    : mCanvasWidth(chipWidth), mCanvasHeight(chipHeight) 
    {
        if((chipWidth <= 0) || (chipHeight <= 0)){
            throw CSException("CORNERSTITCHING_04");
        }
        mCanvasSizeBlankTile = new Tile(tileType::BLANK, Cord(0, 0), chipWidth, chipHeight);
    }

CornerStitching::CornerStitching(const CornerStitching &other){
    this->mCanvasWidth = other.mCanvasWidth;
    this->mCanvasHeight = other.mCanvasHeight;
    this->mCanvasSizeBlankTile = new Tile(*(other.mCanvasSizeBlankTile));
    
    if(other.mAllNonBlankTilesMap.size() == 0) return;

    std::unordered_set <Tile *> oldAllTiles;
    other.collectAllTiles(oldAllTiles);

    std::unordered_map <Tile *, Tile *> oldNewPairs;
    for(Tile *oldTile : oldAllTiles){
        oldNewPairs[oldTile] = new Tile(*oldTile);
    }

    // maintain the pointers of the new Tiles, while simultanuously maintain mAllNonBlankTilesMap
    for(auto pairs : oldNewPairs){

        Tile *father = pairs.first;
        Tile *son = pairs.second;

        // insert to mAllNonBlankTilesMap if tile is not BLANK
        if(son->getType() != tileType::BLANK){
            this->mAllNonBlankTilesMap[son->getLowerLeft()] = son;
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
    if(mAllNonBlankTilesMap.size() == 0) return;
    
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
    Tile *index;
    if(mAllNonBlankTilesMap.empty()){
        return mCanvasSizeBlankTile;
    }else{
        index = mAllNonBlankTilesMap.begin()->second; 
    } 

    while(!(rec::isContained(index->getRectangle(), key))){

        Rectangle indexRec = index->getRectangle();
        bool yDownInRange = (key.y() >= rec::getYL(indexRec));
        bool yUpInRange = (key.y() < rec::getYH(indexRec));
        bool YCordInTile = (yDownInRange && yUpInRange);

        if(!YCordInTile){
            // Adjust vertical range
            if(key.y() >= index->getYLow()){
                assert(index->rt != nullptr);
                index = index->rt;
            }else{
                assert(index->lb != nullptr);
                index = index->lb;
            }
        }else{
            // Vertical range correct! adjust horizontal range
            if(key.x() >= index->getXLow()){
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
    while(n->getXLow() > centre->getXLow()){
    
        neighbors.push_back(n);
        n = n->bl;
    }
    neighbors.push_back(n);
}

void CornerStitching::findDownNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const{
    if(centre == nullptr) return;
    if(centre->lb == nullptr) return;

    Tile *n = centre->lb;
    while(n->getXHigh() < centre->getXHigh()){
        neighbors.push_back(n);
        n = n->tr;
    }
    neighbors.push_back(n);
}

void CornerStitching::findLeftNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const{
    if(centre == nullptr) return;
    if(centre->bl == nullptr) return;

    Tile *n = centre->bl;
    while(n->getYHigh() < centre->getYHigh()){
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
    while(n->getYLow() > centre->getYLow()){
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
    Tile *currentFind = findPoint(Cord(rec::getXL(box), rec::getYH(box) - 1));

    while(currentFind->getYHigh() > rec::getYL(box)){
        // check if the tile is solid
        if(currentFind->getType() != tileType::BLANK){
            // This is an edge of a solid tile
            return true;
        }else if(currentFind->getXHigh() < rec::getXH(box)){
            // See if the right edge within AOI, right must be a tile
            return true;
        }

        // Move down to the next tile touching the left edge of AOI
        if(currentFind->getYLow() < 1) break;

        currentFind = findPoint(Cord(rec::getXL(box), currentFind->getYLow() - 1));
    }

    return false;
}

bool CornerStitching::searchArea(Rectangle box, Tile *someTile) const{

    if(!checkRectangleInCanvas(box)){
        throw CSException("CORNERSTITCHING_02");
    }

    // Use point-finding algo to locate the tile containin the upperleft corner of AOI
    Tile *currentFind = findPoint(Cord(rec::getXL(box), rec::getYH(box) - 1));

    while(currentFind->getYHigh() > rec::getYL(box)){
        // check if the tile is solid
        if(currentFind->getType() != tileType::BLANK){
            // This is an edge of a solid tile
            someTile = currentFind;
            return true;
        }else if(currentFind->getXHigh() < rec::getXH(box)){
            // See if the right edge within AOI, right must be a tile
            someTile = currentFind;
            return true;
        }

        // Move down to the next tile touching the left edge of AOI
        if(currentFind->getYLow() < 1) break;

        currentFind = findPoint(Cord(rec::getXL(box), currentFind->getYLow() - 1));
    }

    return false;
}

void CornerStitching::enumerateDirectedArea(Rectangle box, std::vector <Tile *> &allTiles) const{

    if(!checkRectangleInCanvas(box)){
        throw CSException("CORNERSTITCHING_03");
    }

    // Use point-finding algo to locate the tile containin the upperleft corner of AOI
    Tile *leftTouchTile = findPoint(Cord(rec::getXL(box), rec::getYH(box) - 1));

    while(leftTouchTile->getYHigh() > rec::getYL(box)){
        enumerateDirectedAreaRProcedure(box, allTiles, leftTouchTile);
        if(leftTouchTile->getYLow() < 1) break;
        // step to the next tile along the left edge
        leftTouchTile = findPoint(Cord(rec::getXL(box), leftTouchTile->getYLow() - 1 ));
    }
}


void CornerStitching::visualiseArtpiece(const std::string outputFileName) const{
    std::ofstream ofs;
    ofs.open(outputFileName);
    if(!ofs.is_open()) throw(CSException("CORNERSTITCHING_05"));

    std::unordered_set<Tile *> allTiles;
    collectAllTiles(allTiles);

    // write out the total tile numbers
    ofs << allTiles.size() << std::endl;
    // Then start to write info for each file
    for(Tile *tile : allTiles){
        unsigned long long tileHash;
        tileHash = 2*tile->getLowerLeft().x() + 3*tile->getLowerLeft().y() + 5*tile->getWidth() + 7*tile->getHeight();
        ofs << tileHash << " " << *tile << std::endl;

        Tile *rtTile = tile->rt;
        ofs << "rt: ";
        if(rtTile == nullptr){
            ofs << "nullptr" << std::endl; 
        }else{
            unsigned long long rtHash = 2*rtTile->getLowerLeft().x() + 3*rtTile->getLowerLeft().y() + 5*rtTile->getWidth() + 7*rtTile->getHeight();
            ofs << rtHash << std::endl;
        }

        Tile *trTile = tile->tr;
        ofs << "tr: ";
        if(trTile == nullptr){
            ofs << "nullptr" << std::endl; 
        }else{
            unsigned long long trHash = 2*trTile->getLowerLeft().x() + 3*trTile->getLowerLeft().y() + 5*trTile->getWidth() + 7*trTile->getHeight();
            ofs << trHash << std::endl;
        }

        Tile *blTile = tile->bl;
        ofs << "bl: ";
        if(blTile == nullptr){
            ofs << "nullptr" << std::endl; 
        }else{
            unsigned long long blHash = 2*blTile->getLowerLeft().x() + 3*blTile->getLowerLeft().y() + 5*blTile->getWidth() + 7*blTile->getHeight();
            ofs << blTile << std::endl;
        }

        Tile *lbTile = tile->lb;
        ofs << "lb: ";
        if(lbTile == nullptr){
            ofs << "nullptr" << std::endl; 
        }else{
            unsigned long long lbHash = 2*lbTile->getLowerLeft().x() + 3*lbTile->getLowerLeft().y() + 5*lbTile->getWidth() + 7*lbTile->getHeight();
            ofs << lbTile << std::endl;
        }
    }
    ofs.close();
}