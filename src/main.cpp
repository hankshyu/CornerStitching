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

		Tile tileArr[8];
		tileArr[0] = Tile(tileType::BLOCK, Cord(0, 5), 10, 10);
		tileArr[1] = Tile(tileType::OVERLAP, Cord(10, 0), 10, 5);
		tileArr[2] = Tile(tileType::BLOCK, Cord(20, 5), 10, 5);
		tileArr[3] = Tile(tileType::OVERLAP, Cord(10, 15), 10, 10);
		tileArr[4] = Tile(tileType::BLOCK, Cord(20, 30), 10, 5);
		tileArr[5] = Tile(tileType::BLOCK, Cord(10, 25), 10, 10);
		tileArr[6] = Tile(tileType::OVERLAP, Cord(10, 11), 10, 1);
		tileArr[7] = Tile(tileType::BLOCK, Cord(10, 13), 10, 2);

		CornerStitching cs(30, 35);
		Tile *allP[8];
		for(int i = 0; i < 8; i++){
			allP[i] = cs.insertTile(tileArr[i]);
			cs.conductSelfTest();
		}
		cs.visualiseTileDistribution("./outputs/case09/case09-output-0.txt");

		// std::unordered_set<int> deleteIdx = {2, 7, 4, 0};
		std::unordered_set<int> deleteIdx = {0};
		// for(int idx : deleteIdx){
		// 	std::cout << "cs removing ... " << idx << std::endl;
		// 	cs.removeTile(allP[idx]);
		// 	cs.conductSelfTest();
		// }

		cs.removeTile(allP[0]);
		std::cout << "remove one: " << cs.conductSelfTest() << std::endl;
		cs.visualiseTileDistribution("./outputs/case09/case09-output-1.txt");

		// cs.removeTile(allP[4]);
		// std::cout << "remove two: " << cs.conductSelfTest() << std::endl;
		// cs.visualiseTileDistribution("./outputs/case09/case09-output-2.txt");

		// cs.removeTile(allP[7]);
		// std::cout << "remove three: " << cs.conductSelfTest() << std::endl;
		// cs.visualiseTileDistribution("./outputs/case09/case09-output-3.txt");

		// cs.removeTile(allP[2]);
		// std::cout << "remove four: " << cs.conductSelfTest() << std::endl;
		// cs.visualiseTileDistribution("./outputs/case09/case09-output-4.txt");

		CornerStitching cs1(30, 35);
		for(int i = 0; i < 8; i++){
			if(deleteIdx.find(i) != deleteIdx.end()) continue;
			std::cout << "cs1 inserting ... " << i << std::endl;
			cs1.insertTile(tileArr[i]);
			cs1.conductSelfTest();
		}
		cs1.visualiseTileDistribution("./outputs/case09/case09-output-9.txt");
		
		if(cs1 == cs) printf("After verification the two are equal\n");
		else printf("No!! The two corner stitching system are not equal!\n");

	}catch(CSException e){
		std::cout << e.what() << std::endl;
		abort();
	}
	
}