#include <cmath>
#include <unordered_set>
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

        Tessera *nT = new Tessera();
        nT->name = storeName;
        Rectangle initBox (len_t(initX), len_t(initY), len_t(initX)+initW, len_t(initY)+initH);

        nT->origBox = initBox;
        nT->TileArr.push_back(new Tile(tileType::BLOCK, initBox));
        
        if(tword == "SOFT"){
            nT->type = tesseraType::SOFT;
            this->softTesserae.push_back(nT);
        }else if(tword == "FIXED"){
            nT->type = tesseraType::HARD;
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

    typedef gtl::polygon_90_set_data <len_t> PolygonSet;

    std::vector<Rectangle> test_data;
    for ( const Tessera *curTes : this->softTesserae ) {
        test_data.push_back(curTes->origBox);
    }
    for ( const Tessera *curTes : this->fixedTesserae ) {
        test_data.push_back(curTes->origBox);
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

    // 2 overlapped
    std::vector<IntersectionUnit> overlap2unit;
    for ( int i = 0; i < test_data.size(); i++ ) {
        for ( int n : graph[i] ) {
            if ( n < i ) {
                continue;
            }
            PolygonSet intersections;
            // gtl::polygon_90_set_data <len_t> intersections;
            intersections += test_data[i] & test_data[n];
            if ( intersections.empty() ) {
                continue;
            }
            Rectangle intersectBox;
            gtl::extents(intersectBox, intersections);
            IntersectionUnit iu;
            iu.intersection = intersectBox;
            iu.overlappedIDs.push_back(i);
            iu.overlappedIDs.push_back(n);
            overlap2unit.push_back(iu);
        }
    }

    // 3 overlapped
    std::vector<IntersectionUnit> overlap3unit;
    for ( IntersectionUnit &o2unit: overlap2unit ) {
        int olID0 = o2unit.overlappedIDs[0], olID1 = o2unit.overlappedIDs[1];
        for ( int n : graph[olID1] ) {
            if ( n <= olID1 ) {
                continue;
            }
            PolygonSet intersections;
            intersections += o2unit.intersection & test_data[n];
            if ( intersections.empty() ) {
                continue;
            }
            printf("%2d %2d %2d\n", olID0, olID1, n);
            // std::cout << "3 Modules Intersection: " << intersections << std::endl;
            Rectangle intersectBox;
            gtl::extents(intersectBox, intersections);
            IntersectionUnit iu;
            iu.intersection = intersectBox;
            iu.overlappedIDs.push_back(olID0);
            iu.overlappedIDs.push_back(olID1);
            iu.overlappedIDs.push_back(n);
            overlap3unit.push_back(iu);
        }
    }

    // 4 overlapped
    std::vector<IntersectionUnit> overlap4unit;
    for ( IntersectionUnit &o3unit: overlap3unit ) {
        int olID0 = o3unit.overlappedIDs[0], olID1 = o3unit.overlappedIDs[1], olID2 = o3unit.overlappedIDs[2];
        for ( int n : graph[olID2] ) {
            if ( n <= olID2 ) {
                continue;
            }
            PolygonSet intersections;
            intersections += o3unit.intersection & test_data[n];
            if ( intersections.empty() ) {
                continue;
            }
            printf("%2d %2d %2d %2d\n", olID0, olID1, olID2, n);
            // std::cout << "4 Modules Intersection: " << intersections << std::endl;
            Rectangle intersectBox;
            gtl::extents(intersectBox, intersections);
            IntersectionUnit iu;
            iu.intersection = intersectBox;
            iu.overlappedIDs.push_back(olID0);
            iu.overlappedIDs.push_back(olID1);
            iu.overlappedIDs.push_back(olID2);
            iu.overlappedIDs.push_back(n);
            overlap4unit.push_back(iu);
        }
    }

    // add these tiles
    std::vector<Tile> overlap4TileVec;
    for ( IntersectionUnit &o4unit: overlap4unit ) {
        Rectangle intersectBox = o4unit.intersection;
        len_t x = gtl::xl(intersectBox);
        len_t y = gtl::yl(intersectBox);
        len_t w = gtl::xh(intersectBox) - gtl::xl(intersectBox);
        len_t h = gtl::yh(intersectBox) - gtl::yl(intersectBox);
        Tile overlapTileRef(tileType::OVERLAP, Cord(x, y), w, h);
        overlap4TileVec.push_back(overlapTileRef);

        Tile *overlapTile = new Tile(tileType::OVERLAP, Cord(x, y), w, h);
        for ( int i : o4unit.overlappedIDs ) {
            bool isSoft = i < softTesserae.size();
            int id = ( isSoft ) ? i : i - softTesserae.size();
            Tessera *curTess = ( isSoft ) ? softTesserae[id] : fixedTesserae[id];

            if(isSoft){
                overlapTile->OverlapSoftTesseraeIdx.push_back(id);
            }else{
                overlapTile->OverlapFixedTesseraeIdx.push_back(id);
            }

            curTess->insertTiles(overlapTile);
        }
    }

    std::vector<Tile> overlap3TileVec;
    for ( IntersectionUnit &o3unit: overlap3unit ) {
        Rectangle intersectBox = o3unit.intersection;
        len_t x = gtl::xl(intersectBox);
        len_t y = gtl::yl(intersectBox);
        len_t w = gtl::xh(intersectBox) - gtl::xl(intersectBox);
        len_t h = gtl::yh(intersectBox) - gtl::yl(intersectBox);
        Tile overlapTileRef(tileType::OVERLAP, Cord(x, y), w, h);
        overlap3TileVec.push_back(overlapTileRef);

        std::vector<Tile> interection3TileVec;
        interection3TileVec.push_back(Tile(tileType::OVERLAP, Cord(x, y), w, h));
        std::vector<Tile> cuttedTiles = mergeCutTiles(interection3TileVec, overlap4TileVec);

        for ( auto &tile : cuttedTiles ) {
            Tile *overlapTile = new Tile(tileType::OVERLAP, tile.getLowerLeft(), tile.getWidth(), tile.getHeight());
            for ( int i : o3unit.overlappedIDs ) {
                bool isSoft = i < softTesserae.size();
                int id = ( isSoft ) ? i : i - softTesserae.size();
                Tessera *curTess = ( isSoft ) ? softTesserae[id] : fixedTesserae[id];

                ( isSoft ) ? overlapTile->OverlapSoftTesseraeIdx.push_back(id)
                    : overlapTile->OverlapFixedTesseraeIdx.push_back(id);
                curTess->insertTiles(overlapTile);
            }
        }
    }

    for ( IntersectionUnit &o2unit: overlap2unit ) {
        Rectangle intersectBox = o2unit.intersection;
        len_t x = gtl::xl(intersectBox);
        len_t y = gtl::yl(intersectBox);
        len_t w = gtl::xh(intersectBox) - gtl::xl(intersectBox);
        len_t h = gtl::yh(intersectBox) - gtl::yl(intersectBox);

        std::vector<Tile> interection2TileVec;
        interection2TileVec.push_back(Tile(tileType::OVERLAP, Cord(x, y), w, h));
        std::vector<Tile> cuttedTiles = mergeCutTiles(interection2TileVec, overlap3TileVec);

        for ( auto &tile : cuttedTiles ) {
            Tile *overlapTile = new Tile(tileType::OVERLAP, tile.getLowerLeft(), tile.getWidth(), tile.getHeight());
            for ( int i : o2unit.overlappedIDs ) {
                bool isSoft = i < softTesserae.size();
                int id = ( isSoft ) ? i : i - softTesserae.size();
                Tessera *curTess = ( isSoft ) ? softTesserae[id] : fixedTesserae[id];

                ( isSoft ) ? overlapTile->OverlapSoftTesseraeIdx.push_back(id)
                    : overlapTile->OverlapFixedTesseraeIdx.push_back(id);
                curTess->insertTiles(overlapTile);
            }
        }
    }

}

void globalPhaseAdapter::splitGlobalOverlaps(){
    // Soft&Hard block overlap are located and split if necessary in OverlapArr of each Tessera
    // now cut rectlinear blank space of each Tessera into multiple blank tiles.

    for(Tessera *fixedTess : this->fixedTesserae){
        fixedTess->splitRectliearDueToOverlap();
    }

    for(Tessera *softTess : this->softTesserae){
        softTess->splitRectliearDueToOverlap();
    }
}


void globalPhaseAdapter::printStatus(){
    for(Tessera *tess: this->softTesserae){
        Tessera t = *tess;
        std::cout << t.name << t.origBox << std::endl;
        std::cout << "(" << t.TileArr.size() <<  "/" << t.OverlapArr.size() << "):";
        std::cout << "TileArr(" << t.TileArr.size() << "):" << std::endl;
        if(!t.TileArr.empty()){
            for(Tile *til : t.TileArr){
                std::cout << *til << std::endl;
            }
        }
        std::cout << "OverlapArr(" << t.OverlapArr.size() << "):" << std::endl;
        if(!t.OverlapArr.empty()){
            for(Tile *til : t.OverlapArr){
                std::cout << *til << std::endl;
            }
        }
    }
    for(Tessera *tess: this->fixedTesserae){
        Tessera t = *tess;
        std::cout << t.name << t.origBox << std::endl;
        std::cout << "(" << t.TileArr.size() <<  "/" << t.OverlapArr.size() << "):";
        std::cout << "TileArr(" << t.TileArr.size() << "):" << std::endl;
        if(!t.TileArr.empty()){
            for(Tile *til : t.TileArr){
                std::cout << *til << std::endl;
            }
        }
        std::cout << "OverlapArr(" << t.OverlapArr.size() << "):" << std::endl;
        if(!t.OverlapArr.empty()){
            for(Tile *til : t.OverlapArr){
                std::cout << *til << std::endl;
            }
        }
    }
}

std::vector<Tile *> globalPhaseAdapter::printFlowStatus(){
    // This records the inserted Rectangels
    std::unordered_set<Rectangle> record;    
    std::vector <Tile *> answer;

    for(Tessera *tess: this->softTesserae){
        Tessera t = *tess;
        std::cout << t.name << t.origBox << std::endl;
        std::cout << "(" << t.TileArr.size() <<  "/" << t.OverlapArr.size() << "):";
        std::cout << "TileArr(" << t.TileArr.size() << "):" << std::endl;
        if(!t.TileArr.empty()){
            for(Tile *til : t.TileArr){
                std::cout << *til << std::endl;
                Rectangle trec = til->getRectangle();
                if(record.find(trec) == record.end()){
                    record.insert(til->getRectangle());
                    answer.push_back(til);

                }else{
                    std::cout << "(Tile existed)" << std::endl;
                }
            }
        }
        std::cout << "OverlapArr(" << t.OverlapArr.size() << "):" << std::endl;
        if(!t.OverlapArr.empty()){
            for(Tile *til : t.OverlapArr){
                std::cout << *til << std::endl;
                Rectangle trec = til->getRectangle();
                if(record.find(trec) == record.end()){
                    record.insert(til->getRectangle());
                    answer.push_back(til);
                }else{
                    std::cout << "(Tile existed)" << std::endl;
                }
            }
        }
    }
    for(Tessera *tess: this->fixedTesserae){
        Tessera t = *tess;
        std::cout << t.name << t.origBox << std::endl;
        std::cout << "(" << t.TileArr.size() <<  "/" << t.OverlapArr.size() << "):";
        std::cout << "TileArr(" << t.TileArr.size() << "):" << std::endl;
        if(!t.TileArr.empty()){
            for(Tile *til : t.TileArr){
                std::cout << *til << std::endl;
                Rectangle trec = til->getRectangle();
                if(record.find(trec) == record.end()){
                    record.insert(til->getRectangle());
                    answer.push_back(til);
                }else{
                    std::cout << "(Tile existed)" << std::endl;
                }
            }
        }
        std::cout << "OverlapArr(" << t.OverlapArr.size() << "):" << std::endl;
        if(!t.OverlapArr.empty()){
            for(Tile *til : t.OverlapArr){
                std::cout << *til << std::endl;
                Rectangle trec = til->getRectangle();
                if(record.find(trec) == record.end()){
                    record.insert(til->getRectangle());
                    answer.push_back(til);
                }else{
                    std::cout << "(Tile existed)" << std::endl;
                }
            }
        }
    } 

    return answer;
}