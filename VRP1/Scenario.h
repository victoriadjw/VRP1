/*
*		usage: 1. contain data for identifying a specific VRP.
*			   2. provide solver interface.
*/

#ifndef SCENARIO_H
#define SCENARIO_H

#include<vector>
#include<list>
#include<set>

#include"Utility.h"

typedef double GeographyType;
typedef int CapacityType;
typedef int UnitVolumeType;
typedef int UnitWeightType;
typedef double LengthType;
typedef int GoodsID;
typedef string ClientID;
typedef string VehicleID;
typedef string OrderID;
typedef string ClientID;
typedef string RegionID;
typedef string CarrierID;
typedef int QuantityType;
typedef double DistanceType;
typedef int TimeDistanceType;
typedef double ObjectType;
typedef int CostType;
typedef Timer TimeType;
typedef string ClientNameType;

enum OrderType{ Optional, Mandatory };

class GoodsInfo
{
public:
	GoodsID GoodID;	// identity
	string GoodName;		// name of the goods
	QuantityType quantity;	// quantity of the goods
	CapacityType outputPerMonth;	// output per month
	UnitVolumeType unitVolume;		// unit volume
	UnitWeightType unitWeight;		// unit weight
};

class Edge
{
	friend std::ostream& operator<<(std::ostream&, const Edge&);
	friend std::istream& operator>>(std::istream&, Edge&);
public:
	pair<ClientID, ClientID> getEdge()const{ return edge; }
	void setDistance(const DistanceType &d){ distance = d; }
	DistanceType getDistance()const{ return distance; }
private:
	pair<ClientID, ClientID> edge;
	DistanceType distance;
	TimeDistanceType timeDistance;
};
class Client
{
	friend std::istream& operator>>(std::istream&, Client&);
public:	
	ClientID PriDCID;	// identity
	ClientNameType clientName;	// client name
	GeographyType longitude, latitude;	// longitude and latitude coordinates
	string nameOfDeliveryCenter;	// name of the delivery center
	string provinceAffiliated;		// affiliated province
	CapacityType capacity;			// capacity of the delivery center
	vector<GoodsInfo> goods;
	vector<int> adjEdgeVec;			// adjacent edge index in edge vec

	TimeType servTime;
	RegionID regionID;
	std::pair<int, int> timeWindow;	// time window
};

class Vehicle
{
public:
	Vehicle(VehicleID _id, string _cid, CostType _cost, CapacityType _capacity) :VehID(_id),
		carrierID(_cid), cost(_cost),capacity(_capacity){}
	const VehicleID VehID;	// identity
	string registrationNumber;	// registration number of the vehicle
	string modelOfVehicle;	// model of the vehicle
	const string carrierID;			// which carrier belongs to
	LengthType length, width, height;	// length, width, and height of the carriage of the vehicle
	const CapacityType capacity;	// capacity of the vehicle
	Client initPosition;	// initial position of the vehicle
	const CostType cost;		// fixed cost
};

class Order
{
	friend std::istream& operator>>(std::istream&, Order&);
	friend std::ostream& operator<<(std::ostream&, const Order&);
public:
	Order(){};
	OrderID getID()const{ return OrdID; }
	ClientID getApplierID()const{ return OrdSupDisCenter; }
	ClientID getRequestID()const{ return OrdDemDisCenter; }
	QuantityType getQuantity()const{ return OrdDemandAmount; }
	OrderType getOrderType()const{ return orderType; }
	CostType getOrderValue()const{ return orderValue; }
	void setApplierID(const ClientID &aid){ OrdSupDisCenter = aid; }
	void setResquestID(const ClientID &rid){ OrdDemDisCenter = rid; }
	void setOrderType(const OrderType &t){ orderType = t; }
	void setReadyTime(const TimeType &rt){ readyTime = rt; }
	void setDueTime(const TimeType &dt){ dueTime = dt; }
	TimeType getReadyTime()const{ return readyTime; }
	TimeType getDueTime()const{ return dueTime; }
private:
	OrderID OrdID;	// identity
	ClientID OrdSupDisCenter, OrdDemDisCenter;	// offer and request vertex of the order
	GoodsID goodsID;	// requried goods ID of the order
	QuantityType OrdDemandAmount;	// request quantity of the order
	OrderType orderType;	// type of the order
	TimeType readyTime, dueTime;	// due time of the order
	CostType orderValue;	// the value of the order
};

// contain load and unload information
class ServeClient
{
public:
	ServeClient(const ClientID &vcd, const QuantityType &cq):
	visitClientID(vcd),currentQuantity(cq){}
	ClientID visitClientID;	// visit ClientID	// unique except depot
	vector<OrderID> loadOrderID, unloadOrderID;	// load and unload goods ID
	vector<QuantityType> loadQuantity, unloadQuantity;	// load and unload quantity
	QuantityType currentQuantity;	// current quantity of goods
	Timer arrivalTime, departureTime;	// arrival and departureTime of the vertex
	DistanceType driveDistance;		//  already drived distance
};
// describe a route corresponding to a vehicle
class Route
{
public:
	Route(){ mandaQuantity = 0; cout << "route constructed." << endl; }
	VehicleID VehID;	// corresponding vehicle
	list<OrderID> serveOrderList;	// load information
	//vector<vector<OrderID>> arrangedOrderVec;	// the sequence of served OrderID
	//vector<ClientID> servedClientIDVec;	// the served ClientID sequence
	//vector<QuantityType> quantityGoods;	// the quantity of goods in vehicle

	list<ServeClient> serveClientList;	// served client list

	Timer beginTime, endTime;	// begin and end time of the route
	DistanceType routeDistance; // total distance of the route
	ObjectType routeObject;		// objective value of the route
	ObjectType routeWeightDistance;	// objective value of weight multiply distance
	ObjectType fullLoadRate;	// full load rate of the route
	QuantityType mandaQuantity;	// quantity of mandatory order
};
// describe a carrier
class Carrier {
public:
	Carrier(string &cid, string &bid,int &numIncompa) :
		carrierID(cid), billingID(bid),numIncompatibleRegion(numIncompa){ }
	CarrierID getID() const { return carrierID; }
	CarrierID getBilling() const { return billingID; }
	void addIncompatRegion(const string &regionID){
		incompatRegions.push_back(regionID);
	}
	const vector<string>& getIncompatRegions(){ return incompatRegions; };
private:
	const CarrierID carrierID, billingID;
	const int numIncompatibleRegion;
	vector<string> incompatRegions;
};

class Solution
{
public:
	Solution(DistanceType rd, ObjectType ro, ObjectType rwd, ObjectType fr) :
		totalDistance(rd), totalObject(ro), totalWeightDistance(rwd), averagefullLoadRate(fr){}
	vector<Route> routeVec;	// route list
	int numMandatoryOrder, numOptimalOrder;	// number of mandatory and optimal order

	DistanceType totalDistance; // total distance of the solution
	ObjectType totalObject;		// objective value of the solution
	ObjectType totalWeightDistance;	// objective value of weight multiply distance
	ObjectType averagefullLoadRate;	// full load rate of the solution
};



class Output
{
public:
	Output();
	Timer currentTime;
private:
	//VehicleRouting vr;
};

#endif