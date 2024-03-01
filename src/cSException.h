#ifndef __CSEXCEPTION_H__
#define __CSEXCEPTION_H__

#include <exception>
#include <unordered_map>
#include <string>

#define CSEX_EMPTY "CSEX: Empty Excpeiton"

/* Look Up Table for all exceptin messages */
inline std::unordered_map<std::string, const char*> CSEXCEPTION_LUT = {
    {"TILE_01", "operator <<: tileType out of Range"},

    {"GLOBALPHASEADAPTER_01", "globalPhaseAdapter(): file stream not open"},
    {"GLOBALPHASEADAPTER_02", "realGlobalResult(): Module not marked as SOFT or FIXED"},
    
    {"GLOBALRESULT_01", "GlobalResult(std::string globalResultFile): file stream not open"},
    {"GLOBALRESULT_02", "GlobalResult(const std::ifstream &ifs): file stream not open"},
    {"GLOBALRESULT_03", "readGlobalResult(std::ifstream &ifs): block inital placement is out of chip contour even after adjustments"},

    {"CORNERSTITCHING_01", "findPoint(): function's input key, the finding target, is out of canvas"},
    {"CORNERSTITCHING_02", "searchArea(): function's input box, the searching area, is out of canvas"},
    {"CORNERSTITCHING_03", "enumerateDirectArea(): function's input box, the searching area, is out of canvas"},
    {"CORNERSTITCHING_04", "CornerStitching(): input chipWidth or chipHeight should > 0"},
    {"CORNERSTITCHING_05", "visualiseArtpiece(): outputFileName invalid, cannot open file"},
    {"CORNERSTITCHING_06", "insertTile(): input tile prototype is out of canvas"},
    {"CORNERSTITCHING_07", "insertTile(): input tile's position already exist another tile on canvas"},
    {"CORNERSTITCHING_08", "cutTileHorizontally(): there is no enough height for newDownHeight to cut from origTop (victim tile)"},
    {"CORNERSTITCHING_09", "cutTileVertically(): there is no enough width for newLeftHeight to cut from origRight (victim tile)"},
    {"CORNERSTITCHING_10", "mergeTilesHorizontally(): input mergeUp is not above input mergeDown"},
    {"CORNERSTITCHING_11", "mergeTilesHorizontally(): two input tiles (mergeUp and mergeDown) are not mergable"},
    {"CORNERSTITCHING_12", "insertTile(): input tile's type shall not be tileType::BLANK, blank tile is prohibited from insertion"},
    {"CORNERSTITCHING_13", "mergeTilesVertically(): input mergeLeft is not at the left of input mergeRight"},
    {"CORNERSTITCHING_14", "mergeTilesVertically(): two input tiles (mergeLeft and mergeRight) are not mergable"},
    {"CORNERSTITCHING_15", "cutTileVertically(): victim tile"},

    
    {"CORNERSTITCHING_17", "removeTile(Tile *tile): input tile is not in the cornerStitching system"},
    {"CORNERSTITCHING_18", "removeTile(Tile *tile): cornerStitching exist a tile that has same LowerLeft Cord with input Tile* Tile"},

    {"RECTILINEAR_01", "calculateBoundingBox(): there exist no tile in rectilinear"},
    {"RECTILINEAR_02", "acquireWinding(): there exist no tile in rectilinear"},
    {"RECTILINEAR_03", "acquireWinding(): the rectilinear is not in one shape"},
    {"RECTILINEAR_04", "acquireWinding(): there exist hole in the rectilinear structure"},
    {"RECTILINEAR_05", "operator << (std::ostream &os, const rectilinearType &t): rectilinearType does not exist"},
    {"RECTILINEAR_06", "operator << (std::ostream &os, const rectilinearIllegalType &t): rectilinearIllegalType does not exist"},
    {"RECTILINEAR_07", "operator << (std::ostream &os, const windingDirection &t): windingDirection does not exist"},


    {"CONNECTION_01", "calculateCost(): Connection has < 2 vertices"},

    {"FLOORPLAN_01", "Floorplan(GlobalResult gr): undefined gr.type"},
    {"FLOORPLAN_02", "addBlockTile(const Rectangle &tilePosition, Rectilinear *rt): input tilePosition is not within the chip"},
    {"FLOORPLAN_03", "addBlockTile(const Rectangle &tilePosition, Rectilinear *rt): input Rectilinear *rt is not present in floorplan data structure"},
    {"FLOORPLAN_04", "addOverlapTile(...): input tilePosition is not within the chip"},
    {"FLOORPLAN_05", "addOverlapTile(...): payload containes some Rectilinear* that is not registered in the floorplan system"},
    {"FLOORPLAN_06", "deleteTile(Tile *tile): tile type should be tileType::BLOCK or tileType::OVERLAP"},
    {"FLOORPLAN_07", "deleteTile(Tile *tile): tile type is tile::BLOCK but not entry found in floorplan->blockTilePayload"},
    {"FLOORPLAN_08", "deleteTile(Tile *tile): tile type is tile::OVERLAP but not entry found in floorplan->overlapTilePayload"},
    {"FLOORPLAN_09", "increaseTileOverlap(Tile *tile, Rectilinear *newRect): tile has tileType::BLOCK but not logged in blocTilePayload, tile is unregistered"},
    {"FLOORPLAN_10", "increaseTileOverlap(Tile *tile, Rectilinear *newRect): tile has tileType::OVERLAP but not logged in overlapTilePayload, tile is unregistered"},
    {"FLOORPLAN_11", "increaseTileOverlap(Tile *tile, Rectilinear *newRect): tile's type is neither tileType::BLOCK nor tileType::OVERLAP"},
    {"FLOORPLAN_12", "decreaseTileOverlap(Tile *tile, Rectilinear *removeRect): tile's type is not tileType::OVERLAP"},
    {"FLOORPLAN_13", "decreaseTileOverlap(Tile *tile, Rectilinear *removeRect): tile has tileType::OVERLAP but not logged in overlapTilePayload, tile is unregistered"},
    {"FLOORPLAN_14", "decreaseTileOverlap(Tile *tile, Rectilinear *removeRect): the tile's payload (overlapTilePayload[tile]) does not include the removeal target, removeRect"},
    {"FLOORPLAN_15", "decreaseTileOverlap(Tile *tile, Rectilinear *removeRect): tile's type is tileType::OVERLAP but the payload in floorplan system has entry < 2"},
    {"FLOORPLAN_16", "placeRectilinear(...): input placement is not within the chip"},
    {"FLOORPLAN_17", "placeRectilinear(...): return tiles from cornerStitching.enumerateDirectArea() tile.type could only be tileType::BLOCK or tileType::OVERLAP"},
    {"FLOORPLAN_18", "divideTileHorizontally(Tile *origTop, len_t newDownHeight): Tile origTop cannot be tileType::BLANK type, only tileType::BLOCK or tileType::OVERLAP is accepted"},
    {"FLOORPLAN_19", "divideTileVertically(Tile *origRight, len_t newLeftWidth): Tile origRight cannot be tileType::BLANK type, only tileType::BLOCK or tileType::OVERLAP is accepted"},
    {"FLOORPLAN_20", "increaseTileOverlap(Tile *tile, Rectilinear *newRect): tile payload already contained such Rectilinear"},
};

class CSException : public std::exception{
private:
    std::string m_Exception_MSG;

public :
    CSException() = delete;
    CSException(std::string excpetion_msg);
    const char* what(); 
};

#endif // __CSEXCEPTION_H__