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
	// globalPhaseAdapter gpa("./inputs/case02-output.txt");
	// gpa.readGlobalResult();
	// gpa.detectGlobalOverlaps();
	// gpa.printStatus();

	std::unordered_map <Cord, Tile *> testmap;
	std::unordered_set <Tile *> testset;
	Tile *t1 = new Tile(tileType::BLOCK, Cord(3,5), 4, 7);
	Tile *t2 = new Tile(tileType::BLANK, Cord(13,15), 14, 17);
	
	testmap[t1->getLowerLeft()] = t1;
	testmap[t2->getLowerLeft()] = t2;
	std::cout << testmap.size() << std::endl;
	for(auto a : testmap){
		std::cout << a.first << "->" << a.second<< std::endl;
	}






}