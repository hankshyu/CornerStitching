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

		GlobalResult gr;
		gr.readGlobalResult("./inputs/case02-output.txt");

		Floorplan fp (gr);
		fp.cs->visualiseTileDistribution("outputs/case02/case02-output.txt");
		for(auto p : fp.blockTilePayload){
			std::cout << *(p.first) << "->" << p.second->getName() << std::endl;
		}

		std::cout << "Overlap stuff: " << std::endl;
		for(auto p : fp.overlapTilePayload){
			std::cout << *(p.first) << "-> {";
			for(auto const &pp : p.second){
				std::cout << pp->getName() << ", ";
			}
			std::cout << "}" << std::endl;
		}

		std::cout << "Estimiate Corner Stitching HPWL: " << fp.calculateHPWL() << std::endl;
		std::cout << GREEN << "Corner Stitching Exit normally" << COLORRST << std::endl;




		
	}catch(CSException c){
		std::cout << "Exception caught -> ";
		std::cout << c.what() << std::endl;
		// abort();
	}catch(...){
		std::cout << "Excpetion not caught but aborted!" << std::endl;
		abort();
	}

	
	
	
}