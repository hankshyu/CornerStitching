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


int main(int argc, char const *argv[]) {
	globalPhaseAdapter gpa("./inputs/case02-output.txt");
	gpa.readGlobalResult();
	gpa.detectGlobalOverlaps();
	gpa.printStatus();

}