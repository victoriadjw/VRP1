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
#include<float.h>

#include"Scenario.h"
using namespace boost;

class DijkstraShortPath
{
public:
	typedef property<vertex_name_t, ClientID> VertexProperty;
	typedef property<edge_weight_t, DistanceType> EdgeWeightProperty;
	typedef adjacency_list < listS, vecS, directedS, VertexProperty, EdgeWeightProperty> graph_t;
	typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
	DijkstraShortPath(const vector<Client> &, const vector<Edge> &);
	// get the shortest path for a given pair of client id
	void getShortPath(const ClientID &, const ClientID &, DistanceType &, vector<ClientID> &);
	// get the shortest path for a given starting client id and a given set of client id
	void getShortPathClientIDSet(const ClientID &, const set<ClientID> &, ClientID &, DistanceType &, vector<ClientID> &);
	void getShortPathExcept(const ClientID &, const ClientID &, const set<ClientID>&, DistanceType &, vector<ClientID> &);
	void getShortPathClientIDSetExcept(const ClientID &, const set<ClientID> &, ClientID &, DistanceType &, vector<ClientID> &);
private:
	void modifyGraph(const set<ClientID>&);
	void resetGraph();
	const vector<Client> &clientVec;
	const vector<Edge> &edgeVec;
	graph_t g;
	property_map<graph_t, vertex_name_t>::type cid_map;	// property accessor
	std::map<ClientID, vertex_descriptor> vertex_map;
};
#endif