// #include <iostream>
// #include <vector>
// #include "boost/polygon/polygon.hpp"
// #include "colours.h"
// #include "units.h"

// #include "cord.h"
// #include "line.h"
// #include "rectangle.h"
// #include "tile.h"
// #include "globalPhaseAdapter.h"


// int main(int argc, char const *argv[]) {


// }

#include <boost/polygon/polygon.hpp>
#include <assert.h>
namespace gtl = boost::polygon;

//This function works with both the 90 and 45 versions
//of connectivity extraction algroithm
template <typename ce_type>
void test_ce() {
  //first we create an object to do the connectivity extraction
  ce_type ce;

  //create some test data
  std::vector<gtl::rectangle_data<int> > test_data;
  test_data.push_back(gtl::rectangle_data<int>(10, 10, 90, 90));
  test_data.push_back(gtl::rectangle_data<int>(0, 0, 20, 20));
  test_data.push_back(gtl::rectangle_data<int>(80, 0, 100, 20));
  test_data.push_back(gtl::rectangle_data<int>(0, 80, 20, 100));
  test_data.push_back(gtl::rectangle_data<int>(80, 80, 100, 100));
  //There is one big square and four little squares covering each
  //of its corners.

  for(unsigned int i = 0; i < test_data.size(); ++i) {
    //insert returns an id starting at zero and incrementing
    //with each call
    assert(ce.insert(test_data[i]) == i);
  }
  //notice that ids returned by ce.insert happen to match
  //index into vector of inputs in this case

  //make sure the vector graph has elements for our nodes
  std::vector<std::set<int> > graph(test_data.size());

  //populate the graph with edge data
  ce.extract(graph);

  //make a map type graph to compare results
  std::map<int, std::set<int> > map_graph;
  ce.extract(map_graph);

  assert(map_graph.size() && map_graph.size() == graph.size());
  for(unsigned int i = 0; i < graph.size(); ++i) {
    assert(graph[i] == map_graph[i]);
    if(i == 0)
      assert(graph[i].size() == 4); //four little squares
    else
      assert(graph[i].size() == 1); //each little toches the big square
  }

    for(std::set<int> s : graph){
        for(int i : s){
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }

}

int main() {
  test_ce<gtl::connectivity_extraction_90<int> >();
  test_ce<gtl::connectivity_extraction_45<int> >();
  return 0;


}

//Now you know how to use the connectivity extraction algorithm
//to extract the connectivity graph for overlapping geometry
