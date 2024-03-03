#include <iostream>
#include <random>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "boost/polygon/polygon.hpp"
#include "colours.h"
#include "units.h"
#include "cord.h"
#include "line.h"
#include "rectangle.h"
#include "tile.h"
#include "cornerStitching.h"
#include "cSException.h"
#include "rectilinear.h"
#include "doughnutPolygon.h"
#include "doughnutPolygonSet.h"
#include "globalResult.h"
#include "floorplan.h"


void genEncoder(int bits, unsigned int num, std::vector<int> &answer){
	unsigned int filter = 1;
	for(int i = 0; i < bits ; ++i){
		if(num & filter) answer.push_back(i);
		filter = filter << 1;
	}
}
int main(int argc, char const *argv[]) {

	try{

		std::string caseStr = "case10";
		GlobalResult gr;
		gr.readGlobalResult("./inputs/" + caseStr + "-output.txt");
		Floorplan fp(gr, 0.333, 3, 0.8);
		fp.cs->visualiseCornerStitching("./outputs/" + caseStr + "/" + caseStr + "-cs.txt");
		// fp.visualiseFloorplan("./outputs/" + caseStr + "/" + caseStr + "-fp.txt");
		std::cout << GREEN << "Floorplan job Done!!" << COLORRST << std::endl;
		
	}catch(CSException c){
		std::cout << "Exception caught -> ";
		std::cout << c.what() << std::endl;
		// abort();
	}catch(...){
		std::cout << "Excpetion not caught but aborted!" << std::endl;
		abort();
	}

	
	
	
}