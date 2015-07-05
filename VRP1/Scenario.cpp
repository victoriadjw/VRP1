#include"Scenario.h"

std::istream& operator>>(std::istream &is, Client &c)
{
	is >> c.id >> c.regionID >> c.timeWindow.first
		>> c.timeWindow.second >> c.servTime;
	return is;
}
std::istream& operator>>(std::istream &is, VertexInfo &v)
{	
	is >> v.id >> v.regionID >> v.timeWindow.first
		>> v.timeWindow.second >> v.servTime;
	return is;
}
std::ostream& operator<<(std::ostream &is, const EdgeInfo &e)
{
	is << e.edge.first << "\t" << e.edge.second << "\t" 
		<< e.distance << "\t" << e.timeDistance;
	return is;
}

std::istream& operator>>(std::istream &is, EdgeInfo &e)
{
	is >> e.edge.first >> e.edge.second >> e.distance >> e.timeDistance;
	return is;
}

std::istream& operator>>(std::istream &is, Order &o)
{
	int ot;
	is >> o.id >> o.requestID >> o.requestQuantity
		>> ot >> o.readyTime>>o.dueTime;
	o.orderType = ot == 1 ? OrderType::Mondatory : OrderType::Optional;
	return is;
}
std::ostream& operator<<(std::ostream &is, const Order &o)
{
	//int ot;
	is << o.id << " " << o.requestID << " " << o.requestQuantity << " "
		<< o.orderType << " " << o.readyTime << " " << o.dueTime;
	//o.orderType = ot == 1 ? OrderType::Mondatory : OrderType::Optional;
	return is;
}
