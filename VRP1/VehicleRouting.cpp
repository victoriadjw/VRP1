#include "VehicleRouting.h"
#include<algorithm>

void VehicleRouting::setName(const string &_name){ name = _name; }
void VehicleRouting::setNumBilling(const int &_numBilling){	numBilling = _numBilling;}
void VehicleRouting::setNumCarrier(const int &_numCarrier){	numCarrier = _numCarrier;}
void VehicleRouting::setNumClient(const int &_numClient){ numClient = _numClient;}
void VehicleRouting::setNumOrder(const int &_numOrder){	numOrder = _numOrder;}
void VehicleRouting::setNumVehicle(const int &_numVehicle){	numVehicle = _numVehicle;}
void VehicleRouting::setNumRegion(const int &_numRegion){ numRegion = _numRegion;}
void VehicleRouting::setPlanHorizon(const int &start, const int &end)
{
	CyclePlan .first = start;
	CyclePlan .second = end;
}

int VehicleRouting::getNumClient()const{ return numClient; }
int VehicleRouting::getNumOrder()const{ return numOrder; }
int VehicleRouting::getNumVehicle()const{ return numVehicle; }
int VehicleRouting::getNumCarrier()const{ return numCarrier; }
int VehicleRouting::getNumBilling()const{ return numBilling; }
int VehicleRouting::getNumRegion()const{ return numRegion; }

void VehicleRouting::calculateDistance()
{
	for (vector<Edge>::iterator iter = edgeVec.begin(); iter != edgeVec.end(); iter++)
	{
		(*iter).setDistance(getDistance(clientVec[clientMap[(*iter).getEdge().first]].latitude,
			clientVec[clientMap[(*iter).getEdge().first]].longitude,
			clientVec[clientMap[(*iter).getEdge().second]].latitude,
			clientVec[clientMap[(*iter).getEdge().second]].longitude));
	}
}
void VehicleRouting::modifyOrder()
{
	for (vector<Order>::iterator iter = orderVec.begin(); iter != orderVec.end(); iter++)
	{
		if ((*iter).getOrderType() == OrderType::Optional)
		{
			if (rand() % 6 == 0) // change the order to mandatory with probability of 1/6
			{
				(*iter).setOrderType(OrderType::Mandatory);
			}
			else
			{
				int vindex = rand() % clientVec.size();
				while (vindex != 0 && clientVec[vindex].PriDCID == (*iter).getRequestID())
					vindex = rand() % clientVec.size();
				(*iter).setApplierID(clientVec[vindex].PriDCID);
			}
		}
		cout << *iter << endl;
	}
}

class VehicleRouting::CmpDistance{
public:
	CmpDistance(const vector<Edge> &_edgeVec,const vector<Order> &_orderVec)
		:edgeVec(_edgeVec), orderVec(_orderVec){}
	// sort to (least...greatest)
	bool operator()(const int &l, const int &r)
	{
		return getDistanceByOrderIndex(l) < getDistanceByOrderIndex(r);
	}
	DistanceType getDistanceByOrderIndex(const int &oin)
	{
		ClientID &apid = orderVec[oin].getApplierID();
		ClientID &rqid = orderVec[oin].getRequestID();
		
		for (std::vector<Edge>::const_iterator iter = edgeVec.begin(); iter != edgeVec.end(); iter++)
		{
			if ((*iter).getEdge().first == apid && (*iter).getEdge().second == rqid)
				return (*iter).getDistance();
		}
		return -1;
	}
private:
	const vector<Edge> &edgeVec;
	const vector<Order> &orderVec;
};
void VehicleRouting::assign()
{
	cout << "mandatory order:" << endl;
	int i = 0;
	for (std::vector<Order>::iterator iter = orderVec.begin(); iter != orderVec.end(); iter++,i++)
	{
		if ((*iter).getOrderType() == OrderType::Mandatory)
		{
			cout << i << " : " << *iter << endl;
			mandatoryOrderIndexVec.push_back(i);
		}
		std::sort(mandatoryOrderIndexVec.begin(), mandatoryOrderIndexVec.end(), CmpDistance(edgeVec,orderVec));
	}
	cout << "sorted mandatory order:" << endl;
	CmpDistance cd(edgeVec, orderVec);
	for (std::vector<int>::iterator iter = mandatoryOrderIndexVec.begin(); iter != mandatoryOrderIndexVec.end(); iter++)
	{
		cout << orderVec[*iter] <<"\t"<<*iter<<"\t"<<cd.getDistanceByOrderIndex(*iter)<< endl;
	}
	vector<Route> routeVec;
	for (vector<Vehicle>::iterator iter = vehicleVec.begin(); iter != vehicleVec.end(); iter++)
	{
		Route r;
		r.VehID = (*iter).VehID;
		routeVec.push_back(r);
	}
	for (vector<int>::iterator iter = mandatoryOrderIndexVec.begin(); iter != mandatoryOrderIndexVec.end(); iter++)
	{
		int rd = rand() % routeVec.size();
		if (routeVec[rd].mandaQuantity + orderVec[*iter].getQuantity()>vehicleVec[vehicleMap[routeVec[rd].VehID]].capacity)
			rd = rand() % routeVec.size();
		routeVec[rd].serveOrderList.push_back(orderVec[*iter].getID());
	}
	for (vector<Route>::iterator iter = routeVec.begin(); iter != routeVec.end(); iter++)
	{
		cout << (*iter).VehID << " : " << endl;;
		for (list<OrderID>::iterator it = (*iter).serveOrderList.begin();
			it != (*iter).serveOrderList.end(); it++)
		{
			cout << orderVec[orderMap[*it]] <<"\t"<<cd.getDistanceByOrderIndex(orderMap[*it])<< endl;
		}
	}

}