#ifndef __FLOORPLAN_H__
#define __FLOORPLAN_H__

#include <string.h>
#include <unordered_map>

#include "tile.h"
#include "rectangle.h"
#include "rectilinear.h"
#include "connection.h"
#include "cornerStitching.h"
#include "globalResult.h"

class Floorplan{
private:
    int mIDCounter;

    Rectangle mChipContour;

    int mAllRectilinearCount;
    int mSoftRectilinearCount;
    // int mHardRectilinearCount;
    int mPreplacedRectilinearCount;

    int mConnectionCount;

    double mGlobalAspectRatioMin;
    double mGlobalAspectRatioMax;
    double mGlobalUtilizationMin;
    
    // function that places a rectilinear into the floorplan system. It automatically resolves overlaps by splittng and divide existing tiles
    Rectilinear *placeRectilinear(std::string name, rectilinearType type, Rectangle placement, area_t legalArea, double aspectRatioMin, double aspectRatioMax, double mUtilizationMin);

public:
    CornerStitching *cs;

    std::vector<Rectilinear *> allRectilinears;
    std::vector<Rectilinear *> softRectilinears;
    // std::vector<Rectilinear *> hardRectilinears;
    std::vector<Rectilinear *> preplacedRectilinears;

    std::vector<Connection> allConnections;
    
    std::unordered_map<Tile *, Rectilinear *> blockTilePayload;
    std::unordered_map<Tile *, std::vector<Rectilinear *>> overlapTilePayload;


    Floorplan();
    Floorplan(GlobalResult gr, double aspectRatioMin, double aspectRatioMax, double utilizationMin);
    Floorplan(const Floorplan &other);
    ~Floorplan();

    Floorplan& operator = (const Floorplan &other);

    Rectangle getChipContour() const;
    int getAllRectilinearCount() const;
    int getSoftRectilinearCount() const;
    int getPreplacedRectilinearCount() const;
    int getConnectionCount() const;
    double getGlobalAspectRatioMin() const;
    double getGlobalAspectRatioMax() const;
    double getGlobalUtilizationMin() const;

    void setGlobalAspectRatioMin(double globalAspectRatioMin);
    void setGlobalAspectRatioMax(double globalAspectRatioMax);
    void setGlobalUtilizationMin(double globalUtilizationMin);

    // insert a tleType::BLOCK tile at tilePosition into cornerStitching & rectilinear (*rt) system,
    // record rt as it's payload into floorplan system (into blockTilePayload) and return new tile's pointer
    Tile *addBlockTile(const Rectangle &tilePosition, Rectilinear *rt);

    // insert a tleType::OVERLAP tile at tilePosition into cornerStitching & rectilinear (•rt) system,
    // record payload as it's payload into floorplan system (into overlapTilePayload) and return new tile's pointer
    Tile *addOverlapTile(const Rectangle &tilePosition, const std::vector<Rectilinear*> &payload);

    // remove tile data payload at floorplan system, the rectilienar that records it and lastly remove from cornerStitching,
    // only tile.type == tileType::BLOCK or tileType::OVERLAP is accepted
    void deleteTile(Tile *tile);

    // log rt as it's overlap tiles and update Rectilinear structure & floorplan paylaod, upgrade tile to tile::OVERLAP if necessary 
    void increaseTileOverlap(Tile *tile, Rectilinear *newRect);

    // remove rt as tile's overlap and update Rectilinear structure & floorplan payload, make tile tile::BLOCK if necessary
    void decreaseTileOverlap(Tile *tile, Rectilinear *removeRect);

    // collect all blocks within a rectilinear, reDice them into Rectangles. (potentially reduce Tile count)
    void reshapeRectilinear(Rectilinear *rt);

    // Pass in the victim tile pointer through origTop, it will split the tile into two pieces:
    // 1. origTop represents the top portion of the split, with height (origTop.height - newDownHeight)
    // 2. newDown represents the lower portion of the split, with height newDownHeight, is the return value
    Tile *divideTileHorizontally(Tile *origTop, len_t newDownHeight);

    // Pass in the victim tile pointer through origRight, it will stplit the tile into two pieces:
    // 1. origRight represents the right portion of the split, with width (origRight.width - newLeftWidth)
    // 2. newLeft represents the left portion of the split, with width newLeftWidth, is the return value
    Tile *divideTileVertically(Tile *origRight, len_t newLeftWidth);

    // calculate the HPWL (cost) of the floorplan system, using the connections information stored inside "allConnections"
    double calculateHPWL();

    // write Floorplan class for presenting software (renderFloorplan.py)
    void visualiseLegalFloorplan(const std::string &outputFileName) const;
    
};

namespace std{
    template<>
    struct hash<Floorplan>{
        size_t operator()(const Floorplan &key) const;
    };
}

#endif // #define __FLOORPLAN_H__