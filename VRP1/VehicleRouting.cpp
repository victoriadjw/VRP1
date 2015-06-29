#include "VehicleRouting.h"

using namespace std;

void VehicleRouting::createDG()
{
	cout << "input the keyword of each vertex:" << endl;
	int id;
	while (cin >> id)
	{
		VertexInfo vi;
		vi.id = id;
		addVertex(vi);
	}
	cin.clear();
	cout << "input the starting and ending vertex of each arc:" << endl;
	int vHead, vTail, weight;
	while (cin >> vHead >> vTail >> weight)
	{
		ArcInfo ai;
		ai.weight = weight;
		insertArc(vHead, vTail, ai);
	}
}
void VehicleRouting::createUDG()
{
	cout << "input the keyword of each vertex:" << endl;
	int id;
	while (cin >> id)
	{
		VertexInfo vi;
		vi.id = id;
		addVertex(vi);
	}
	cin.clear();
	cout << "input the starting and ending vertex of each arc:" << endl;
	int vHead, vTail, weight;
	while (cin >> vHead >> vTail >> weight)
	{
		ArcInfo ai;
		ai.weight = weight;
		insertArc(vHead, vTail, ai);
		insertArc(vTail, vHead, ai);
	}
}
void VehicleRouting::printGraph()
{
	for (vector<Vertex>::iterator iter = vertices.begin(); iter != vertices.end(); iter++)
	{
		cout << "the adjacent vertex of node " << (*iter).vertexInfo.id << ":";
		for (list<ArcNode>::iterator iter_list = (*iter).firstArc.begin(); iter_list != (*iter).firstArc.end(); iter_list++)
		{
			cout << " -> " << (*iter_list).adjVertex
				<< "(" << (*iter_list).arcInfo.weight << ")";
		}
		cout << endl;
	}
	cout << "vertex num: " << vertexNum << " " << vertices.size() << endl
		<< "arc num: " << (arcNum>>1);
}
int main(int argc, char *argv[])
{
	cout << "vehicle routing problem." << endl;
	VehicleRouting vr;
	vr.createUDG();
	vr.printGraph();
	system("pause");
}