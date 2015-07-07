#include"Scenario.h"

std::istream& operator>>(std::istream &is, Client &c)
{
	is >> c.PriDCID >> c.regionID >> c.timeWindow.first
		>> c.timeWindow.second >> c.servTime;
	return is;
}
std::ostream& operator<<(std::ostream &is, const Edge &e)
{
	is << e.edge.first << "\t" << e.edge.second << "\t" 
		<< e.distance << "\t" << e.timeDistance;
	return is;
}

std::istream& operator>>(std::istream &is, Edge &e)
{
	is >> e.edge.first >> e.edge.second >> e.distance >> e.timeDistance;
	return is;
}

std::istream& operator>>(std::istream &is, Order &o)
{
	int ot;
	is >> o.OrdID >> o.OrdDemDisCenter >> o.OrdDemandAmount
		>> ot >> o.readyTime >> o.OrdDeadline;
	o.orderType = ot == 1 ? OrderType::Mandatory : OrderType::Optional;
	return is;
}
std::ostream& operator<<(std::ostream &is, const Order &o)
{
	//int ot;
	is << o.OrdID << "\t" << o.OrdSupDisCenter << "\t" << o.OrdDemDisCenter << "\t" << o.OrdDemandAmount << "\t"
		<< o.orderType << "\t" << o.readyTime << "\t" << o.OrdDeadline;
	//o.orderType = ot == 1 ? OrderType::Mandatory : OrderType::Optional;
	return is;
}
