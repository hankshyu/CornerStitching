#ifndef __GLOBALPHASEADAPTER_H__
#define __GLOBALPHASEADAPTER_H__

#include <fstream>
#include <string>
#include <vector>

#include "tile.h"

enum class tesseraType{
    EMPTY ,SOFT, HARD, OVERLAP
};

struct Tessera{
    std::string name;
    tesseraType type;
    Rectangle origBox;

    std::vector <Tile *> TileArr;
    std::vector <Tile *> OverlapArr; 
};

class globalPhaseAdapter{

private:
    std::ifstream ifs;
public:
    int totalTesseraeNum;
    int totalConnNum;
    len_t chipWidth, chipHeight;
    std::vector<Tessera> softTesserae;
    std::vector<Tessera> fixedTesserae;
    
    globalPhaseAdapter() = delete;
    globalPhaseAdapter(std::string fileName);
    ~globalPhaseAdapter();

    void readGlobalResult();
    void detectGlobalOverlaps();
    void splitGlobalOverlaps();

    void printStatus();
    
};

#endif // __GLOBALPHASEADAPTER_H__