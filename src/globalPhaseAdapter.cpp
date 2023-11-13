#include <cmath>
#include "globalPhaseAdapter.h"
#include "rectangle.h"


globalPhaseAdapter::globalPhaseAdapter(std::string fileName){
    this->ifs.open(fileName);
}

globalPhaseAdapter::~globalPhaseAdapter(){
    this->ifs.close();
}

void globalPhaseAdapter::readGlobalResult() {
    int blockNum = 0;
    std::string tword;
    int tnum;

    ifs >> tword >> this->totalTesseraeNum >> tword >> this->totalConnNum;
    ifs >> chipWidth >> chipHeight;
    
    std::cout << this->totalTesseraeNum << ", " << this->totalConnNum << std::endl;
    std::cout << this->chipWidth << ", " << this->chipHeight << std::endl;
    for (int i = 0; i < this->totalTesseraeNum; i++)
    {
        std::string storeName;
        double initX, initY;
        len_t initW, initH;

        ifs >> storeName >> tword >> initX >> initY >> initW >> initH;
        initX = round(initX);
        initY = round(initY);

        Tessera nT;
        nT.name = storeName;
        Rectangle initBox (len_t(initX), len_t(initY), len_t(initX)+initW, len_t(initY)+initH);

        nT.origBox = initBox;
        nT.TileArr.push_back(new Tile(tileType::BLOCK, initBox));
        
        if(tword == "SOFT"){
            nT.type = tesseraType::SOFT;
            this->softTesserae.push_back(nT);
        }else if(tword == "FIXED"){
            nT.type = tesseraType::HARD;
            this->hardTesserae.push_back(nT);
        }else{
            throw std::out_of_range("Module not marked as SOFT or FIXED");
        }

    }

    for(Tessera t: this->softTesserae){
        std::cout << t.name << t.origBox << std::endl;
        std::cout << "(" << t.TileArr.size() <<  "/" << t.OverlapArr.size() << "):";
        std::cout << *(t.TileArr[0]) << std::endl;
    }
    for(Tessera t: this->hardTesserae){
        std::cout << t.name << t.origBox << std::endl;
        std::cout << "(" << t.TileArr.size() <<  "/" << t.OverlapArr.size() << "):";
        std::cout << *(t.TileArr[0]) << std::endl;
    }
    
}