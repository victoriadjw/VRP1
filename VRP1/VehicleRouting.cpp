#include "VehicleRouting.h"
int main(int argc, char *argv[])
{
	cout << "vehicle routing problem." << endl;
	VehicleRouting *vr = new VehicleRouting();
	VehicleRouting::Graph *g = new VehicleRouting::Graph(VehicleRouting::UDG);
	g->createGraph();
	g->printGraph();
	system("pause");
}