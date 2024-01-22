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
	try{

		globalPhaseAdapter gpa("./inputs/case06-output.txt");
		gpa.readGlobalResult();
		gpa.detectGlobalOverlaps();
		// gpa.printStatus();
		std::vector <Tile *> insertArr = gpa.printFlowStatus(false);
		std::cout << "This is inset Arr: " << std::endl;

		for(Tile *t : insertArr){
			std::cout << *t << std::endl;
		}

		CornerStitching cs(gpa.chipWidth, gpa.chipHeight);

		std::cout << std::endl << "Start Inserting Process" << std::endl;
		for(int i = 0; i < insertArr.size(); ++i){
			printf("Insert Tile No. %d\t", i);
			std::cout << *insertArr[i] << std::endl;
			if(i == 2){
				cs.insertTile(*insertArr[i], 1);
			}else{
				cs.insertTile(*insertArr[i], 0);
			}
			std::vector<std::pair<Tile *, Tile *>> failTiles;
			if(cs.checkMergingSuccess(failTiles)){
				std::cout << "Neo check result: N-Pass" << std::endl;
			}else{
				std::cout << "Neo check result: N-Fail" << std::endl;
			}
			// if(!failTiles.empty()){
			// 	for(std::pair<Tile *, Tile *>failPair : failTiles){
			// 		std::cout << "Fail Pair: " << *(failPair.first) << " " << *(failPair.second) << std::endl;
			// 	}
			// }else{
			// }
			bool traditonalCheck = cs.checkCombinableBlanks();
			std::cout << "Traditional check result: " << ((traditonalCheck)? "PASS" : "FAIL") << std::endl;
			std::string visuailizeFile = "./outputs/case06/case06-output-" + std::to_string(i) + ".txt";
			std::cout << "printing visualize to -> " << visuailizeFile << std::endl;
			cs.visualiseTileDistribution(visuailizeFile);
			
		}

	}catch(CSException e){
		std::cout << e.what() << std::endl;
		abort();
	}

	// try{
	// 	CornerStitching cs(100, 100);
	// 	cs.insertTile(Tile(tileType::BLOCK, Cord(75, 90), 25, 10), 0);
	// 	cs.visualiseTileDistribution("./outputs/case07/case07-output-0.txt");

	// 	cs.insertTile(Tile(tileType::BLOCK, Cord(40, 60), 35, 30), 0);
	// 	cs.visualiseTileDistribution("./outputs/case07/case07-output-1.txt");

	// 	cs.insertTile(Tile(tileType::BLOCK, Cord(40, 0), 35, 20), 0);
	// 	cs.visualiseTileDistribution("./outputs/case07/case07-output-2.txt");

	// 	cs.insertTile(Tile(tileType::BLOCK, Cord(20, 40), 10, 10), 0);
	// 	cs.visualiseTileDistribution("./outputs/case07/case07-output-3.txt");

	// 	cs.insertTile(Tile(tileType::BLOCK, Cord(85, 40), 10, 10), 0);
	// 	cs.visualiseTileDistribution("./outputs/case07/case07-output-4.txt");

	// 	std::cout << "Real thing comming up!" << std::endl;
	// 	cs.insertTile(Tile(tileType::BLOCK, Cord(40, 20), 35, 40), 0);
	// 	cs.visualiseTileDistribution("./outputs/case07/case07-output-5.txt");
	// }catch(CSException e){
	// 	std::cout << e.what() << std::endl;
	// 	abort();
	// }
	
}