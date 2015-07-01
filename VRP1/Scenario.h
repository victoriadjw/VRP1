/*
*		usage: 1. contain data for identifying a specific VRP.
*			   2. provide solver interface.
*/

#ifndef SCENARIO_H
#define SCENARIO_H

#include"Utility.h"

typedef double GeographyType;
typedef int CapacityType;
typedef int UnitVolumeType;
typedef int UnitWeightType;
typedef double LengthType;
typedef int GoodsID;
typedef int VertexID;
typedef int VehicleID;
typedef int OrderID;
typedef int QuantityType;
typedef int DistanceType;
typedef double ObjectType;

enum OrderType{Mondatory, Optional};

class GoodsInfo
{
public:
	GoodsID id;	// identity
	string nameOfGoods;		// name of the goods
	QuantityType quantity;	// quantity of the goods
	CapacityType outputPerMonth;	// output per month
	UnitVolumeType unitVolume;		// unit volume
	UnitWeightType unitWeight;		// unit weight
};

class ArcInfo
{
public:
	int distance;
};
class VertexInfo
{
public:

	VertexID id;	// identity
	GeographyType longitude, latitude;	// longitude and latitude coordinates
	string nameOfDeliveryCenter;	// name of the delivery center
	string provinceAffiliated;		// affiliated province
	CapacityType capacity;			// capacity of the delivery center
	vector<GoodsInfo> goods;
};

class Vehicle
{
public:
	VehicleID id;	// identity
	string registrationNumber;	// registration number of the vehicle
	string modelOfVehicle;	// model of the vehicle
	LengthType length, width, height;	// length, width, and height of the carriage of the vehicle
	CapacityType capacity;	// capacity of the vehicle
	VertexInfo initPosition;	// initial position of the vehicle
};

class Order
{
public:
	OrderID id;	// identity
	VertexInfo &offerVertex, &requestVertex;	// offer and request vertex of the order
	GoodsID goodsID;	// requried goods ID of the order
	QuantityType requestQuantity;	// request quantity of the order
	OrderType orderType;	// type of the order
	Timer dueTime;	// due time of the order
};

// contain load and unload information
class LoadInfo
{
public:
	VertexID vertexID;	// load and unload vertex
	OrderID resolvedOrderID;	// resolved order ID 
	vector<GoodsID> loadGoodsID, unloadGoodsID;	// load and unload goods ID
	vector<QuantityType> loadQuantity, unloadQuantity;	// load and unload quantity
	Timer arrivalTime, departureTime;	// arrival and departureTime of the vertex
	DistanceType driveDistance;		//  already drived distance
};
// describe a route corresponding to a vehicle
class Route
{
public:
	VehicleID vehicleID;	// corresponding vehicle
	list<LoadInfo *> loadInfo;	// load information
	Timer beginTime, endTime;	// begin and end time of the route
	DistanceType routeDistance; // total distance of the route
	ObjectType routeObject;		// objective value of the route
};

class Solution
{
public:
	list<Route *> routeList;	// route list
	ObjectType solutionObjective;	// objective value of the solution
	int numMandatoryOrder, numOptimalOrder;	// number of mandatory and optimal order
};


class Input
{

};

class Output
{

};

#endif