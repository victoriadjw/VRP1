/*
*	usage: 1. provide solver for vehicle routing problem.
*
*
*/

#ifndef SOLVER_H
#define SOLVER_H

#include"VehicleRouting.h"

// comment to the insertion of OM order to route
#ifndef SOLVER_OM_ORDER_INSERT
#define SOLVER_OM_ORDER_INSERT
#endif

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
	void insertMMOrderToRoute(const int&, list<OrderID>::iterator&, list<ServeClient>::iterator &, list<ServeClient>::iterator &);
	void insertMOOrderToRoute(const int&, list<OrderID>::iterator&, list<ServeClient>::iterator &);
	void insertOMOrderToRoute(const int&, list<OrderID>::iterator&, list<ServeClient>::iterator &);
	void insertOOOrderToRoute(const int&, list<OrderID>::iterator&);
	// find the least cost in mandatory order
	OrderID findLeastCostOrder(const list<OrderID>&, const ClientID&);
	// find the iterator of a client id in serve client id list
	void findClientIDServeList( list<ServeClient> &, const ClientID &, list<ServeClient>::iterator &);
	void printRoute(const int&)const;
private:
	const VehicleRouting &vr;
	DijkstraShortPath *dsp;
	Solution solution;
};
#endif