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
		Tile tileArr [12];
		tileArr[0] = Tile(tileType::BLOCK, Rectangle(7, 15, 15, 21));
		tileArr[1] = Tile(tileType::OVERLAP, Rectangle(15, 17, 21, 22));
		tileArr[2] = Tile(tileType::OVERLAP, Rectangle(5, 9, 13, 15));
		tileArr[3] = Tile(tileType::OVERLAP, Rectangle(13, 11, 15, 15));
		tileArr[4] = Tile(tileType::OVERLAP, Rectangle(15, 11, 21, 17));
		tileArr[5] = Tile(tileType::OVERLAP, Rectangle(21, 13, 27, 20));
		tileArr[6] = Tile(tileType::OVERLAP, Rectangle(13, 9, 21, 11));
		tileArr[7] = Tile(tileType::OVERLAP, Rectangle(7, 4, 17, 9));
		tileArr[8] = Tile(tileType::OVERLAP, Rectangle(17, 4, 21, 9));
		tileArr[9] = Tile(tileType::OVERLAP, Rectangle(21, 4, 27, 13));
		tileArr[10] = Tile(tileType::OVERLAP, Rectangle(27, 13, 29, 19));
		tileArr[11] = Tile(tileType::OVERLAP, Rectangle(7, 2, 12, 4));
		CornerStitching cs(30, 30);
		for(int i = 0; i < 12; i++){
			cs.insertTile(tileArr[i], 0);
		}
		Tile *tile1, *tile2;
		bool mergeSuccess = cs.debugBlankMerged(tile1, tile2);
		if(mergeSuccess){
			std::cout << "Merging successful!" << std::endl;
		}else{
			std::cout << " Merging fail!" << std::endl;
		}
		cs.visualiseTileDistribution("./outputs/case08/case08-output-0.txt");

		Rectangle recArr[10];
		recArr[0] = Rectangle(7, 4, 27, 19);
		recArr[1] = Rectangle(2, 3, 6, 6);
		recArr[2] = Rectangle(2, 3, 5, 6);
		recArr[3] = Rectangle(4, 4, 7, 6);


		for(int i = 0; i < 4; ++i){
			std::vector<Tile *> allTiles;
			cs.enumerateDirectedArea(recArr[i], allTiles);
			if(allTiles.empty()){
				std::cout << "allTiles are empty" << std::endl;
			}else{
				std::cout << "There are " << allTiles.size() << "blocks in " << recArr[i] << std::endl;
				for(int j = 0; j < allTiles.size(); ++j){
					std::cout << j << ": " << *allTiles[j] << std::endl;
				}
			}
		}


	}catch(CSException e){
		std::cout << e.what() << std::endl;
		abort();
	}
	
}