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


}