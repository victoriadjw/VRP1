#include"Solver.h"


void Solver::assign()
{
	//vector<Route> routeVec;
	for (vector<Vehicle>::const_iterator iter = vr.vehicleVec.begin(); iter != vr.vehicleVec.end(); iter++)
	{
		Route r;
		r.VehID = (*iter).VehID;
		r.mandaQuantity = 0;
		solution.routeVec.push_back(r);
	}
	// assign mandatory order to each route, confine to vehicle's capacity
	vector<int> moiv = vr.getMandatoryOrderIndexVec();
	for (vector<int>::iterator iter =moiv.begin(); iter != moiv.end(); iter++)
	{
		int rd = rand() % solution.routeVec.size();
		if (solution.routeVec[rd].mandaQuantity + vr.orderVec[*iter].getQuantity()>
			vr.vehicleVec[vr.vehicleMap[solution.routeVec[rd].VehID]].capacity)
			rd = rand() % solution.routeVec.size();
		solution.routeVec[rd].mandaQuantity += vr.orderVec[*iter].getQuantity();
		solution.routeVec[rd].serveOrderList.push_back(vr.orderVec[*iter].getID());
	}
	
	// assign optional order to each route
	vector<int> ooiv = vr.getOptionalOrderIndexVec();
	for (vector<int>::iterator iter = ooiv.begin(); iter != ooiv.end(); iter++)
	{
		solution.routeVec[rand() % solution.routeVec.size()].serveOrderList.push_back(vr.orderVec[*iter].getID());
	}
	for (vector<Route>::iterator iter = solution.routeVec.begin(); iter != solution.routeVec.end(); iter++)
	{
		cout << (*iter).VehID <<", "<<(*iter).serveOrderList.size()<< " : " << endl;;
		for (list<OrderID>::iterator it = (*iter).serveOrderList.begin();
			it != (*iter).serveOrderList.end(); it++)
		{
			cout << vr.orderVec[vr.orderMap[*it]] << "\t" << vr.getDistanceByOrderIndex(*it) << endl;
		}
	}

	generateRoute(1);
}

void Solver::generateRoute(const int &rin)
{
	// find the optional MM order with the in degree of the initial vertex is 0, 
	// note that the in degree of all MM order may be larger than 0
	set<ClientID> MMOrderClientIDSet;	// the ClientID set of mandatory order
	vector<OrderID>MMOrderVec;
	for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
		iter != solution.routeVec[rin].serveOrderList.end(); iter++)
	{
		if (vr.orderVec[vr.orderMap[*iter]].getOrderType() == OrderType::Mandatory)
		{
			MMOrderClientIDSet.insert(vr.orderVec[vr.orderMap[*iter]].getApplierID());
			MMOrderClientIDSet.insert(vr.orderVec[vr.orderMap[*iter]].getRequestID());
			MMOrderVec.push_back(*iter);
		}
		cout << *iter << endl;
	}
	cout <<solution.routeVec[rin].serveOrderList.size()<< "\tclient id of mandatory order: \n";
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
		if (vr.orderVec[vr.orderMap[*iter]].getOrderType() == OrderType::Optional&&
			MMOrderClientIDSet.count(vr.orderVec[vr.orderMap[*iter]].getApplierID()) == 1 &&
			MMOrderClientIDSet.count(vr.orderVec[vr.orderMap[*iter]].getRequestID()) == 1)
		{
			MMOptionalOrderVec.push_back(*iter);
			MMOrderInitClientIDSet.insert(vr.orderVec[vr.orderMap[*iter]].getApplierID());
			cout << *iter << "\t";
		}
	}
	cout << endl;
	vector<OrderID> MMOrderInitVec;	// the optional order where the initial ClientID are not in MMOrderInitClientIDSet
	for (vector<OrderID>::iterator iter = MMOptionalOrderVec.begin();
		iter != MMOptionalOrderVec.end(); iter++)
	{
		if (MMOrderInitClientIDSet.count(vr.orderVec[vr.orderMap[*iter]].getRequestID()) == 0)
			MMOrderInitVec.push_back(*iter);
	}
	if (MMOrderInitVec.size() == 0)
	{
		// there exists a circle, at least one order 
		// should be served by other vehicles.
		// remove an order to other route
	}
	// arrange the mandatory and MM optional orders
	cout << "arrange the order sequence: " << endl;
	ClientID init_cid = vr.clientVec[0].PriDCID;
	ServeClient sc(init_cid, 0);
	for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
		iter != solution.routeVec[rin].serveOrderList.end(); iter++)
	{
		if (vr.orderVec[vr.orderMap[*iter]].getOrderType() == OrderType::Mandatory)
		{
			sc.loadOrderID.push_back(*iter);
			sc.currentQuantity += vr.orderVec[vr.orderMap[*iter]].getQuantity();
		}
	}
	solution.routeVec[rin].serveClientList.push_back(sc);

	cout << init_cid << " -> ";
	MMOrderClientIDSet.erase(vr.clientVec[0].PriDCID);
	while (!MMOrderClientIDSet.empty())
	{		
		ClientID sel_end_cid;
		DistanceType shortest_distance;
		vector<ClientID> shortest_cid_vec;
		dsp->getShortPathClient(init_cid, MMOrderClientIDSet, sel_end_cid, shortest_distance, shortest_cid_vec);
		MMOrderClientIDSet.erase(sel_end_cid);

		ServeClient sc1(sel_end_cid, solution.routeVec[rin].serveClientList.back().currentQuantity);
		for (vector<OrderID>::iterator iter = MMOrderVec.begin(); iter != MMOrderVec.end(); iter++)
		{
			if (vr.orderVec[vr.orderMap[*iter]].getRequestID() == sel_end_cid)
			{
				solution.routeVec[rin].serveOrderList.remove(*iter);
				sc1.unloadOrderID.push_back(*iter);
				sc1.currentQuantity -= vr.orderVec[vr.orderMap[*iter]].getQuantity();
			}
		}
		solution.routeVec[rin].serveClientList.push_back(sc1);
		cout << init_cid << "->";
		init_cid = sel_end_cid;
	}
	// repair the mandatory order to adjust the MM optional order
	for (vector<OrderID>::iterator iter = MMOptionalOrderVec.begin();
		iter != MMOptionalOrderVec.end(); iter++)
	{
		ClientID &init_cid = vr.orderVec[vr.orderMap[*iter]].getApplierID();
		ClientID &term_cid = vr.orderVec[vr.orderMap[*iter]].getRequestID();
		list<ServeClient>::iterator it;
		for (it = solution.routeVec[rin].serveClientList.begin();
			it != solution.routeVec[rin].serveClientList.end(); it++)
		{
			if (it->visitClientID == init_cid || it->visitClientID == term_cid)
				break;
		}
		// find the initial and terminal ServeClient of the MM optional order
		list<ServeClient>::iterator  init_scid, term_scid;
		for (list<ServeClient>::iterator itr = solution.routeVec[rin].serveClientList.begin();
			itr != solution.routeVec[rin].serveClientList.end(); itr++)
		{
			if (itr->visitClientID == init_cid)
				init_scid = itr;
			if (itr->visitClientID == term_cid)
				term_scid = itr;
		}
		if (it->visitClientID == init_cid)	// the order is along with the route
		{
			// if adding the MM optional order does not exceed the vehicle capacity, add it
			if (init_scid->currentQuantity + vr.orderVec[vr.orderMap[*iter]].getQuantity() <=
				vr.vehicleVec[vr.vehicleMap[solution.routeVec[rin].VehID]].capacity)
			{
				init_scid->loadOrderID.push_back(*iter);
				term_scid->unloadOrderID.push_back(*iter);
				for (list<ServeClient>::iterator it1 = init_scid; it1 != term_scid; it1++)
				{
					it1->currentQuantity += vr.orderVec[vr.orderMap[*iter]].getQuantity();
				}
				solution.routeVec[rin].serveOrderList.remove(*iter);
			}
			continue;
		}
		// the MM optional order is opposite to the arranged list
		QuantityType unload_init, unload_term;
		unload_init = unload_term = 0;
		for (vector<OrderID>::iterator it1 = init_scid->unloadOrderID.begin();
			it1 != init_scid->unloadOrderID.end(); it1++)
			unload_init += vr.orderVec[vr.orderMap[*it1]].getQuantity();
		for (vector<OrderID>::iterator it1 = term_scid->unloadOrderID.begin();
			it1 != term_scid->unloadOrderID.end(); it1++)
			unload_term += vr.orderVec[vr.orderMap[*it1]].getQuantity();
		// if adding the reversed MM optional order does not exceed the vehicle capacity, swap it and add it
		if (term_scid->currentQuantity + unload_term - unload_init + vr.orderVec[vr.orderMap[*iter]].getQuantity() <=
			vr.vehicleVec[vr.vehicleMap[solution.routeVec[rin].VehID]].capacity)
		{
			init_scid->loadOrderID.push_back(*iter);
			term_scid->unloadOrderID.push_back(*iter);
			QuantityType temp_init_q = init_scid->currentQuantity;
			init_scid->currentQuantity = term_scid->currentQuantity + unload_term - unload_init + vr.orderVec[vr.orderMap[*iter]].getQuantity();
			term_scid->currentQuantity = temp_init_q;
			it = term_scid;
			for (it++; it != init_scid; it++)
			{
				it->currentQuantity += (unload_term - unload_init + vr.orderVec[vr.orderMap[*iter]].getQuantity());
			}
			swap(init_scid, term_scid);
			solution.routeVec[rin].serveOrderList.remove(*iter);
		}
	}

	// print the route information
	cout << "route information:" << endl;
	for (list<ServeClient>::iterator iter = solution.routeVec[rin].serveClientList.begin();
		iter != solution.routeVec[rin].serveClientList.end(); iter++)
	{
		cout << iter->visitClientID << ", " << (*iter).currentQuantity << ", load order size: " << (*iter).loadOrderID.size() << ", ";
		for (vector<OrderID>::iterator it = (*iter).loadOrderID.begin(); it != (*iter).loadOrderID.end(); it++)
		{
			cout << *it << " ";
		}
		//cout << endl;
		cout << ", unload order size: " << (*iter).unloadOrderID.size() << ":";
		for (vector<OrderID>::iterator it = (*iter).unloadOrderID.begin(); it != (*iter).unloadOrderID.end(); it++)
		{
			cout << *it << " ";
		}
		cout << endl;
	}
	// insert the MO and OM optional order to the already arranged orders
	DistanceType shortest_distance;
	vector<ClientID> shortest_vid_vec;
	for (vector<Client>::const_iterator iter = vr.clientVec.begin();
		iter != vr.clientVec.end(); iter++)
	{
		cout << iter->PriDCID << endl;
	}
	dsp->getShortPath("c0", vr.clientVec[24].PriDCID, shortest_distance, shortest_vid_vec);
	cout << solution.routeVec[rin].serveOrderList.size();

}

// insert optional order (OM, MO and OO) to a route
void Solver::insertOptionalOrderToRoute(const int &rin)
{
	for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
		iter != solution.routeVec[rin].serveOrderList.end(); iter++)
	{
		if (vr.orderVec[vr.orderMap[*iter]].getOrderType() == OrderType::Mandatory)
			continue;
		list<ServeClient>::const_iterator start_iter_cid, end_iter_cid;
		findClientIDServeList(solution.routeVec[rin].serveClientList,
			vr.orderVec[vr.orderMap[*iter]].getApplierID(), start_iter_cid);
		findClientIDServeList(solution.routeVec[rin].serveClientList,
			vr.orderVec[vr.orderMap[*iter]].getRequestID(), end_iter_cid);
		// insert MO optional order
		if (start_iter_cid != solution.routeVec[rin].serveClientList.end() &&
			end_iter_cid == solution.routeVec[rin].serveClientList.end())
		{

		}
	}
}

void findClientIDServeList(const list<ServeClient> &sc_list, const ClientID &cid, list<ServeClient>::const_iterator &iter_cid)
{
	for (iter_cid = sc_list.begin(); iter_cid != sc_list.end(); iter_cid++)
		if (iter_cid->visitClientID == cid)
			break;

}
// find the OrderID of the least cost order in servedOrderList/servedOrderIDSet with respect to init_cid
OrderID Solver::findLeastCostOrder(const list<OrderID> &servedOrderList, const ClientID &init_cid)
{
	DistanceType least_dis = INT16_MAX;
	OrderID least_oid;
	for (list<OrderID>::const_iterator iter = servedOrderList.begin(); iter != servedOrderList.end(); iter++)
	{
		if (vr.orderVec[vr.orderMap[*iter]].getOrderType() == OrderType::Optional)
			continue;
		DistanceType cur_dis = vr.edgeVec[vr.orderEdge[vr.clientMap[init_cid]][vr.clientMap[vr.orderVec[vr.orderMap[*iter]].getRequestID()]]].getDistance();
		if (least_dis > cur_dis)
		{
			least_dis = cur_dis;
			least_oid = *iter;
		}
	}
	return least_oid;
}
