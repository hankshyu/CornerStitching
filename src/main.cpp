#include <iostream>
#include <vector>

#include "boost/polygon/polygon.hpp"
#include "colours.h"
#include "units.h"

#include "cord.h"
#include "line.h"
#include "rectangle.h"
#include "tile.h"
#include "globalPhaseAdapter.h"
#include "cornerStitching.h"
#include "cSException.h"

#include <unordered_map>
#include <unordered_set>

int main(int argc, char const *argv[]) {
	// try{

	// 	globalPhaseAdapter gpa("./inputs/case06-output.txt");
	// 	gpa.readGlobalResult();
	// 	gpa.detectGlobalOverlaps();
	// 	// gpa.printStatus();
	// 	std::vector <Tile *> insertArr = gpa.printFlowStatus(false);
	// 	std::cout << "This is inset Arr: " << std::endl;

	// 	for(Tile *t : insertArr){
	// 		std::cout << *t << std::endl;
	// 	}

	// 	CornerStitching cs(gpa.chipWidth, gpa.chipHeight);

	// 	std::cout << std::endl << "Start Inserting Process" << std::endl;
	// 	for(int i = 0; i < insertArr.size(); ++i){
	// 		printf("Insert Tile No. %d\t", i);
	// 		std::cout << *insertArr[i] << std::endl;
	// 		if(i == 2){
	// 			cs.insertTile(*insertArr[i], 1);
	// 		}else{
	// 			cs.insertTile(*insertArr[i], 0);
	// 		}
	// 		std::vector<std::pair<Tile *, Tile *>> failTiles;
	// 		if(cs.checkMergingSuccess(failTiles)){
	// 			std::cout << "Neo check result: N-Pass" << std::endl;
	// 		}else{
	// 			std::cout << "Neo check result: N-Fail" << std::endl;
	// 		}
	// 		// if(!failTiles.empty()){
	// 		// 	for(std::pair<Tile *, Tile *>failPair : failTiles){
	// 		// 		std::cout << "Fail Pair: " << *(failPair.first) << " " << *(failPair.second) << std::endl;
	// 		// 	}
	// 		// }else{
	// 		// }
	// 		bool traditonalCheck = cs.checkCombinableBlanks();
	// 		std::cout << "Traditional check result: " << ((traditonalCheck)? "PASS" : "FAIL") << std::endl;
	// 		std::string visuailizeFile = "./outputs/case06/case06-output-" + std::to_string(i) + ".txt";
	// 		std::cout << "printing visualize to -> " << visuailizeFile << std::endl;
	// 		cs.visualiseTileDistribution(visuailizeFile);
			
	// 	}

	// }catch(CSException e){
	// 	std::cout << e.what() << std::endl;
	// 	abort();
	// }

	try{
		// Tile tileArr [12];
		// tileArr[0] = Tile(tileType::BLOCK, Rectangle(7, 15, 15, 21));
		// tileArr[1] = Tile(tileType::OVERLAP, Rectangle(15, 17, 21, 22));
		// tileArr[2] = Tile(tileType::OVERLAP, Rectangle(5, 9, 13, 15));
		// tileArr[3] = Tile(tileType::OVERLAP, Rectangle(13, 11, 15, 15));
		// tileArr[4] = Tile(tileType::BLOCK, Rectangle(15, 11, 21, 17));
		// tileArr[5] = Tile(tileType::OVERLAP, Rectangle(21, 13, 27, 20));
		// tileArr[6] = Tile(tileType::OVERLAP, Rectangle(13, 9, 21, 11));
		// tileArr[7] = Tile(tileType::BLOCK, Rectangle(7, 4, 17, 9));
		// tileArr[8] = Tile(tileType::OVERLAP, Rectangle(17, 4, 21, 9));
		// tileArr[9] = Tile(tileType::OVERLAP, Rectangle(21, 4, 27, 13));
		// tileArr[10] = Tile(tileType::OVERLAP, Rectangle(27, 13, 29, 19));
		// tileArr[11] = Tile(tileType::BLOCK, Rectangle(7, 2, 12, 4));

		CornerStitching cs(100, 65);
		cs.insertTile(Tile(tileType::BLOCK, Cord(15, 5), 75, 10));
		cs.insertTile(Tile(tileType::BLOCK, Cord(7, 38), 25, 14));
		cs.visualiseTileDistribution("./outputs/case09/case09-output-0.txt");
		Tile *todelete = cs.insertTile(Tile(tileType::BLOCK, Cord(40, 23), 20, 37));
		cs.insertTile(Tile(tileType::BLOCK, Cord(66, 25), 29, 20));

		// CornerStitching cs(100, 100);
		// cs.insertTile(Tile(tileType::BLOCK, Cord(10, 10), 10, 15));
		// Tile *todelete = cs.insertTile(Tile(tileType::BLOCK, Cord(10, 25), 10, 20));
		// cs.insertTile(Tile(tileType::BLOCK, Cord(10, 45), 10, 25));

		// cs.insertTile(Tile(tileType::OVERLAP, Cord(55, 10), 20, 20));
		// cs.insertTile(Tile(tileType::OVERLAP, Cord(55, 30), 20, 25));
		// cs.insertTile(Tile(tileType::OVERLAP, Cord(55, 55), 20, 15));

		Tile *tile1, *tile2;
		bool mergeSuccess = cs.debugBlankMerged(tile1, tile2);
		std::cout << ((mergeSuccess)? "Merging successful" : "Merging fail!") << std::endl;

		cs.visualiseTileDistribution("./outputs/case09/case09-output-1.txt");
		cs.removeTile(todelete);
		cs.visualiseTileDistribution("./outputs/case09/case09-output-2.txt");

	}catch(CSException e){
		std::cout << e.what() << std::endl;
		abort();
	}
	
}