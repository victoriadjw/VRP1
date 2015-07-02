/*
*		usage: 1. contain data for identifying a Vehicle Routing Problem (VRP).
*			   2. provid solver interface.
*/

#ifndef VEHICLEROUTING_H
#define VEHICLEROUTING_H

#include<map>
#include<vector>
#include<list>
#include<string>
#include<sstream>
#include<iostream>
#include"Graph.h"
#include"Scenario.h"

#define PI  3.1415926
#define EARTH_RADIUS  6378.137// earth radius

class VehicleRouting:public Graph<VertexInfo,EdgeInfo>
{
public:
	typedef int IterCount;	// iteration count for meta-heuristic solver
	typedef int ObjValue;	// unit for objective value


	
	void createDG();
	void createUDG();
	void calculateDistance();
	void printGraph() const;
	void initSolution(Solution &);

	list<Order *> orderList;
	list<Vehicle *> vehicleList;
	Solution *solution;

private:
	double rad(const double &d)const;
	double getDistance(const double &, const double &, const double &, const double &)const;
};
#endif