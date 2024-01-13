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
	
	Rectangle r1(6, 3, 11, 7);

	Rectangle r2(7, 4, 8, 6);
	Rectangle r3(10, 2, 11, 4);
	Rectangle r4(11, 4, 12, 6);
	Rectangle r5(4, 7, 6, 11);
	Rectangle r6(9, 5, 11, 7);
	Rectangle r7(6, 3, 7, 4);
	Rectangle r8(7, 8, 8, 9);
	
	// for(int i = 5; i<= 13; ++i){
	// 	for(int j = 2; j <= 9; ++j){
	// 		std::cout << "(" << i << ", " << j << ")";
	// 		std::cout << (rec::isContained(r1,Cord(i, j))? "o" : "x") << "\t";
	// 	}
	// 	std::cout << std::endl;
	// }

	bool consTouch  = false;
	std::cout << rec::hasIntersect(r1, r2, consTouch) << std::endl;
	std::cout << rec::hasIntersect(r1, r3, consTouch) << std::endl;
	std::cout << rec::hasIntersect(r1, r4, consTouch) << std::endl;
	std::cout << rec::hasIntersect(r1, r5, consTouch) << std::endl;
	std::cout << rec::hasIntersect(r1, r6, consTouch) << std::endl;
	std::cout << rec::hasIntersect(r1, r7, consTouch) << std::endl;
	std::cout << rec::hasIntersect(r1, r8, consTouch) << std::endl;



}