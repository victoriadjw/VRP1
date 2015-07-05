#include"Scenario.h"

std::istream& operator>>(std::istream &is, Client &c)
{
	is >> c.ID >> c.IDRegion >> c.timeWindow.first
		>> c.timeWindow.second >> c.servTime;
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
