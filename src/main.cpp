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

		globalPhaseAdapter gpa("./inputs/case02-output.txt");
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
			if(i <= 2){
				printf("Insert Tile No. %d\t", i);
				std::cout << *insertArr[i] << std::endl;
				cs.insertTile(*insertArr[i]);
				std::vector<std::pair<Tile *, Tile *>> failTiles;
				cs.checkMergingSuccess(failTiles);
				if(!failTiles.empty()){
					for(std::pair<Tile *, Tile *>failPair : failTiles){
						std::cout << "Fail Pair: " << *(failPair.first) << " " << *(failPair.second) << std::endl;
					}
				}
				std::string visuailizeFile = "./outputs/case02/case02-output-" + std::to_string(i) + ".txt";
				std::cout << "printing visualize to -> " << visuailizeFile << std::endl;
				cs.visualiseTileDistribution(visuailizeFile);
			}
		}

	}catch(CSException e){
		std::cout << e.what() << std::endl;
		abort();
	}
	// Tile *m1 = new Tile(tileType::BLANK, Cord(0, 5), 3, 7);
	// Tile *m2 = new Tile(tileType::BLANK, Cord(3, 7), 1, 9);
	// std::unordered_set<Tile *> us;
	// us.insert(m1);
	// std::cout << us.size() << std::endl;
	// for(Tile *t : us){
	// 	std::cout << *t << std::endl;
	// }

	// bool f1 = (us.find(m1) == us.end());
	// bool f2 = (us.find(m2) == us.end());
	// std::cout << "f1: " << f1 <<" f2: " << f2 << std::endl;
	
}