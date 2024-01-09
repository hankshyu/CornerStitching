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

	Rectangle mr (4, 5, 7, 9);
	std::cout << rec::getWidth(mr) << std::endl;
	std::cout << rec::getHeight(mr) << std::endl;
	std::cout << rec::getXL(mr) << std::endl;
	std::cout << rec::getXH(mr) << std::endl;
	std::cout << rec::getYL(mr) << std::endl;
	std::cout << rec::getYH(mr) << std::endl;
	std::cout << rec::getLL(mr) << std::endl;
	std::cout << rec::getLR(mr) << std::endl;
	std::cout << rec::getUL(mr) << std::endl;
	std::cout << rec::getUR(mr) << std::endl;

	Tile tt (tileType::BLANK, mr);
	std::cout << tt.getWidth() << std::endl;
	std::cout << tt.getHeight() << std::endl;
	std::cout << tt.getXLow() << std::endl;
	std::cout << tt.getXHigh() << std::endl;
	std::cout << tt.getYLow() << std::endl;
	std::cout << tt.getYHigh() << std::endl;

	std::cout << tt.getLowerLeft() << std::endl;
	std::cout << tt.getLowerRight() << std::endl;
	std::cout << tt.getUpperLeft() << std::endl;
	std::cout << tt.getUpperRight() << std::endl;

}