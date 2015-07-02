#ifndef GRAPH_H
#define GRAPH_H

using namespace std;
template<class VertexInfo,class ArcInfo>
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
	void insertArc(int vHead, int vTail, ArcInfo *arcInfo)
	{
		ArcNode *an=new ArcNode;
		an->adjVertex = vTail;
		an->arcInfo = arcInfo;
		vertices[vHead]->firstArc.push_back(an);
		arcNum += 1;
	}

	class ArcNode
	{
	public:
		int adjVertex;		// adjecent vertex of arc
		ArcInfo *arcInfo;	// information of arc
	};
	class Vertex
	{
	public:
		VertexInfo *vertexInfo;	// information of vertex
		list<ArcNode *> firstArc;	// first arc adjacent to the vertex
	};
	vector<Vertex *> vertices;
	int vertexNum;	// number of vertex
	int arcNum;		// number of arc

};

#endif