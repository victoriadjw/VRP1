#include"Scenario.h"

std::istream& operator>>(std::istream &is, Client &c)
{
	int temp;
	is >> c.PriDCID >> c.clientName >> c.timeWindow.first
		//>> c.timeWindow.second >> c.servTime
		>> c.timeWindow.second >> temp
		>> c.longitude >> c.latitude;
	return is;
}
std::ostream& operator<<(std::ostream &is, const Edge &e)
{
	is << e.edge.first << "\t" << e.edge.second << "\t" 
		<< e.distance << "\t" << e.timeDistance;
	return is;
}

std::istream& operator>>(std::istream &is, Vehicle &v)
{
	is >> v.VehID >> v.capacity >> v.cost >> v.carrierID >> v.speed;
	return is;
}
std::ostream& operator<<(std::ostream &os, const Vehicle &v)
{
	os << v.VehID << "\t" << v.capacity << "\t" << v.cost
		<< "\t" << v.carrierID << "\t" << v.speed;
	return os;
}
std::istream& operator>>(std::istream &is, Edge &e)
{
	is >> e.edge.first >> e.edge.second >> e.distance >> e.timeDistance;
	return is;
}

std::istream& operator>>(std::istream &is, Order &o)
{
	int ot,temp;
	is >> o.OrdID >> o.OrdDemDisCenter >> o.OrdDemandAmount
		>> ot >> temp >> temp >> o.readyTime >> o.dueTime;
	o.orderType = ot == 1 ? OrderType::Mandatory : OrderType::Optional;
	o.orderValue = 300;
	return is;
}
std::ostream& operator<<(std::ostream &is, const Order &o)
{
	is << o.OrdID << " " << o.OrdSupDisCenter << " " << o.OrdDemDisCenter << " "
		<< o.OrdDemandAmount << " " << o.orderType
		<< o.readyTime << " " << o.dueTime;
	return is;
}
