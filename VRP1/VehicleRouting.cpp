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
	cout << "input the starting and ending vertex of each edge:" << endl;
	int vHead, vTail, weight;
	while (cin >> vHead >> vTail >> weight)
	{
		EdgeInfo *ai=new EdgeInfo;
		ai->distance = weight;
		insertEdge(vHead, vTail, ai);
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
	cout << "input the starting and ending vertex of each edge:" << endl;
	int vHead, vTail, weight;
	while (cin >> vHead >> vTail >> weight)
	{
		EdgeInfo *ai=new EdgeInfo;
		ai->distance = weight;
		insertEdge(vHead, vTail, ai);
		insertEdge(vTail, vHead, ai);
	}
}
void VehicleRouting::printGraph() const
{
	for (vector<Vertex *>::const_iterator iter = vertices.begin(); iter != vertices.end(); iter++)
	{
		cout << "the adjacent vertex of node " << (*iter)->vertexInfo->id << ":";
		for (list<EdgeNode *>::const_iterator iter_list = (*iter)->firstEdge.begin(); 
			iter_list != (*iter)->firstEdge.end(); iter_list++)
		{
			cout << " -> " << (*iter_list)->adjVertex
				<< "(" << (*iter_list)->edgeInfo->distance << ")";
		}
		cout << endl;
	}
	cout << "vertex num: " << vertexNum << " " << vertices.size() << endl
		<< "edge num: " << (edgeNum>>1);
}

double VehicleRouting::rad(const double &d)const
{
	return (d * PI / 180.0);
}

double VehicleRouting::getDistance(const double &lat1, const double &lng1, const double &lat2, const double &lng2)const
{
	double radLat1 = rad(lat1);
	double radLat2 = rad(lat2);
	double a = radLat1 - radLat2;
	double b = rad(lng1) - rad(lng2);
	double s = 2 * asin(sqrt(pow(sin(a / 2), 2) +
		cos(radLat1)*cos(radLat2)*pow(sin(b / 2), 2)));
	return (round(s * EARTH_RADIUS * 10000) / 10000);
}

void VehicleRouting::calculateDistance()
{
	for (vector<Vertex *>::const_iterator iter = vertices.begin(); iter != vertices.end(); iter++)
	{
		for (list<EdgeNode *>::const_iterator iter_list = (*iter)->firstEdge.begin(); 
			iter_list != (*iter)->firstEdge.end(); iter_list++)
		{
			(*iter_list)->edgeInfo->distance = getDistance((*iter)->vertexInfo->latitude, (*iter)->vertexInfo->longitude,
				(*iter)->vertexInfo->latitude, (*iter)->vertexInfo->longitude);
		}
	}
}