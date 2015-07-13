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
		r.mandaQuantity = 0;
		solution.routeVec.push_back(r);
	}
	// assign mandatory order to each route, confine to vehicle's capacity
	for (vector<int>::iterator iter = mandatoryOrderIndexVec.begin(); iter != mandatoryOrderIndexVec.end(); iter++)
	{
		int rd = rand() % solution.routeVec.size();
		if (solution.routeVec[rd].mandaQuantity + orderVec[*iter].getQuantity()>vehicleVec[vehicleMap[solution.routeVec[rd].VehID]].capacity)
			rd = rand() % solution.routeVec.size();
		solution.routeVec[rd].mandaQuantity += orderVec[*iter].getQuantity();
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
	vector<OrderID>MMOrderVec;
	for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
		iter != solution.routeVec[rin].serveOrderList.end(); iter++)
	{
		if (orderVec[orderMap[*iter]].getOrderType() == OrderType::Mandatory)
		{
			MMOrderClientIDSet.insert(orderVec[orderMap[*iter]].getApplierID());
			MMOrderClientIDSet.insert(orderVec[orderMap[*iter]].getRequestID());
			MMOrderVec.push_back(*iter);
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
	cout << endl << "the optional MM order:" << endl;
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
	cout << endl << MMOrderInitVec.size();	
	// arrange the mandatory and MM optional orders
	cout << "arrange the order sequence: " << endl;
	int servedOrderIDCnt=0;
	ClientID &initClientID = clientVec[0].PriDCID;
	cout << initClientID << " -> ";
	solution.routeVec[rin].servedClientIDVec.push_back(initClientID);
	solution.routeVec[rin].quantityGoods.push_back(solution.routeVec[rin].mandaQuantity);
	while (servedOrderIDCnt != MMOrderVec.size())
	{
		vector<OrderID> oid_vec;
		OrderID least_oid = findLeastCostOrder(solution.routeVec[rin].serveOrderList, initClientID);
		initClientID = orderVec[orderMap[least_oid]].getRequestID();
		oid_vec.push_back(least_oid);
		servedOrderIDCnt += 1;
		solution.routeVec[rin].serveOrderList.remove(least_oid);
		if (solution.routeVec[rin].servedClientIDVec.back() != initClientID)
		{
			solution.routeVec[rin].servedClientIDVec.push_back(initClientID);
			solution.routeVec[rin].arrangedOrderVec.push_back(oid_vec);
		}else
			solution.routeVec[rin].arrangedOrderVec.back().push_back(least_oid);
		cout << least_oid << " " << initClientID << " -> ";
	}
	// repair the mandatory order to adjust the MM optional order
	for (vector<OrderID>::iterator iter = MMOptionalOrderVec.begin();
		iter != MMOptionalOrderVec.end(); iter++)
	{
		ClientID &init_cid = orderVec[orderMap[*iter]].getApplierID();
		ClientID &term_cid = orderVec[orderMap[*iter]].getRequestID();
		vector<ClientID>::iterator init_it = find(solution.routeVec[rin].servedClientIDVec.begin(), 
			solution.routeVec[rin].servedClientIDVec.end(), init_cid);
		vector<ClientID>::iterator term_it = find(solution.routeVec[rin].servedClientIDVec.begin(), 
			solution.routeVec[rin].servedClientIDVec.end(), term_cid);
		vector<ClientID>::iterator init_it1 = init_it;
		while (init_it1 != term_it&&init_it1 != solution.routeVec[rin].servedClientIDVec.end())
			init_it1++;
		if (init_it1 == solution.routeVec[rin].servedClientIDVec.end())
		{
			// the MM optional order is opposite to the arranged list
			int init_oid, term_oid, oid_cnt = 0;
			for (vector<vector<OrderID>>::iterator it = solution.routeVec[rin].arrangedOrderVec.begin();
				it != solution.routeVec[rin].arrangedOrderVec.end(); it++,oid_cnt++)
			{
				if (orderVec[orderMap[(*it).front()]].getRequestID() == init_cid)
					init_oid = oid_cnt;
				if (orderVec[orderMap[(*it).front()]].getRequestID() == term_cid)
					term_oid = oid_cnt;
			}
			swap(solution.routeVec[rin].arrangedOrderVec[init_oid], solution.routeVec[rin].arrangedOrderVec[term_oid]);
		}
	}
	// calculate the current quantity of goods at each customer
	solution.routeVec[rin].quantityGoods.push_back(solution.routeVec[rin].mandaQuantity);
	for (vector<vector<OrderID>>::iterator iter = solution.routeVec[rin].arrangedOrderVec.begin();
		iter != solution.routeVec[rin].arrangedOrderVec.end(); iter++)
	{
		QuantityType sum_quan = solution.routeVec[rin].quantityGoods.back();
		for (vector<OrderID>::iterator it = (*iter).begin(); it != (*iter).end(); it++)
		{
			sum_quan -= orderVec[orderMap[*it]].getQuantity();
		}
		solution.routeVec[rin].quantityGoods.push_back(sum_quan);
	}
	// print the route information
	int index = 0;
	for (vector<vector<OrderID>>::iterator iter = solution.routeVec[rin].arrangedOrderVec.begin();
		iter != solution.routeVec[rin].arrangedOrderVec.end(); iter++,index++)
	{
		cout << (*iter).size() << "\t" << solution.routeVec[rin].quantityGoods[index+1] << ":";
		for (vector<OrderID>::iterator it = (*iter).begin(); it != (*iter).end(); it++)
		{
			cout << *it << " ";
		}
		cout << endl;
	}
	// insert the MO and OM optional order to the already arranged orders

}
// find the OrderID of the least cost order in servedOrderList/servedOrderIDSet with respect to initClientID
OrderID VehicleRouting::findLeastCostOrder(const list<OrderID> &servedOrderList, const ClientID &initClientID)
{
	DistanceType least_dis=INT16_MAX;
	OrderID least_oid;
	for (list<OrderID>::const_iterator iter = servedOrderList.begin(); iter != servedOrderList.end(); iter++)
	{
		if (orderVec[orderMap[*iter]].getOrderType()==OrderType::Optional)
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