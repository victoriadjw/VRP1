#include"DijkstraShortPath.h"

DijkstraShortPath::DijkstraShortPath(const vector<Client> &cv, const vector<Edge> &ev) :clientVec(cv), edgeVec(ev)
{
	cid_map = get(vertex_name, g);
	for (vector<Client>::const_iterator iter = clientVec.begin();
		iter != clientVec.end(); iter++)
	{
		vertex_map[iter->PriDCID] = add_vertex(g);
		cid_map[vertex_map[iter->PriDCID]] = iter->PriDCID;
	}
	for (vector<Edge>::const_iterator iter = edgeVec.begin();
		iter != edgeVec.end(); iter++)
	{
		add_edge(vertex_map[iter->getEdge().first], vertex_map[iter->getEdge().second], iter->getDistance(), g);
	}
	//print_graph(g, cid_map);
	//print_graph(g, get(vertex_index,g));
}
void DijkstraShortPath::modifyGraph(const set<ClientID> &except_cid_set)
{
	vertex_map.clear();
	g.clear();
	cid_map = get(vertex_name, g);
	for (vector<Client>::const_iterator iter = clientVec.begin();
		iter != clientVec.end(); iter++)
	{
		if (except_cid_set.count(iter->PriDCID) == 1)
			continue;
		vertex_map[iter->PriDCID] = add_vertex(g);
		cid_map[vertex_map[iter->PriDCID]] = iter->PriDCID;
	}
	for (vector<Edge>::const_iterator iter = edgeVec.begin();
		iter != edgeVec.end(); iter++)
	{
		if (except_cid_set.count(iter->getEdge().first) == 1 ||
			except_cid_set.count(iter->getEdge().second) == 1)
			continue;
		add_edge(vertex_map[iter->getEdge().first], vertex_map[iter->getEdge().second], iter->getDistance(), g);
	}
	//print_graph(g, cid_map);
	//print_graph(g, get(vertex_index,g));
}
// get the shortest path for a given pair of client id
void DijkstraShortPath::getShortPath(const ClientID &start_cid, const ClientID &end_cid, 
	DistanceType &shortest_distance, vector<ClientID> &shortest_cid_vec)
{
	vector<vertex_descriptor> p(num_vertices(g));
	vector<DistanceType> d(num_vertices(g));
	dijkstra_shortest_paths(g, vertex_map[start_cid],
		predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g))).
		distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g))));
	/*std::cout << "distances and partents: " << std::endl;
	graph_traits<graph_t>::vertex_iterator vi, vend;
	for (boost::tie(vi, vend) = vertices(g); vi != vend; vi++)
	{
		std::cout << "distance(" << cid_map[*vi] << ")=" << d[*vi] << ",";
		std::cout << "parent(" << cid_map[*vi] << ")=" << cid_map[p[*vi]] << std::endl;
	}*/
	shortest_cid_vec.clear();
	//cout << start_cid<<" " << vertex_map[start_cid] << ", "<<end_cid<<" " << vertex_map[end_cid] << endl;
	vertex_descriptor vd;
	for (vd = vertex_map[end_cid]; vd != vertex_map[start_cid]; vd = p[vd])
	{
		//cout << cid_map[vd] << " ";
		shortest_cid_vec.push_back(cid_map[vd]);
	}
	//cout << cid_map[vd] << endl;
	shortest_cid_vec.push_back(cid_map[vd]);
	reverse(shortest_cid_vec.begin(), shortest_cid_vec.end());
	/*for (vector<ClientID>::iterator iter = shortest_cid_vec.begin();
		iter != shortest_cid_vec.end(); iter++)
		cout << *iter << ",";*/

}
// get the shortest path for a given starting client id and a given set of client id
void DijkstraShortPath::getShortPathClientIDSet(const ClientID &start_cid, const set<ClientID> &end_cid_set, ClientID &sel_end_cid, 
	DistanceType &shortest_distance, vector<ClientID> &shortest_cid_vec)
{
	vector<vertex_descriptor> p(num_vertices(g));
	vector<DistanceType> d(num_vertices(g));
	dijkstra_shortest_paths(g, vertex_map[start_cid],
		predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g))).
		distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g))));
	//std::cout << "distances and partents for one client id and a vector of client id: " << std::endl;
	shortest_distance = DBL_MAX;
	graph_traits<graph_t>::vertex_iterator vi, vend;
	for (boost::tie(vi, vend) = vertices(g); vi != vend; vi++)
	{
		//std::cout << "distance(" << cid_map[*vi] << ")=" << d[*vi] << ",";
		//std::cout << "parent(" << cid_map[*vi] << ")=" << cid_map[p[*vi]] << std::endl;
		if (end_cid_set.count(cid_map[*vi])==1&&
			d[*vi] < shortest_distance)
		{
			shortest_distance = d[*vi];
			sel_end_cid = cid_map[*vi];
		}
	}
	shortest_cid_vec.clear();
	//cout << start_cid << " " << vertex_map[start_cid] << ", " << sel_end_cid << " " << vertex_map[sel_end_cid] << endl;
	vertex_descriptor vd;
	for (vd = vertex_map[sel_end_cid]; vd != vertex_map[start_cid]; vd = p[vd])
	{
		//cout << cid_map[vd] << " ";
		shortest_cid_vec.push_back(cid_map[vd]);
	}
	//cout << cid_map[vd] << endl;
	shortest_cid_vec.push_back(cid_map[vd]);
	reverse(shortest_cid_vec.begin(), shortest_cid_vec.end());
	/*for (vector<ClientID>::iterator iter = shortest_cid_vec.begin();
		iter != shortest_cid_vec.end(); iter++)
		cout << *iter << ",";*/
}
#if 0
int main(int, char *[])
{
	typedef adjacency_list < listS, vecS, directedS,
		no_property, property < edge_weight_t, int > > graph_t;
	typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
	typedef std::pair<int, int> Edge;

	const int num_nodes = 5;
	enum nodes { A, B, C, D, E };
	char name[] = "ABCDE";
	Edge edge_array[] = { Edge(A, C), Edge(B, B), Edge(B, D), Edge(B, E),
		Edge(C, B), Edge(C, D), Edge(D, E), Edge(E, A), Edge(E, B)
	};
	int weights[] = { 1, 2, 1, 2, 7, 3, 1, 1, 1 };
	int num_arcs = sizeof(edge_array) / sizeof(Edge);
	graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);
	property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);
	std::vector<vertex_descriptor> p(num_vertices(g));
	std::vector<int> d(num_vertices(g));
	vertex_descriptor s = vertex(A, g);

	dijkstra_shortest_paths(g, s,
		predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g))).
		distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g))));

	std::cout << "distances and parents:" << std::endl;
	graph_traits < graph_t >::vertex_iterator vi, vend;
	for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
		std::cout << "distance(" << name[*vi] << ") = " << d[*vi] << ", ";
		std::cout << "parent(" << name[*vi] << ") = " << name[p[*vi]] << std::
			endl;
	}
	std::cout << std::endl;

	std::ofstream dot_file("results/dijkstra-eg.dot");

	dot_file << "digraph D {\n"
		<< "  rankdir=LR\n"
		<< "  size=\"4,3\"\n"
		<< "  ratio=\"fill\"\n"
		<< "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";

	graph_traits < graph_t >::edge_iterator ei, ei_end;
	for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
		graph_traits < graph_t >::edge_descriptor e = *ei;
		graph_traits < graph_t >::vertex_descriptor
			u = source(e, g), v = target(e, g);
		dot_file << name[u] << " -> " << name[v]
			<< "[label=\"" << get(weightmap, e) << "\"";
		if (p[v] == u)
			dot_file << ", color=\"black\"";
		else
			dot_file << ", color=\"grey\"";
		dot_file << "]";
	}
	dot_file << "}";
	system("pause");
	return EXIT_SUCCESS;
}
#endif