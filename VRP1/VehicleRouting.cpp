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
			if (rand() % 4 == 0) // change the order to mandatory with probability of 1/6
			{
				(*iter).setOrderType(OrderType::Mandatory);
			}
			else
			{
				int vindex = rand() % clientVec.size();
				while (vindex == 0 || clientVec[vindex].PriDCID == (*iter).getRequestID())
					vindex = rand() % clientVec.size();
				(*iter).setApplierID(clientVec[vindex].PriDCID);
			}
		}
		cout << *iter << endl;
	}
}

class VehicleRouting::CmpDistance{
public:
	CmpDistance(const vector<Edge> &_edgeVec, const vector<Order> &_orderVec,
		map<ClientID, int> &_clientMap,
		const vector<vector<int>> &_orderEdge)
		:edgeVec(_edgeVec), orderVec(_orderVec),
		clientMap(_clientMap),orderEdge(_orderEdge){}
	// sort to (least...greatest)
	bool operator()(const int &l, const int &r)
	{
		int le = orderEdge[clientMap[orderVec[l].getApplierID()]][clientMap[orderVec[l].getRequestID()]];
		int re = orderEdge[clientMap[orderVec[r].getApplierID()]][clientMap[orderVec[r].getRequestID()]];
		return edgeVec[le].getDistance()<edgeVec[re].getDistance();
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
	map<ClientID, int> &clientMap;
	const vector<vector<int>> &orderEdge;
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
		else
		{
			optionalOrderIndexVec.push_back(i);
		}
	}
	std::sort(mandatoryOrderIndexVec.begin(), mandatoryOrderIndexVec.end(), CmpDistance(edgeVec, orderVec, clientMap, orderEdge));
	sort(optionalOrderIndexVec.begin(), optionalOrderIndexVec.end(), CmpDistance(edgeVec, orderVec, clientMap, orderEdge));
	cout << "sorted mandatory order:" << mandatoryOrderIndexVec.size() << endl;
	CmpDistance cd(edgeVec, orderVec, clientMap, orderEdge);
	for (std::vector<int>::iterator iter = mandatoryOrderIndexVec.begin(); iter != mandatoryOrderIndexVec.end(); iter++)
	{
		cout << orderVec[*iter] <<"\t"<<*iter<<"\t"<<cd.getDistanceByOrderIndex(*iter)<< endl;
	}
	cout << "sorted optional order:" << optionalOrderIndexVec.size() << endl;
	for (std::vector<int>::iterator iter = optionalOrderIndexVec.begin(); iter != optionalOrderIndexVec.end(); iter++)
	{
		cout << orderVec[*iter] << "\t" << *iter << "\t" << cd.getDistanceByOrderIndex(*iter) << endl;
	}
	//vector<Route> routeVec;
	for (vector<Vehicle>::iterator iter = vehicleVec.begin(); iter != vehicleVec.end(); iter++)
	{
		Route r;
		r.VehID = (*iter).VehID;
		solution.routeVec.push_back(r);
	}
	// assign mandatory order to each route, confine to vehicle's capacity
	for (vector<int>::iterator iter = mandatoryOrderIndexVec.begin(); iter != mandatoryOrderIndexVec.end(); iter++)
	{
		int rd = rand() % solution.routeVec.size();
		if (solution.routeVec[rd].mandaQuantity + orderVec[*iter].getQuantity()>vehicleVec[vehicleMap[solution.routeVec[rd].VehID]].capacity)
			rd = rand() % solution.routeVec.size();
		solution.routeVec[rd].serveOrderList.push_back(orderVec[*iter].getID());
	}
	// assign optional order to each route
	for (vector<int>::iterator iter = optionalOrderIndexVec.begin(); iter != optionalOrderIndexVec.end(); iter++)
	{
		solution.routeVec[rand() % solution.routeVec.size()].serveOrderList.push_back(orderVec[*iter].getID());
	}
	for (vector<Route>::iterator iter = solution.routeVec.begin(); iter != solution.routeVec.end(); iter++)
	{
		cout << (*iter).VehID << " : " << endl;;
		for (list<OrderID>::iterator it = (*iter).serveOrderList.begin();
			it != (*iter).serveOrderList.end(); it++)
		{
			cout << orderVec[orderMap[*it]] <<"\t"<<cd.getDistanceByOrderIndex(orderMap[*it])<< endl;
		}
	}
	generateRoute(1);
}
void VehicleRouting::generateRoute(const int &rin)
{
	// find the optional MM order with the in degree of the initial vertex is 0, 
	// note that the in degree of all MM order may be larger than 0
	set<ClientID> MMOrderClientIDSet;	// the ClientID set of mandatory order
	int mand_order_cnt = 0;
	for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
		iter != solution.routeVec[rin].serveOrderList.end(); iter++)
	{
		if (orderVec[orderMap[*iter]].getOrderType() == OrderType::Mandatory)
		{
			MMOrderClientIDSet.insert(orderVec[orderMap[*iter]].getApplierID());
			MMOrderClientIDSet.insert(orderVec[orderMap[*iter]].getRequestID());
			mand_order_cnt += 1;
		}
		cout << *iter << endl;
	}
	cout << "client id of mandatory order: \n";
	for (set<ClientID>::iterator iter = MMOrderClientIDSet.begin();
		iter != MMOrderClientIDSet.end(); iter++)
	{
		cout << *iter << "\t";
	}
	vector<OrderID> MMOptionalOrderVec;		// the optional MM order
	set<ClientID> MMOrderInitClientIDSet;	// the initial ClientID set of the optional MM order
	for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
		iter != solution.routeVec[rin].serveOrderList.end(); iter++)
	{
		if (orderVec[orderMap[*iter]].getOrderType() == OrderType::Optional&&
			MMOrderClientIDSet.count(orderVec[orderMap[*iter]].getApplierID()) == 1&&
			MMOrderClientIDSet.count(orderVec[orderMap[*iter]].getRequestID()) == 1)
		{
			MMOptionalOrderVec.push_back(*iter);
			MMOrderInitClientIDSet.insert(orderVec[orderMap[*iter]].getApplierID());
			cout << *iter << endl;
		}
	}
	vector<OrderID> MMOrderInitVec;	// the optional order where the initial ClientID are not in MMOrderInitClientIDSet
	for (vector<OrderID>::iterator iter = MMOptionalOrderVec.begin();
		iter != MMOptionalOrderVec.end(); iter++)
	{
		if (MMOrderInitClientIDSet.count(orderVec[orderMap[*iter]].getRequestID()) == 0)
			MMOrderInitVec.push_back(*iter);
	}
	if (MMOrderInitVec.size() == 0)
	{
		// there exists a circle, at least one order 
		// should be served by other vehicles.
		// remove an order to other route
	}
	cout << MMOrderInitVec.size();
	cout << "arrange the order sequence: " << endl;
	set<OrderID> servedOrderIDSet;
	ClientID &initClientID = clientVec[0].PriDCID;
	cout << initClientID << " -> ";
	while (servedOrderIDSet.size() != mand_order_cnt)
	{
		OrderID least_oid = findLeastCostOrder(servedOrderIDSet, solution.routeVec[rin].serveOrderList, initClientID);
		servedOrderIDSet.insert(least_oid);
		initClientID = orderVec[orderMap[least_oid]].getRequestID();
		cout <<least_oid <<" " << initClientID << " -> ";
	}
}
// find the OrderID of the least cost order in servedOrderList/servedOrderIDSet with respect to initClientID
OrderID VehicleRouting::findLeastCostOrder(const set<OrderID> &servedOrderIDSet,const list<OrderID> &servedOrderList, const ClientID &initClientID)
{
	DistanceType least_dis=INT16_MAX;
	OrderID least_oid;
	for (list<OrderID>::const_iterator iter = servedOrderList.begin(); iter != servedOrderList.end(); iter++)
	{
		if (servedOrderIDSet.count(*iter) == 1||
			orderVec[orderMap[*iter]].getOrderType()==OrderType::Optional)
			continue;
		DistanceType cur_dis = edgeVec[orderEdge[clientMap[initClientID]][clientMap[orderVec[orderMap[*iter]].getRequestID()]]].getDistance();
		if (least_dis > cur_dis)
		{
			least_dis = cur_dis;
			least_oid = *iter;
		}
	}
	return least_oid;
}