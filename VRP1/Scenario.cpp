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

std::istream& operator>>(std::istream &is, Edge &e)
{
	is >> e.edge.first >> e.edge.second >> e.distance >> e.timeDistance;
	return is;
}

std::istream& operator>>(std::istream &is, Order &o)
{
	int ot,temp;
	is >> o.OrdID >> o.OrdDemDisCenter >> o.OrdDemandAmount
		//>> ot >> o.readyTime >> o.OrdDeadline;
		>> ot >> temp >> temp;
	o.orderType = ot == 1 ? OrderType::Mandatory : OrderType::Optional;
	o.orderValue = 300;
	return is;
}
std::ostream& operator<<(std::ostream &is, const Order &o)
{
	//is << o.OrdID << "\t" << o.OrdSupDisCenter << "\t" << o.OrdDemDisCenter << "\t" << o.OrdDemandAmount << "\t"
	//	<< o.orderType << "\t" << o.readyTime << "\t" << o.OrdDeadline;
	//o.orderType = ot == 1 ? OrderType::Mandatory : OrderType::Optional;
	is << o.OrdID << " " << o.OrdSupDisCenter << " " << o.OrdDemDisCenter << " "
		<<o.OrdDemandAmount << " "<< o.orderType;
	return is;
}
