#ifndef GRAPH_H
#define GRAPH_H

using namespace std;
template<class VertexInfo,class EdgeInfo>
class Graph
{
public:

	void addVertex(VertexInfo *vertexInfo)
	{
		Vertex *v=new Vertex;
		v->vertexInfo = vertexInfo;
		vertices.push_back(v);
		vertexNum += 1;
	}
	void insertEdge(int vHead, int vTail, EdgeInfo *edgeInfo)
	{
		EdgeNode *an=new EdgeNode;
		an->adjVertex = vTail;
		an->edgeInfo = edgeInfo;
		vertices[vHead]->firstEdge.push_back(an);
		edgeNum += 1;
	}

	class EdgeNode
	{
	public:
		int adjVertex;		// adjecent vertex of edge
		EdgeInfo *edgeInfo;	// information of edge
	};
	class Vertex
	{
	public:
		VertexInfo *vertexInfo;	// information of vertex
		list<EdgeNode *> firstEdge;	// first edge adjacent to the vertex
	};
	vector<Vertex *> vertices;
	int vertexNum;	// number of vertex
	int edgeNum;		// number of edge

};

#endif