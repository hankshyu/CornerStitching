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

    std::cout << 1 << " " << argv[0] << std::endl;
    std::cout << 2 << " " << argv[1] << std::endl;
    globalPhaseAdapter gpa(argv[1]);
    gpa.readGlobalResult();
}
