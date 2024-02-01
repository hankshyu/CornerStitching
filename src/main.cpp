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
#include <random>
void genEncoder(int bits, unsigned int num, std::vector<int> &answer){
	unsigned int filter = 1;
	for(int i = 0; i < bits ; ++i){
		if(num & filter) answer.push_back(i);
		filter = filter << 1;
	}
}
// void genRandomEncoder(int bits, std::vector<int> &answer){
// 	for(int i = 0; i < bits; ++i){
// 		answer.push_back(i);
// 	}


// }
int main(int argc, char const *argv[]) {

	try{



	}catch(CSException e){
		std::cout << e.what() << std::endl;
		abort();
	}

	std::random_device rd1, rd2;
	
	std::mt19937 generator1(rd1());
	std::mt19937 generator2(rd2());

	const int totalTiles = 20;
	std::uniform_int_distribution<int> normInt1(0, totalTiles);


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


		// std::cout << std::endl << "Start Inserting Process" << std::endl;
		// for(int i = 0; i < insertArr.size(); ++i){
		// 	printf("Insert Tile No. %d\t", i);
		// 	std::cout << *insertArr[i] << std::endl;
		// }

		const int arrSize = insertArr.size();
		const int VERIFICATION_ROUND = 1000000;
		const len_t CHIP_WIDTH = gpa.chipWidth;
		const len_t CHIP_HEIGHT = gpa.chipHeight;
		for(int veriRound = 0; veriRound < VERIFICATION_ROUND; ++veriRound){
			// Tile tilearr [arrSize] = {
				// Tile(tileType::BLOCK, Rectangle(0, 0, 20, 10)),
				// Tile(tileType::OVERLAP, Rectangle(0, 20, 20, 30)),
				// Tile(tileType::OVERLAP, Rectangle(0, 40, 20, 50)),

				// Tile(tileType::BLOCK, Rectangle(7, 15, 15, 21)),
				// Tile(tileType::OVERLAP, Rectangle(15, 17, 21, 22)),
				// Tile(tileType::OVERLAP, Rectangle(5, 9, 13, 15)),
				// Tile(tileType::OVERLAP, Rectangle(13, 11, 15, 15)),
				// Tile(tileType::BLOCK, Rectangle(15, 11, 21, 17)),
				// Tile(tileType::OVERLAP, Rectangle(21, 13, 27, 20)),
				// Tile(tileType::BLOCK, Rectangle(13, 9, 21, 11)),
				// Tile(tileType::BLOCK, Rectangle(7, 4, 17, 9)),
				// Tile(tileType::BLOCK, Rectangle(17, 4, 21, 9)),
				// Tile(tileType::OVERLAP, Rectangle(21, 4, 27, 13)),
				// Tile(tileType::BLOCK, Rectangle(27, 13, 29, 19)),
				// Tile(tileType::OVERLAP, Rectangle(7, 2, 12, 4)),

				// Tile(tileType::BLOCK, Cord(0, 5), 10, 10),
				// Tile(tileType::OVERLAP, Cord(10, 0), 10, 5),
				// Tile(tileType::BLOCK, Cord(20, 5), 10, 5),
				// Tile(tileType::OVERLAP, Cord(10, 15), 10, 10),
				// Tile(tileType::BLOCK, Cord(20, 30), 10, 5),
				// Tile(tileType::BLOCK, Cord(10, 25), 10, 10),
				// Tile(tileType::OVERLAP, Cord(10, 11), 10, 1),

				// Tile(tileType::OVERLAP, Rectangle(4, 15, 12, 22)),
				// Tile(tileType::BLOCK, Rectangle(11, 9, 16, 15)),
				// Tile(tileType::BLOCK, Rectangle(8, 3, 13, 9)),
				// Tile(tileType::BLOCK, Rectangle(13, 5, 14, 9)),
				// Tile(tileType::OVERLAP, Rectangle(16, 4, 22, 8)),
				// Tile(tileType::BLOCK, Rectangle(16, 17, 22, 20)),
				// Tile(tileType::BLOCK, Rectangle(16, 8, 22, 17)),


			// 	Tile(tileType::OVERLAP, Rectangle(7, 20, 9, 24)),
			// 	Tile(tileType::BLOCK, Rectangle(11, 11, 13, 13)),
			// };


			CornerStitching cs(CHIP_WIDTH, CHIP_HEIGHT);
			CornerStitching cs1(cs);

			Tile *allp[arrSize];
			for(int i = 0; i < arrSize; i++){
				// allp[i] = cs.insertTile(tilearr[i]);
				allp[i] = cs.insertTile(*insertArr[i]);
				cs.conductSelfTest();
			}
			// cs.visualiseTileDistribution("./outputs/case09/case09-output-0.txt");

			std::vector<int> deleteidx;
			// genEncoder(arrSize, veriRound, deleteidx);

			for(int i = 0; i < arrSize; ++i){
				deleteidx.push_back(i);
			}
			std::random_device rd1, rd2;
			std::mt19937 gen1(rd1());
			std::mt19937 gen2(rd2());
			std::binomial_distribution<int> normInt1(arrSize, 0.5);
			int removeCount = normInt1(gen1);
			for(int rmIdx = 0; rmIdx < removeCount; ++rmIdx){
				std::uniform_int_distribution<int> normInt2(0, deleteidx.size() - 1);
				deleteidx.erase(deleteidx.begin() + normInt2(gen2));
			}
				
			std::cout << "Experiment " << veriRound <<  " (" << double(veriRound*100)/VERIFICATION_ROUND << "%)" << " [";
			for(int i : deleteidx){
				std::cout << i << ", ";
			}
			std::cout << "]";

			for(int idx : deleteidx){
				// std::cout << "cs removing ... " << idx << std::endl;
				cs.removeTile(allp[idx]);
				cs.conductSelfTest();
			}

			// cs.removeTile(allp[1]);
			// std::cout << "remove one: " << cs.conductSelfTest() << std::endl;
			// cs.visualiseTileDistribution("./outputs/case09/case09-output-1.txt");



			for(int i = arrSize - 1; i >= 0; --i){
				if(std::find(deleteidx.begin(), deleteidx.end(), i) != deleteidx.end()) continue;
				// std::cout << "cs1 inserting ... " << i << std::endl;
				// cs1.insertTile(tilearr[i]);
				cs1.insertTile(*insertArr[i]);
				cs1.conductSelfTest();
			}
			// cs1.visualiseTileDistribution("./outputs/case09/case09-output-9.txt");
			
			if(cs1 == cs){
				std::cout << "PASS" << std::endl;
			}else {
				std::cout << "FAIL" << std::endl;
				abort();
			}
		}
	}catch(CSException e){
		std::cout << e.what() << std::endl;
		abort();
	}
	
}