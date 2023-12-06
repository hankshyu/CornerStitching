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

    // Pick-up all tiles(include BLANK) inside the data structure 
    void collectAllTiles(std::unordered_set<Tile *> &allTiles) const;
    void collectAllTilesDFS(Tile *tile, std::unordered_set<Tile *> &allTiles) const;

public:
    CornerStitching() = delete;
    CornerStitching(len_t chipWidth, len_t chipHeight);
    CornerStitching(const CornerStitching &other);
    ~CornerStitching();

    len_t getCanvasWidth() const;
    len_t getCanvasHeight() const;

    // Given a Cord, find the tile(could be balnk or block) that includes it.
    Tile *findPoint(const Cord &key) const;
    
    // Pushes all neighbors of Tile "centre" to vector "neighbors".
    void findTopNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const;
    void findDownNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const;
    void findLeftNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const;
    void findRightNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const;
    void findAllNeighbors(Tile *centre, std::vector<Tile *> &neighbors) const;

    // Determine if there is any nonblank tiles in the box area
    bool searchArea(Rectangle box, Tile &target) const;

    // Enumerates all nonblank tiles in a given box area, 
    // each tile is visited only after all the tiles above and to its left does
    void enumerateDirectArea(Rectangle box, std::vector <Tile *> &allTiles) const;

    void insertTile(Tile &tile);
    void removeTile(Tile *tile);

    void visualiseArtpiece(const std::string ouputFileName);
};
#endif // __CORNERSTITCHING_H__