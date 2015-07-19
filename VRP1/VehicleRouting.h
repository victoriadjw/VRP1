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
#include"DijkstraShortPath.h"

class VehicleRouting
{
public:
	typedef int IterCount;	// iteration count for meta-heuristic solver
	typedef int ObjValue;	// unit for objective value

	VehicleRouting(){ }

	Timer t;

	vector<Client> clientVec;
	vector<Edge> edgeVec;
	vector<Order> orderVec;
	vector<Vehicle> vehicleVec;
	vector<Carrier> carrierVec;

	mutable map<ClientID, int> clientMap;
	map<RegionID, int> regionMap;
	map<CarrierID, int>carrierMap;
	mutable map<OrderID, int>orderMap;
	mutable map<VehicleID, int>vehicleMap;
	vector<vector<int>> orderEdge; 


	void modifyOrder();
	void setMandOptionOrder();
	DistanceType getDistanceByOrderIndex(const OrderID &)const;

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
	vector<int> getMandatoryOrderIndexVec()const;
	vector<int> getOptionalOrderIndexVec()const;
private:
	void calculateDistance();	// calculate distance according to latitude and longitude
	class CmpDistance;
	string name;
	int numClient,numOrder,numVehicle,numCarrier,numBilling,numRegion;
	pair<int, int>CyclePlan ;
	vector<int> mandatoryOrderIndexVec, optionalOrderIndexVec;
};
#endif