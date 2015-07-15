#ifndef DIJKSTRASHORTPATH_H
#define DIJKSTRASHORTPATH_H
#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_utility.hpp>

#include"Scenario.h"
using namespace boost;

class DijkstraShortPath
{
public:
	typedef property<vertex_name_t, ClientID> VertexProperty;
	typedef property<edge_weight_t, DistanceType> EdgeWeightProperty;
	typedef adjacency_list < listS, vecS, directedS, VertexProperty, EdgeWeightProperty> graph_t;
	typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
	
	DijkstraShortPath(const vector<Client> &,const vector<Edge> &);
private:
	const vector<Client> &clientVec;
	const vector<Edge> &edgeVec;
	graph_t g;
	std::map<ClientID, vertex_descriptor> vertex_map;
};
#endif