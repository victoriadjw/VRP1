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
	Solver(const VehicleRouting &_vr);
	~Solver(){ delete dsp; cout << "deconstruct Solver." << endl; }
	void modifyOrder();	// modify the order to accommodate the specific problem
	void assign();		// assign orders to vehicles
	void generateRoute(const int &);	// generate route according to route index
	void generateRoute1(const int &);	// generate route for mandatory orders according to route index
	void insertMMOrderToRoute(const int&, list<OrderID>::iterator&, list<ServeClient>::iterator &, list<ServeClient>::iterator &);
	void insertMMOrderToRoute1(const int&, list<OrderID>::iterator&, list<ServeClient>::iterator &, list<ServeClient>::iterator &);
	void insertMOOrderToRoute(const int&, list<OrderID>::iterator&, list<ServeClient>::iterator &);
	void insertOMOrderToRoute(const int&, list<OrderID>::iterator&, list<ServeClient>::iterator &);
	void insertOOOrderToRoute(const int&, list<OrderID>::iterator&);
	bool checkRoute(const int&)const;
	bool checkClientInRoute(const int&,const vector<ClientID>&)const;
	// find the least cost in mandatory order
	OrderID findLeastCostOrder(const list<OrderID>&, const ClientID&);
	// find the iterator of a client id in serve client id list
	void findClientIDServeList( list<ServeClient> &, const ClientID &, list<ServeClient>::iterator &);
	void printRoute(const int&)const;
	void calculateObjValue(const int&);
private:
	const VehicleRouting &vr;
	DijkstraShortPath *dsp;
	Solution solution;
	list<OrderID> mandatoryOrderList, optionalOrderList;
};
#endif