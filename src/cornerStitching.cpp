#include "cornerStitching.h"

void CornerStitching::collectAllTiles(std::unordered_set<Tile *> &allTiles) const{
    allTiles.clear();
    if(allNonBlankTilesMap.empty()){
        allTiles.insert(CanvasSizeBlankTile);
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

    // work on the exception handling....
    assert(key.x() >= 0);
    assert(key.y() >= 0);
    assert(key.x < getCanvasWidth());
    assert(key.y < getCanvasHeight());

    if((key.x() < 0) || (key.x() < 0) || (key.) || ()){
        throw 
    }

    Tile *index = getRandomTile();
    
    while(!(index->checkCordInTile(key))){
        if(!index->checkYCordInTile(key)){
            // Adjust vertical range
            if(key.y >= index->getLowerLeft().y){
                assert(index->rt != nullptr);
                index = index->rt;
            }else{
                assert(index->lb != nullptr);
                index = index->lb;
            }
        }else{
            // Vertical range correct! adjust horizontal range
            if(key.x >= index->getLowerLeft().x){
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