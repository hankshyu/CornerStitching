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
	globalPhaseAdapter gpa("./inputs/case02-output.txt");
	gpa.readGlobalResult();
	gpa.detectGlobalOverlaps();
	// gpa.printStatus();
	std::vector <Tile *> insertArr = gpa.printFlowStatus();
	std::cout << "This is inset Arr: " << std::endl;

	for(Tile *t : insertArr){
		std::cout << *t << std::endl;
	}

	Tile tt(tileType::BLOCK, Cord(3, 7), 4, 5);
	std::cout << "This is the tt: " << tt << std::endl;
	std::cout << "This is lower left: " << tt.getLowerLeft() << std::endl;
	std::cout << "This is upper left: " << tt.getUpperLeft() << ", " << tt.getContainedUpperLeft() << std::endl;
	std::cout << "This is lower right: " << tt.getLowerRight() << ", " << tt.getContainedLowerRight() << std::endl;
	std::cout << "This is upper right: " << tt.getUpperRight() << ", " << tt.getContainedUpperRight() << std::endl;

	try{
		throw CSException("TILE_01");
	}catch(CSException cs){
		std::cout << cs.what() << std::endl;
	}
}