#ifndef __CORNERSTITCHING_H__
#define __CORNERSTITCHING_H__

#include <unordered_map>
#include <unordered_set>

#include "units.h"
#include "cord.h"
#include "tile.h"

class CornerStitching {
private:
    
    len_t mCanvasWidth, mCanvasHeight;
    Tile *mCanvasSizeBlankTile;

    std::unordered_map <Cord, Tile*> mAllNonBlankTilesMap;
    // return true if the object is inside the canvas, else return false
    bool checkPointInCanvas(const Cord &point) const;
    bool checkRectangleInCanvas(const Rectangle &rect) const;

    // Pick-up all tiles(include BLANK) inside the data structure 
    void collectAllTiles(std::unordered_set<Tile *> &allTiles) const;
    void collectAllTilesDFS(Tile *tile, std::unordered_set<Tile *> &allTiles) const;

    // Helper function of enumerateDirectArea
    void enumerateDirectedAreaRProcedure(Rectangle box, std::vector <Tile *> &allTiles, Tile *targetTile) const;

    // Pass in a victim tile through origTop, it will split the victim into two pieces:
    // 1. origTop represents the top portion of the split, with height (origTop.height - newDownHeight)
    // 2. newDown represents the lower portion of the split, with height newDownHeight, is the return value
    Tile *cutTileHorizontally(Tile *origTop, len_t newDownHeight);

    // Merges two tiles (up, down) along a horizontal cut line:
    // mergeDown is merged into mergeUp, mergeDown is deleted and the new merged tile is the return value
    Tile *mergeTilesHorizontally(Tile *mergeUp, Tile *mergeDown);

    // Merges two tiles (left, right) along a vertical cut line:
    // mergeRight is merged into mergeLeft, mergeRight is deleted and the new merged tile is the return value
    Tile *mergeTilesVertically(Tile *mergeLeft, Tile *mergeRight);

public:
    CornerStitching() = delete;
    CornerStitching(len_t chipWidth, len_t chipHeight);
    CornerStitching(const CornerStitching &other);
    ~CornerStitching();
 
    bool operator == (const CornerStitching &comp) const;

    len_t getCanvasWidth() const;
    len_t getCanvasHeight() const;

    // Given a Cord, find the tile (could be balnk or block) that includes it.
    Tile *findPoint(const Cord &key) const;
    
    // Pushes all top neighbors of Tile "centre" to vector "neighbors", the first would be centre->tr, then to left (<--)
    void findTopNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const;
    // Pushes all down neighbors of Tile "centre" to vector "neighbors", the first would be centre->lb, then to right (-->) 
    void findDownNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const;
    // Pushes all left neighbors of Tile "centre" to vector "neighbors", the first would be centre->bl, then up (^)
    void findLeftNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const;
    // Pushes all right neighbors of Tile "centre" to vector "neighbors", the first would be centre->tr, then down (v)
    void findRightNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const;
    // Pushes all neighbors (in the order of top, left, down, right) of Tile "centre" to vector "neighbors"
    // Short hand for continuoutsly call, find Top, Left, Down, Right neighbors
    void findAllNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const;

    // Determine if there is any nonblank tiles in the box area
    bool searchArea(Rectangle box) const;
    // Determine if there is any nonblank tiles in the box area, return any tile in the region if found (via pointer)
    bool searchArea(Rectangle box, Tile *someTile) const;

    // Enumerates all nonblank tiles in a given box area, each tile is visited only after all the tiles above and to its left does
    void enumerateDirectedArea(Rectangle box, std::vector <Tile *> &allTiles) const;

    // Input a tile "prototype" to insert into the corner stitching system, returns the actual pointer to the tile in the system
    Tile *insertTile(const Tile &tile);

    // Removes the tile within system, retunrs false if Tile not within cornerStitching System. true if succes
    bool removeTile(Tile *tile);

    // Output format for presenting software
    void visualiseTileDistribution(const std::string ouputFileName) const;
    // Check globally if any two tiles is mergable, returns true if no fails found, else return false and
    // two mergable tiles is returned through tile1 and tile2
    bool debugBlankMerged(Tile *tile1, Tile *tile2) const;

    
};
#endif // __CORNERSTITCHING_H__