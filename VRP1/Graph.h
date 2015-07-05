#ifndef GRAPH_H
#define GRAPH_H

using namespace std;
template<class VertexInfo,class EdgeInfo>
class Graph
{
public:
	Graph(){ vertexNum = edgeNum = 0; /*cout << "construct graph" << endl;*/ }
	

	int vertexNum;	// number of vertex
	int edgeNum;		// number of edge

};

#endif