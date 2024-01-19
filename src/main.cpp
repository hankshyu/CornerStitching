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
		std::vector <Tile *> insertArr = gpa.printFlowStatus(true);
		std::cout << "This is inset Arr: " << std::endl;

		for(Tile *t : insertArr){
			std::cout << *t << std::endl;
		}

		CornerStitching cs(gpa.chipWidth, gpa.chipHeight);

		std::cout << std::endl << "Start Inserting Process" << std::endl;
		for(int i = 0; i < insertArr.size(); ++i){
			printf("Insert Tile No. %2d", i);
			std::cout << *insertArr[i] << std::endl;
			cs.insertTile(*insertArr[i]);
		}
	}catch(CSException e){
		std::cout << e.what() << std::endl;
		abort();
	}
}