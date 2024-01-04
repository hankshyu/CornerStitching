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

    std::unordered_map <Cord, Tile*> allNonBlankTilesMap;

    // return true if the object is inside the canvas, else return false
    bool checkPointInCanvas(const Cord &point) const;
    bool checkRectangleInCanvas(const Rectangle &rect) const;

    // Pick-up all tiles(include BLANK) inside the data structure 
    void collectAllTiles(std::unordered_set<Tile *> &allTiles) const;
    void collectAllTilesDFS(Tile *tile, std::unordered_set<Tile *> &allTiles) const;

    // Helper function of enumerateDirectArea
    void enumerateDirectedAreaRProcedure(Rectangle box, std::vector <Tile *> &allTiles, Tile *targetTile) const;

public:
    CornerStitching() = delete;
    CornerStitching(len_t chipWidth, len_t chipHeight);
    CornerStitching(const CornerStitching &other);
    ~CornerStitching();

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
    // Determine if there is any nonblank tiles in the box area, return any tile in the region if found, nullptr otherwise 
    bool searchArea(Rectangle box, Tile *target) const;

    // Enumerates all nonblank tiles in a given box area, each tile is visited only after all the tiles above and to its left does
    void enumerateDirectedArea(Rectangle box, std::vector <Tile *> &allTiles) const;

    void insertTile(Tile &tile);
    void removeTile(Tile *tile);

    void visualiseArtpiece(const std::string ouputFileName) const;
};
#endif // __CORNERSTITCHING_H__