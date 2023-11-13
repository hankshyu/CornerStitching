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
            this->fixedTesserae.push_back(nT);
        }else{
            throw std::out_of_range("Module not marked as SOFT or FIXED");
        }

    }

}

// If an overlap is detected, You should:
// 1. Locate the overlap and crate a new Tile marking the overlap, 
//  the tile should include the spacing info and the overlap Tessera idx
//  Tile *overlapTile = new Tile(tileType::OVERLAP, Cord(1,3), 4, 5);
//  overlapTile->OverlapFixedTesseraeIdx.pushback()....
//  overlapTile->OverlapSoftTesseraeIdx.pushback()....

// 2. Split (both) the Tesserae into smaller tiles if it become rectlinear.
// 3. Update (both) the Tesserae's tile list.
void globalPhaseAdapter::detectGlobalOverlaps(){
    using namespace boost::polygon::operators;
    namespace gtl = boost::polygon;

    gtl::connectivity_extraction_90<len_t> ce;

    std::vector<Rectangle> test_data;
    for ( Tessera curTes : this->softTesserae ) {
        test_data.push_back(curTes.origBox);
    }
    for ( Tessera curTes : this->fixedTesserae ) {
        test_data.push_back(curTes.origBox);
    }

    for ( unsigned int i = 0; i < test_data.size(); ++i ) {
        //insert returns an id starting at zero and incrementing
        //with each call
        assert(ce.insert(test_data[i]) == i);
    }
    //notice that ids returned by ce.insert happen to match
    //index into vector of inputs in this case

    //make sure the vector graph has elements for our nodes
    std::vector< std::set<len_t> > graph(test_data.size());

    //populate the graph with edge data
    ce.extract(graph);

    struct IntersectionUnit
    {
        Rectangle intersection;
        std::vector<int> overlappedIDs;
    };



}

void globalPhaseAdapter::printStatus(){
    for(Tessera t: this->softTesserae){
        std::cout << t.name << t.origBox << std::endl;
        std::cout << "(" << t.TileArr.size() <<  "/" << t.OverlapArr.size() << "):";
        std::cout << *(t.TileArr[0]) << std::endl;
    }
    for(Tessera t: this->fixedTesserae){
        std::cout << t.name << t.origBox << std::endl;
        std::cout << "(" << t.TileArr.size() <<  "/" << t.OverlapArr.size() << "):";
        std::cout << *(t.TileArr[0]) << std::endl;
    }
}