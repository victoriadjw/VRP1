/*
*	usage: 1. provide solver for vehicle routing problem.
*
*
*/

#ifndef SOLVER_H
#define SOLVER_H

#include"VehicleRouting.h"

class Solver
{
public:
	Solver(const VehicleRouting &_vr) :vr(_vr)
	{
		dsp = new DijkstraShortPath(vr.clientVec, vr.edgeVec);
	}
	~Solver(){ delete dsp; cout << "deconstruct Solver." << endl; }
	void modifyOrder();	// modify the order to accommodate the specific problem
	void assign();		// assign orders to vehicles
	void generateRoute(const int &);	// generate route according to route index
	// find the least cost in mandatory order
	OrderID findLeastCostOrder(const list<OrderID>&, const ClientID&);
private:
	const VehicleRouting &vr;
	DijkstraShortPath *dsp;
	Solution solution;
};
#endif