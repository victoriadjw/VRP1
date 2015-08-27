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
	Solver(const VehicleRouting &_vr, ostream &_os);
	~Solver(){ delete dsp; cout << "deconstruct Solver." << endl; }
	const vector<Solution>& getSolutionVec()const { return solutionVec; }
	void modifyOrder();	// modify the order to accommodate the specific problem
	void initSolution(const int&);		// assign orders to vehicles
	void generateRoute(Solution &, const int &);	// generate route according to route index
	void generateRoute1(Solution &,const int &);	// generate route for mandatory orders according to route index
	void insertMMOrderToRoute(Solution &solution, const int&, list<OrderID>::iterator&, list<ServeClient>::iterator &, list<ServeClient>::iterator &);
	void insertMMOrderToRoute1(Solution &solution, const int&, list<OrderID>::iterator&, list<ServeClient>::iterator &, list<ServeClient>::iterator &);
	void insertMOOrderToRoute(Solution &solution, const int&, list<OrderID>::iterator&, list<ServeClient>::iterator &);
	void insertOMOrderToRoute(Solution &solution, const int&, list<OrderID>::iterator&, list<ServeClient>::iterator &);
	void insertOOOrderToRoute(Solution &solution, const int&, list<OrderID>::iterator&);
	bool checkRoute(const Solution &solution, const int&)const;
	bool checkClientInRoute(const Solution &solution, const int&, const vector<ClientID>&)const;
	// find the least cost in mandatory order
	OrderID findLeastCostOrder(const list<OrderID>&, const ClientID&);
	// find the iterator of a client id in serve client id list
	void findClientIDServeList( list<ServeClient> &, const ClientID &, list<ServeClient>::iterator &);
	void printRoute(const Solution &solution, const int&)const;
	void calculateObjValue(Solution &solution, const int&);
	void calculateTotalObjValue(Solution &solution);
	void cancelOrder(Solution &solution, const int &rin, const OrderID &oid);
	bool isFeasibleAddServeTime(Solution &solution, const int &rin, const list<ServeClient>::iterator &start_sc_iter,
		const list<ServeClient>::iterator &end_sc_iter, const set<OrderID>::iterator &oid_iter);
	void arrangeMandatoryOrder(Solution &, const int &);
	void arrangeOptionalOrder(Solution &, const int &);
	bool insertMandatoryOrder(Solution &solution, const int &, const OrderID &);
	bool removeMandatoryOrder(Solution &solution, const int &, const ClientID &);
	void localSearch(const int&, const int &iteration);
	void makeMove(Solution &,int &,int &);
private:
	const VehicleRouting &vr;
	ostream &os;
	DijkstraShortPath *dsp;
	vector<Solution> solutionVec;
};
#endif