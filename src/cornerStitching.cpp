#include <fstream>
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
        if((chipWidth <= 0) || (chipHeight <= 0)){
            throw CSException("CORNERSTITCHING_04");
        }
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

void CornerStitching::enumerateDirectedArea(Rectangle box, std::vector <Tile *> &allTiles) const{

    if(!checkRectangleInCanvas(box)){
        throw CSException("CORNERSTITCHING_03");
    }

    // Use point-finding algo to locate the tile containin the upperleft corner of AOI
    len_t searchRBorderHeight = boost::polygon::yh(box) - 1;
    Tile *leftTouchTile = findPoint(Cord(boost::polygon::xl(box), searchRBorderHeight));

    while(leftTouchTile->getUpperLeft().y() > boost::polygon::yl(box)){
        enumerateDirectedAreaRProcedure(box, allTiles, leftTouchTile);
        if(leftTouchTile->getLowerLeft().y() < 1) break;
        leftTouchTile = findPoint(Cord(boost::polygon::xl(box), leftTouchTile->getLowerLeft().y() - 1));
    }
}

void CornerStitching::enumerateDirectedAreaRProcedure(Rectangle box, std::vector <Tile *> &allTiles, Tile *targetTile) const{
    
    // R1) Enumerate the tile
    if(targetTile->getType() == tileType::BLOCK || targetTile->getType() == tileType::OVERLAP){
        allTiles.push_back(targetTile);
    }

    // R2) If the right edge of the tile is outside of the seearch area, return
    if(targetTile->getLowerRight().x() >= boost::polygon::xh(box)) return;

    // R3) Use neighbor-finding algo to locate all the tiles that touch the right side of the current tile and also intersect the search area
    std::vector<Tile *> rightNeighbors;
    findRightNeighbors(targetTile, rightNeighbors);

    Rectangle targetTileRect = targetTile->getRectangle();

    for(Tile *t : rightNeighbors){
        // make sure the tile is in the AOI
        Rectangle trec = t->getRectangle();
        len_t Containedyl = boost::polygon::yl(trec);
        len_t Containedyh = boost::polygon::yh(trec) - 1;
        len_t Containedxl = boost::polygon::xl(trec);
        Tile boxSizeTile(tileType::BLANK, box);
        bool containedylInBox = boxSizeTile.checkCordInTile(Cord(Containedxl, Containedyl));
        bool containedyhInBox = boxSizeTile.checkCordInTile(Cord(Containedxl, Containedyh));
        if(!((containedylInBox) || (containedyhInBox))) break;

        // R4) If bottom left corner of the neighbor touches the current tile
        Cord rightLL = t->getLowerLeft();
        bool R4 = (rightLL.y() >= boost::polygon::yl(targetTileRect));

        // R5) If the bottom edge ofthe search area cuts both the target tile and its neighbor
        len_t bottomEdge = boost::polygon::yl(targetTileRect);

        len_t targetTileyl = boost::polygon::yl(targetTileRect);
        len_t targetTileyh = boost::polygon::yh(targetTileRect);
        bool cutTargetTile = (targetTileyl <= bottomEdge) && (targetTileyh > bottomEdge); 

        len_t tyl = boost::polygon::yl(trec);
        len_t tyh = boost::polygon::yh(trec);
        bool cutNeighbor = (tyl <= bottomEdge) && (tyh > bottomEdge); 
        bool R5 = cutTargetTile && cutNeighbor;

        if(R4 || R5) enumerateDirectedAreaRProcedure(box, allTiles, t);
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