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

void CornerStitching::cutTileHorizontally(Tile *origTop, Tile *newDown, len_t newDownHeight){

    // check if the cut is valid due to the height of origTop and expect newDownHeight
    if(origTop->getHeight() <= newDownHeight){
        throw CSException("CORNERSTITCHING_09");
    }

    newDown = new Tile(tileType::BLANK, origTop->getLowerLeft(),origTop->getWidth(), newDownHeight);

    newDown->rt = origTop;
    newDown->lb = origTop->lb;
    newDown->bl = origTop->bl;

    // maniputlate surronding tiles of origTop and newDown

    // change lower-neighbors' rt pointer to newDown
    std::vector <Tile *> origDownNeighbors;
    findDownNeighbors(origTop, origDownNeighbors);
    for(Tile *t : origDownNeighbors){
        if(t->rt == origTop) t->rt = newDown;
    }

    // 1. find the correct tr for newDown
    // 2. change right neighbors to point their bl to the correct tile (whether to switch to newDown or keep origTop)
    std::vector <Tile *> origRightNeighbors;
    findRightNeighbors(origTop, origRightNeighbors);
    
    bool rightModified = false;
    for(int i = 0; i < origRightNeighbors.size(); ++i){
        if(origRightNeighbors[i]->getYLow() < newDown->getYHigh()){
            if(!rightModified){
                rightModified = true;
                newDown->tr = origRightNeighbors[i];
            }
            // 08/06/2023 bug fix: change "tile" -> "newDown", add break statement to terminate unecessary serarch early
            if(origRightNeighbors[i]->getYLow() >= newDown->getYLow()){
                origRightNeighbors[i]->bl = newDown;
            }else{
                break;
            }
        }
    }

    // 1. find the new correct bl for origTop
    // 2. change left neighbors to point their tr to the correct tile (whether to switch to newDown or keep origTop)
    std::vector <Tile *> origLeftNeighbors;
    findLeftNeighbors(origTop, origLeftNeighbors);

    bool leftModified = false;
    for(int i = 0; i < origLeftNeighbors.size(); ++i){
        if(origLeftNeighbors[i]->getYHigh() > newDown->getYHigh()){
            if(!leftModified){
                leftModified = true;
                origTop->bl = origLeftNeighbors[i];
                break;
            }
        }else{
            origLeftNeighbors[i]->tr = newDown;
        }
    }

    origTop->setLowerLeft(newDown->getUpperLeft());
    origTop->setHeight(origTop->getHeight() - newDownHeight);
    origTop->lb = newDown;
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

Tile *CornerStitching::insertTile(const Tile &tile){
    // check if the input prototype is within the canvas
    if(!checkRectangleInCanvas(tile.getRectangle())){
        throw CSException("CORNERSTITCHING_06");
    }

    // check if the tile inserting position already exist anotehr tile
    if(searchArea(tile.getRectangle())){
        throw CSException("CORNERSTITCHING_07");
    }

    /*  STEP 1)
        Find the space tile containing the top edge of the area to be occupied by the new tile
        (due to the strip property, a single space tile must contain the entire edge).
        Then split the top space tile along a horizontal line into a piece entirely above the new tile 
        and a piece overlapping the new tile. Update corne stitches in the tiles adjoining the new tile
    */

    bool tileTouchesSky = (tile.getYHigh() == mCanvasSizeBlankTile->getYHigh());
    bool cleanTopCut = true;
    Tile *origTop;
    if(!tileTouchesSky){
        origTop = findPoint(tile.getUpperLeft());
        cleanTopCut = (origTop->getYLow() == tile.getYHigh());
    }

    if((!tileTouchesSky) && (!cleanTopCut)){
        Tile *newDown;
        cutTileHorizontally(origTop, newDown, tile.getYHigh() - origTop->getYHigh());
    }

    /*  STEP 2)
        Find the space tile containing the bottom edge of the area to be occupied by the new tile, 
        split it in similar fation in STEP 1, and update stitches around it.
    */

    bool tileTouchesGround = (tile.getYLow() == mCanvasSizeBlankTile->getYLow());
    bool cleanBottomCut = true;
    Tile *origBottom;
    if(!tileTouchesGround){
        Cord targetTileLL = tile.getLowerLeft();
        origBottom = findPoint(Cord(targetTileLL.x(), (targetTileLL.y() - 1)));
        cleanBottomCut = (origBottom->getYHigh() == tile.getYLow());
    }

    if((!tileTouchesGround) && (!cleanBottomCut)){
        Tile *newDown;
        cutTileHorizontally(origBottom, newDown, tile.getYLow() - origBottom->getYLow());
    }

    /*  STEP 3)
        Work down along the left side ofthe area of the new tile. Each tile along this edge must be a space tile that
        spans the entire width of the new solid tile. Split the tile into a piece entirely to the left of the new tile, 
        one entirely within the new tile, and one to the right. The splitting may make it possible to merge the left and 
        right remainders vertically with the tiles just above them: merge whenever possible. Finally, merge the centre
        space tile with the solid tile that is forming. Each split or merge requires stitches to be updated and adjoining tiles.
    */

    Tile *splitTile = findPoint(Cord(tile.getXLow(), (tile.getYHigh() - 1)));
    len_t findTileY = splitTile->getYLow();
    Tile *oldsplitTile;

    // Merge assisting indexes
    len_t leftMergeWidth = 0, rightMergeWidth = 0;
    bool topMostMerge = true;

    while(true){

        // calculate the borders of left blank tile and right blank tile
        len_t blankLeftBorder = splitTile->getXLow();
        len_t tileLeftBorder = tile.getXLow();
        len_t tileRightBorder = tile.getXHigh();
        len_t blankRightBorder = splitTile->getXHigh();

        // The middle tile that's completely within the new tile.
        Tile *newMid = new Tile(tileType::BLANK, Cord(tileLeftBorder, splitTile->getYLow()), tile.getWidth(), splitTile->getHeight());

        // initialize bl, tr pointer in case the left and right tile do not exist
        newMid->bl = splitTile->bl;
        newMid->tr = splitTile->tr;

        std::vector<Tile *> topNeighbors;
        findTopNeighbors(splitTile, topNeighbors);
        std::vector<Tile *>bottomNeighbors;
        findDownNeighbors(splitTile, bottomNeighbors);
        std::vector<Tile *>leftNeighbors;
        findLeftNeighbors(splitTile, leftNeighbors);
        std::vector<Tile *>rightNeighbors;
        findRightNeighbors(splitTile, rightNeighbors);

        // split the left piece if necessary, maintain pointers all around
        bool leftSplitNecessary = (blankLeftBorder != tileLeftBorder);

        if(leftSplitNecessary){
            Tile *newLeft = new Tile(tileType::BLANK, splitTile->getLowerLeft(), (tileLeftBorder - blankLeftBorder) ,splitTile->getHeight());

            newLeft->tr = newMid;
            newLeft->bl = splitTile->bl;

            newMid->bl = newLeft;

            // fix pointers about the top neighbors of new created left tile
            bool rtModified = false;
            for(int i = 0; i < topNeighbors.size(); ++i){
                if(topNeighbors[i]->getXLow() < tileLeftBorder){
                    if(!rtModified){
                        rtModified = true;
                        newLeft->rt = topNeighbors[i];
                    }
                    if(topNeighbors[i]->getXLow() >= blankLeftBorder){
                        topNeighbors[i]->lb = newLeft;
                    }else{
                        break;
                    }
                }
            }
            
            // fix pointers about the bottom neighbors of new created left tile
            bool lbModified = false;
            for(int i = 0; i < bottomNeighbors.size(); ++i){
                if(bottomNeighbors[i]->getXLow() < tileLeftBorder){
                    if(!lbModified){
                        lbModified = true;
                        newLeft->lb = bottomNeighbors[i];
                    }
                    if(bottomNeighbors[i]->getXHigh() <= tileLeftBorder){
                        bottomNeighbors[i]->rt = newLeft;
                    }
                }else{
                    break;
                }
            }

            // change tr pointers of left neighbors to the new created left tile
            for(int i = 0; i < leftNeighbors.size(); ++i){
                if(leftNeighbors[i]->tr == splitTile){
                    leftNeighbors[i]->tr = newLeft;
                }
            }

        }else{
            // change the tr pointers of the left neighbors to newMid
            for(int i = 0; i < leftNeighbors.size(); ++i){
                if(leftNeighbors[i]->tr == splitTile){
                    leftNeighbors[i]->tr = newMid;
                }
            }
        }


        // split the right piece if necessary, maintain pointers all around
        bool rightSplitNecessary = (tileRightBorder != blankRightBorder);
        if(rightSplitNecessary){
            Tile *newRight = new Tile(tileType::BLANK, newMid->getLowerRight(), (blankRightBorder- tileRightBorder) ,newMid->getHeight());

            newRight->tr = splitTile->tr;
            newRight->bl = newMid;

            newMid->tr = newRight;

            // fix pointers about the top neighbors of new created right tile
            bool rtModified = false;
            for(int i = 0; i < topNeighbors.size(); ++i){
                if(topNeighbors[i]->getXHigh() > tileRightBorder){
                    if(!rtModified){
                        rtModified = true;
                        newRight->rt = topNeighbors[i];
                    }
                    if(topNeighbors[i]->getLowerLeft().x >= tileRightBorder){
                        topNeighbors[i]->lb = newRight;
                    }
                }else{
                    break;
                }
            }

            // fix pointers about the bottom neighbors of new created right tile

            bool lbModified = false;
            for(int i = 0 ; i < bottomNeighbors.size(); ++i){
                if(bottomNeighbors[i]->getLowerRight().x > tileRightBorder){
                    if(!lbModified){
                        lbModified = true;
                        newRight->lb = bottomNeighbors[i];
                    }
                    if(bottomNeighbors[i]->getLowerRight().x <= blankRightBorder){
                        bottomNeighbors[i]->rt = newRight;
                    }else{
                        break;
                    }
                }
            }

            // also change bl pointers of right neighbors to the newly created left tile
            for(int i = 0; i < rightNeighbors.size(); ++i){
                if(rightNeighbors[i]->bl == splitTile){
                    rightNeighbors[i]->bl = newRight;
                }
            }
            
        }else{
            // change bl pointers of right neighbors back to newMid
            for(int i = 0; i < rightNeighbors.size(); ++i){
                if(rightNeighbors[i]->bl == splitTile){
                    rightNeighbors[i]->bl = newMid;
                }
            }
        }




   }




}

void CornerStitching::visualiseTileDistribution(const std::string outputFileName) const{
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
            ofs << *rtTile << std::endl;
        }

        Tile *trTile = tile->tr;
        ofs << "tr: ";
        if(trTile == nullptr){
            ofs << "nullptr" << std::endl; 
        }else{
            ofs << *trTile << std::endl;
        }

        Tile *blTile = tile->bl;
        ofs << "bl: ";
        if(blTile == nullptr){
            ofs << "nullptr" << std::endl; 
        }else{
            ofs << *blTile << std::endl;
        }

        Tile *lbTile = tile->lb;
        ofs << "lb: ";
        if(lbTile == nullptr){
            ofs << "nullptr" << std::endl; 
        }else{
            ofs << *lbTile << std::endl;
        }
    }
    ofs.close();
}

bool CornerStitching::checkMergingSuccess(std::vector<std::pair<Tile *, Tile *>> &failTiles) const{
    std::unordered_set<Tile *> allTilesSet;
    collectAllTiles(allTilesSet);

    std::vector<Tile *> allTilesArr (allTilesSet.begin(), allTilesSet.end());
    
    // this is for checking the copying works
    std::cout << "allTilesArr: " << std::endl;
    for(Tile *t : allTilesArr){
        std::cout << *t << std::endl;
    }
    bool errorFree = true;

    Tile *tile1, *tile2;
    for(int i = 0; i < allTilesArr.size(); ++i){
        for(int j = (i+1); j < allTilesArr.size(); ++j){
            tile1 = allTilesArr[i];
            tile2 = allTilesArr[j];
            assert(tile1 != nullptr);
            assert(tile2 != nullptr);
            
            if((tile1->getYLow() == tile2->getYLow()) && (tile1->getHeight() == tile2->getHeight())){
                // horizontal merge potential
                assert(tile1->getXLow() != tile2->getXLow());

                Tile *leftTile, *rightTile;
                if(tile1->getXLow() < tile2->getXLow()){
                    leftTile = tile1;
                    rightTile = tile2;
                }else{
                    leftTile = tile2;
                    rightTile = tile1;
                }

                if(leftTile->getXHigh() == rightTile->getXLow()){
                    failTiles.push_back(std::pair<Tile *, Tile *>(tile1, tile2));
                    errorFree = false;
                }

            }else if((tile1->getXLow() == tile2->getXLow()) && (tile1->getWidth() == tile2->getWidth())){
                // vertical merge potential
                assert(tile1->getYLow() != tile2->getYLow());

                Tile *upTile, *downTile;
                if(tile1->getYLow() < tile2->getYLow()){
                    upTile = tile1;
                    downTile = tile2;
                }else{
                    upTile = tile2;
                    downTile = tile1;
                }

                if(downTile->getYHigh() == upTile->getYLow()){
                    failTiles.push_back(std::pair<Tile *, Tile *>(tile1, tile2));
                    errorFree = false;
                }

            }
        }
    }

    return errorFree;

}

