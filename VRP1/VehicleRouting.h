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

#include"Scenario.h"

class VehicleRouting
{
public:
	typedef int IterCount;	// iteration count for meta-heuristic solver
	typedef int ObjValue;	// unit for objective value

	VehicleRouting(){ }
	void modifyOrder();	// modify the order to accommodate the specific problem
	void assign();		// assign orders to vehicles
	void generateRoute(const int &);	// generate route according to route index
	// find the least cost in mandatory order
	OrderID findLeastCostOrder(const set<OrderID>&, const list<OrderID>&, const ClientID&);
	Timer t;
	// map structure
	vector<Client> clientVec;
	vector<Edge> edgeVec;
	map<ClientID, int> clientMap;

	vector<Order> orderVec;
	vector<Vehicle> vehicleVec;
	Solution solution;
	vector<Carrier> carrierVec;

	map<RegionID, int> regionMap;
	map<CarrierID, int>carrierMap;
	map<OrderID, int>orderMap;
	map<VehicleID, int>vehicleMap;
	vector<vector<int>> orderEdge;

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
	void calculateDistance();	// calculate distance according to latitude and longitude
	class CmpDistance;
	DistanceType getDistanceByOrderIndex(const int &);
	string name;
	int numClient,numOrder,numVehicle,numCarrier,numBilling,numRegion;
	pair<int, int>CyclePlan ;
	vector<int> mandatoryOrderIndexVec, optionalOrderIndexVec;
};
#endif