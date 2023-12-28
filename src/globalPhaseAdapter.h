#ifndef __GLOBALPHASEADAPTER_H__
#define __GLOBALPHASEADAPTER_H__

#include <fstream>
#include <string>
#include <vector>

#include "tile.h"
#include "tessera.h"

class globalPhaseAdapter{

private:
    std::ifstream ifs;
public:
    int totalTesseraeNum;
    int totalConnNum;
    len_t chipWidth, chipHeight;
    std::vector<Tessera *> softTesserae;
    std::vector<Tessera *> fixedTesserae;
    
    globalPhaseAdapter() = delete;
    globalPhaseAdapter(std::string fileName);
    ~globalPhaseAdapter();

    void readGlobalResult();
    void detectGlobalOverlaps();
    void splitGlobalOverlaps();

    void printStatus();
    // prints wherter insertion is valid
    std::vector<Tile *> printFlowStatus();

    
};

#endif // __GLOBALPHASEADAPTER_H__