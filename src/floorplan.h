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
    int mHardRectilinearCount;
    int mPreplacedRectilinearCount;

    int mConnectionCount;

    int mGlobalAspectRatioMin;
    int mGlobalAspectRatioMax;
    int mGlobalUtilizationMin;
    
public:
    CornerStitching *cs;

    std::vector<Rectilinear *> allRectilinears;
    std::vector<Rectilinear *> softRectilinears;
    std::vector<Rectilinear *> hardRectilinears;
    std::vector<Rectilinear *> preplacedRectilinears;

    std::vector<Connection> allConnections;
    
    std::unordered_map<Tile *, Rectilinear *> blockTilePayload;
    std::unordered_map<Tile *, std::vector<Rectilinear *>> overlapTilePayload;


    Floorplan();
    Floorplan(GlobalResult gr);
    Floorplan(const Floorplan &other);
    ~Floorplan();

    Floorplan& operator = (const Floorplan &other);

    Rectangle getChipContour() const;
    int getAllRectilinearCount() const;
    int getSoftRectilinearCount() const;
    int getHardRectilinearCount() const;
    int getPreplacedRectilinearCount() const;
    int getConnectionCount() const;
    int getGlobalAspectRatioMin() const;
    int getGlobalAspectRatioMax() const;
    int getGlobalUtilizationMin() const;

    void setGlobalAspectRatioMin(int globalAspectRatioMin);
    void setGlobalAspectRatioMax(int globalAspectRatioMax);
    void setGlobalUtilizationMin(int globalUtilizationMin);

    // insert a tleType::BLOCK tile (using tilePrototype as prototype) into cornerStitching & rectilinear system,
    // and record rt as it's payload into blockTilePayload
    Tile *addBlockTile(Rectilinear *rt, const Tile &tilePrototype);
    // insert a tleType::OVERLAP tile (using tilePrototype as prototype) into cornerStitching & rectilinear system,
    // and record payload as it's payload into overlapTilePayload
    Tile *addOverlapTile(Rectilinear *rt, const Tile &tilePrototype, const std::vector<Rectilinear*> payload);

    // remove the tile from certain rectilinear structure
    void deleteTile(Tile *tile);

    // log rt as it's overlap tiles and update Rectilinear structure & floorplan paylaod, upgrade tile to tile::OVERLAP if necessary 
    void increaseTileOverlap(Tile *tile, Rectilinear *newRect);

    // remove rt as tile's overlap and update Rectilinear structure & floorplan payload, make tile tile::BLOCK if necessary
    void decreaseTileOverlap(Tile *tile, Rectilinear *rt);

    // function that places a rectilinear into the floorplan system. It automatically resolves overlaps by splittng and divide existing tiles
    Rectilinear *placeRectilinear(std::string name, rectilinearType type, Rectangle placement, area_t legalArea, double aspectRatioMin, double aspectRatioMax, double mUtilizationMin);

    // calculate the HPWL (cost) of the floorplan system, using the connections information stored inside "allConnections"
    double calculateHPWL();
    
};

#endif // #define __FLOORPLAN_H__