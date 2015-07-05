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

class VehicleRouting
{
public:
	typedef int IterCount;	// iteration count for meta-heuristic solver
	typedef int ObjValue;	// unit for objective value

	VehicleRouting(){ }
	Timer t;
	Graph<VertexInfo, EdgeInfo> g;
	vector<Order> orderVec;
	vector<Vehicle> vehicleVec;
	vector<Client> clientVec;
	Solution *solution;
	vector<Carrier> carrierVec;

	map<RegionID, int> regionMap;
	map<CarrierID, int>carrierMap;
	map<ClientID, int>clientMap;
	map<OrderID, int>orderMap;

	void setName(const string &);
	void setNumClient(const int &);
	void setNumOrder(const int &);
	void setNumVehicle(const int &);
	void setNumCarrier(const int &);
	void setNumBilling(const int &);
	void setNumRegion(const int &);
	void setPlanHorizon(const int &, const int &);

	int getNumClient()const;
	int getNumOrder()const;
	int getNumVehicle()const;
	int getNumCarrier()const;
	int getNumBilling()const;
	int getNumRegion()const;

private:
	string name;
	int numClient,numOrder,numVehicle,numCarrier,numBilling,numRegion;
	pair<int, int>planHorizon;
};
#endif