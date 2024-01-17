#ifndef __CSEXCEPTION_H__
#define __CSEXCEPTION_H__

#include <exception>
#include <unordered_map>
#include <string>

#define CSEX_EMPTY "CSEX: Empty Excpeiton"

/* Look Up Table for all exceptin messages */
inline std::unordered_map<std::string, const char*> CSEXCEPTION_LUT = {
    {"TILE_01", "operator<<: tileType out of Range"},

    {"GLOBALPHASEADAPTER_01", "realGlobalResult(): Module not marked as SOFT or FIXED"},

    {"CORNERSTITCHING_01", "findPoint(): function's input key, the finding target, is out of canvas"},
    {"CORNERSTITCHING_02", "searchArea(): function's input box, the searching area, is out of canvas"},
    {"CORNERSTITCHING_03", "enumerateDirectArea(): function's input box, the searching area, is out of canvas"},
    {"CORNERSTITCHING_04", "CornerStitching(): input chipWidth or chipHeight should > 0"},
    {"CORNERSTITCHING_05", "visualiseArtpiece(): outputFileName invalid, cannot open file"},
    {"CORNERSTITCHING_06", "insertTile(): input tile prototype is out of canvas"},
    {"CORNERSTITCHING_07", "insertTile(): input tile's position already exist another tile on canvas"},
    {"CORNERSTITCHING_08", "cutBlankTileHorizontally(): victim tile (origTop) is not tileType::BLANK"},
    {"CORNERSTITCHING_09", "cutBlankTileHorizontally(): ther is no enough height for newDownHeight to cut from origTop (victim tile)"},

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