#include "VehicleRouting.h"
//#include<math.h>

using namespace std;

void VehicleRouting::createDG()
{
	cout << "input the keyword of each vertex:" << endl;
	int id;
	while (cin >> id)
	{
		VertexInfo *vi=new VertexInfo;
		vi->id = id;
		addVertex(vi);
	}
	cin.clear();
	cout << "input the starting and ending vertex of each arc:" << endl;
	int vHead, vTail, weight;
	while (cin >> vHead >> vTail >> weight)
	{
		ArcInfo *ai=new ArcInfo;
		ai->distance = weight;
		insertArc(vHead, vTail, ai);
	}
}
void VehicleRouting::createUDG()
{
	cout << "input the keyword of each vertex:" << endl;
	int id;
	while (cin >> id)
	{
		VertexInfo *vi=new VertexInfo;
		vi->id = id;
		addVertex(vi);
	}
	cin.clear();
	cout << "input the starting and ending vertex of each arc:" << endl;
	int vHead, vTail, weight;
	while (cin >> vHead >> vTail >> weight)
	{
		ArcInfo *ai=new ArcInfo;
		ai->distance = weight;
		insertArc(vHead, vTail, ai);
		insertArc(vTail, vHead, ai);
	}
}
void VehicleRouting::printGraph() const
{
	for (vector<Vertex *>::const_iterator iter = vertices.begin(); iter != vertices.end(); iter++)
	{
		cout << "the adjacent vertex of node " << (*iter)->vertexInfo->id << ":";
		for (list<ArcNode *>::const_iterator iter_list = (*iter)->firstArc.begin(); 
			iter_list != (*iter)->firstArc.end(); iter_list++)
		{
			cout << " -> " << (*iter_list)->adjVertex
				<< "(" << (*iter_list)->arcInfo->distance << ")";
		}
		cout << endl;
	}
	cout << "vertex num: " << vertexNum << " " << vertices.size() << endl
		<< "arc num: " << (arcNum>>1);
}
void VehicleRouting::calculateDistance()
{
	for (vector<Vertex *>::const_iterator iter = vertices.begin(); iter != vertices.end(); iter++)
	{
		for (list<ArcNode *>::const_iterator iter_list = (*iter)->firstArc.begin(); 
			iter_list != (*iter)->firstArc.end(); iter_list++)
		{
			(*iter_list)->arcInfo->distance = sqrt(((*iter)->vertexInfo->latitude - vertices[(*iter_list)->adjVertex]->vertexInfo->latitude)
				*((*iter)->vertexInfo->latitude - vertices[(*iter_list)->adjVertex]->vertexInfo->latitude) +
				((*iter)->vertexInfo->longitude - vertices[(*iter_list)->adjVertex]->vertexInfo->longitude)
				*((*iter)->vertexInfo->longitude - vertices[(*iter_list)->adjVertex]->vertexInfo->longitude));
		}
	}
}