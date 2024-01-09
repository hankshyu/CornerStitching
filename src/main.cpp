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
	// globalPhaseAdapter gpa("./inputs/case02-output.txt");
	// gpa.readGlobalResult();
	// gpa.detectGlobalOverlaps();
	// // gpa.printStatus();
	// std::vector <Tile *> insertArr = gpa.printFlowStatus(true);
	// std::cout << "This is inset Arr: " << std::endl;

	// for(Tile *t : insertArr){
	// 	std::cout << *t << std::endl;
	// }

	// Tile t1 (tileType::BLANK, Cord(2, 6), Cord(7, 11));
	// Rectangle rt = t1.getRectangle(); 
	// std::cout << rt << std::endl;
	// std::cout << t1.getLowerLeft().x() + t1.getWidth() << std::endl;
	// std::cout << boost::polygon::xh(rt) << std::endl;

	CornerStitching cs(6, 8);
	
	std::cout << cs.checkRectangleInCanvas(Rectangle(0, 0, 4, 3)) << std::endl;
	std::cout << cs.checkRectangleInCanvas(Rectangle(0, 0, 1, 7)) << std::endl;
	std::cout << cs.checkRectangleInCanvas(Rectangle(0, 0, 1, 8)) << std::endl;
	std::cout << cs.checkRectangleInCanvas(Rectangle(0, 0, 1, 9)) << std::endl;
	std::cout << cs.checkRectangleInCanvas(Rectangle(5, 3, 6, 8)) << std::endl;
	std::cout << cs.checkRectangleInCanvas(Rectangle(5, 3, 7, 8)) << std::endl;

	Rectangle r1(7, 8, 9, 11);
	Rectangle r2(10, 12, 12, 16);

	std::cout << "aotehr" << std::endl;
	std::cout << rec::hasIntersection(r1, r2, false) << std::endl;
	std::cout << rec::hasIntersection(r1, r2, true) << std::endl;
	
	Rectangle r3(7, 8, 9, 11);
	Rectangle r4(9, 11, 12, 16);

	std::cout << "aotehr" << std::endl;
	std::cout << rec::hasIntersection(r3, r4, false) << std::endl;
	std::cout << rec::hasIntersection(r3, r4, true) << std::endl;
}