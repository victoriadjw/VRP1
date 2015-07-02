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

class VehicleRouting:public Graph<VertexInfo,ArcInfo>
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
};
#endif