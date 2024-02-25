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
		// std::vector <Tile *> tiles;
		// CornerStitching cs(100, 100);
		// tiles.push_back(cs.insertTile(Tile(tileType::BLOCK, Rectangle(3, 18, 6, 22))));
		// tiles.push_back(cs.insertTile(Tile(tileType::BLOCK, Rectangle(6, 20, 11, 23))));
		// tiles.push_back(cs.insertTile(Tile(tileType::BLOCK, Rectangle(4, 23, 8, 25))));
		// tiles.push_back(cs.insertTile(Tile(tileType::BLOCK, Rectangle(8, 17, 10, 20))));

		// Rectilinear r1(5, "CPU", rectilinearType::SOFT, Rectangle(13, 15, 22, 35), 20, 0.333, 3, 0.6);
		// for(Tile *t : tiles){
		// 	r1.blockTiles.insert(t);
		// }
		// Tile *bugt = new Tile(tileType::OVERLAP, Rectangle(2, 22, 4, 24));
		// r1.overlapTiles.insert(bugt);

		// std::cout << "Legal no overlap: " << r1.isLegalNoOverlap() << std::endl;
		// std::cout << "Legal no hole: " << r1.isLegalNoHole() << std::endl;
		// std::cout << "Legal one shape: " << r1.isLegalOneShape() << std::endl;

		// rectilinearIllegalType illegalType;	
		// std::cout << r1.isLegal(illegalType) << ", Error Type: " << illegalType << std::endl;

		// std::vector<Cord> winding;
		// r1.acquireWinding(winding, windingDirection::CLOCKWISE);
		// std::cout << "Presenting clockwise winding: " << std::endl;
		// for(Cord c : winding){
		// 	std::cout << c << std::endl;
		// }

		// cs.visualiseTileDistribution("./outputs/case09/case09-output-0.txt");

		// GlobalResult gr;
		// gr.readGlobalResult("./inputs/case09-output.txt");

		// Floorplan fp (gr);
		// fp.cs->visualiseTileDistribution("./outputs/case09/case09-output-0.txt");
		using namespace boost::polygon::operators;

		Rectangle blk1 = Rectangle(9, 19, 12, 22);
		Rectangle blk2 = Rectangle(15, 10, 19, 14);
		Rectangle blk3 = Rectangle(7, 15, 10, 18);
		Rectangle blk4 = Rectangle(10, 12, 12, 15);
		Rectangle ov5 = Rectangle(7, 12, 10, 15);

		Rectangle newR = Rectangle(9, 13, 18, 23);

		DoughnutPolygonSet dblk1, dblk2, dblk3, dblk4, dov5, dnewR;
		DoughnutPolygonSet i1, i2, i3, i4, i5;

		dblk1 += blk1;
		dblk2 += blk2;
		dblk3 += blk3;
		dblk4 += blk4;
		dov5 += ov5;

		dnewR += newR;

		boost::polygon::assign(i1, dblk1 & dnewR);
		boost::polygon::assign(i2, dblk2 & dnewR);
		boost::polygon::assign(i3, dblk3 & dnewR);
		boost::polygon::assign(i4, dblk4 & dnewR);
		boost::polygon::assign(i5, dov5 & dnewR);


		std::cout << "Ins1: " << i1.size() << std::endl;
		for(DoughnutPolygon dp : i1){
			std::cout << dp << std::endl;
		}

		std::cout << "Ins2: " << i2.size() << std::endl;
		for(DoughnutPolygon dp : i2){
			std::cout << dp << std::endl;
		}

		std::cout << "Ins3: " << i3.size() << std::endl;
		for(DoughnutPolygon dp : i3){
			std::cout << dp << std::endl;
		}

		std::cout << "Ins4: " << i4.size() << std::endl;
		for(DoughnutPolygon dp : i4){
			std::cout << dp << std::endl;
		}

		std::cout << "Ins5: " << i5.size() << std::endl;
		for(DoughnutPolygon dp : i5){
			std::cout << dp << std::endl;
		}

		std::cout << boost::polygon::equivalence(dblk1, i1) << std::endl;
		std::cout << boost::polygon::equivalence(dblk2, i2) << std::endl;
		std::cout << boost::polygon::equivalence(dblk3, i3) << std::endl;
		std::cout << boost::polygon::equivalence(dblk4, i4) << std::endl;
		std::cout << boost::polygon::equivalence(dov5, i5) << std::endl;

		dblk2 -= i2;
		std::cout << "dblk after remove intersect" << dblk2.size() << std::endl;
		for(DoughnutPolygon dp : dblk2){
			std::cout << dp << std::endl;
		}

		std::vector<Rectangle> fragments;

		boost::polygon::get_rectangles(fragments, dblk2);
		std::cout << "Fragments in " << fragments.size() << " pieces" << std::endl;
		for(Rectangle r : fragments){
			std::cout << r << " ";
		}
		std::cout << std::endl;






		
	}catch(CSException c){
		std::cout << "Exception caught -> ";
		std::cout << c.what() << std::endl;
		// abort();
	}catch(...){
		std::cout << "Excpetion not caught but aborted!" << std::endl;
		abort();
	}

	
	
	
}