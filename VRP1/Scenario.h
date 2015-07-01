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
	Timer dueTime;	// due time of the order
};

class Input
{

};

class Output
{

};

#endif