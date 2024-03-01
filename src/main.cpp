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
		gr.readGlobalResult("./inputs/case11-output.txt");

		Floorplan fp (gr, 0.5, 2, 0.8);
		Tile *t1 = fp.addBlockTile(Rectangle(0, 13, 2, 14), fp.allRectilinears[1]);
		Tile *t2 = fp.addOverlapTile(Rectangle(24, 2, 25, 7), {fp.allRectilinears[0], fp.allRectilinears[1]});
		fp.cs->visualiseTileDistribution("outputs/case11/case11-output.txt");
		std::cout << "Floorplan blockTilePayload: " << std::endl;
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
		std::cout << "All Rectilinear structure: " << std::endl;
		for(int i = 0; i < fp.allRectilinears.size(); ++i){
			Rectilinear *rect = fp.allRectilinears[i];

			std::cout << "[" << i << "]" << rect->getName() << " " << rect->getLegalArea() << std::endl;
			
			for(Tile *t : rect->blockTiles){
				std::cout << *t << std::endl;
			}
			for(Tile *t : rect->overlapTiles){
				std::cout << *t << std::endl;
			}
		}
		std::cout << "Estimiate Corner Stitching HPWL: " << fp.calculateHPWL() << std::endl;
		std::cout << GREEN << "Corner Stitching Exit normally" << COLORRST << std::endl;

		std::cout << "Start Testing: " << std::endl;
		fp.reshapeRectilinear(fp.allRectilinears[0]);
		fp.cs->visualiseTileDistribution("./outputs/case11/case11-output1.txt");
		std::cout << "Floorplan blockTilePayload: " << std::endl;
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
		std::cout << "All Rectilinear structure: " << std::endl;
		for(int i = 0; i < fp.allRectilinears.size(); ++i){
			Rectilinear *rect = fp.allRectilinears[i];

			std::cout << "[" << i << "]" << rect->getName() << " " << rect->getLegalArea() << std::endl;
			
			for(Tile *t : rect->blockTiles){
				std::cout << *t << std::endl;
			}
			for(Tile *t : rect->overlapTiles){
				std::cout << *t << std::endl;
			}
		}


		
	}catch(CSException c){
		std::cout << "Exception caught -> ";
		std::cout << c.what() << std::endl;
		// abort();
	}catch(...){
		std::cout << "Excpetion not caught but aborted!" << std::endl;
		abort();
	}

	
	
	
}