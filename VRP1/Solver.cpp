#include"Solver.h"

Solver::Solver(const VehicleRouting &_vr) :vr(_vr)
{
	dsp = new DijkstraShortPath(vr.clientVec, vr.edgeVec, vr.clientNameMap);
	for (std::vector<Order>::const_iterator iter = vr.orderVec.begin(); 
		iter != vr.orderVec.end(); iter++)
	{
		if ((*iter).getOrderType() == OrderType::Mandatory)
			mandatoryOrderList.push_back(iter->getID());
		else
			optionalOrderList.push_back(iter->getID());
	}
}

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
	for (vector<int>::iterator iter = moiv.begin(); iter != moiv.end(); iter++)
	{
		int rd = rand() % solution.routeVec.size();
		if (solution.routeVec[rd].mandaQuantity + vr.orderVec[*iter].getQuantity() >
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
		cout << (*iter).VehID << ", " << (*iter).serveOrderList.size() << " : " << endl;;
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
		cout << *iter << "\t";
	}
	cout << solution.routeVec[rin].serveOrderList.size() << "\tclient id of mandatory order: \n";
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
		// NOTE: there exists a circle, at least one order 
		// should be served by other vehicles.
		// remove an order to other route
	}
	// arrange the mandatory and MM optional orders
	//cout << "arrange the order sequence: " << endl;
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
	MMOrderClientIDSet.erase(vr.clientVec[0].PriDCID);
	while (!MMOrderClientIDSet.empty())
	{
		ClientID sel_end_cid;
		DistanceType shortest_distance;
		vector<ClientID> shortest_cid_vec;
		dsp->getShortPathClientIDSet(init_cid, MMOrderClientIDSet, sel_end_cid, shortest_distance, shortest_cid_vec);
		MMOrderClientIDSet.erase(sel_end_cid);
		for (vector<ClientID>::iterator iter = ++shortest_cid_vec.begin(); iter != --shortest_cid_vec.end(); iter++)
		{
			// insert the client between init_cid and sel_end_cid
			ServeClient sc1(*iter, solution.routeVec[rin].serveClientList.back().currentQuantity);
			solution.routeVec[rin].serveClientList.push_back(sc1);
		}
		ServeClient sc1(sel_end_cid, solution.routeVec[rin].serveClientList.back().currentQuantity);
		for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
			iter != solution.routeVec[rin].serveOrderList.end();)
		{
			if (vr.orderVec[vr.orderMap.at(*iter)].getOrderType() == OrderType::Optional ||
				vr.orderVec[vr.orderMap.at(*iter)].getRequestID() != sel_end_cid)
			{
				iter++;
				continue;
			}
			sc1.unloadOrderID.push_back(*iter);
			sc1.currentQuantity -= vr.orderVec[vr.orderMap.at(*iter)].getQuantity();
			iter = solution.routeVec[rin].serveOrderList.erase(iter);
		}
		solution.routeVec[rin].serveClientList.push_back(sc1);
		init_cid = sel_end_cid;
	}
	// insert the last visiting client depot
	vector<ClientID> shortest_cid_vec;
	DistanceType shortest_distance;
	dsp->getShortPath(init_cid, vr.clientVec[0].PriDCID, shortest_distance, shortest_cid_vec);
	for (vector<ClientID>::iterator iter = ++shortest_cid_vec.begin(); iter != shortest_cid_vec.end(); iter++)
	{
		ServeClient sc2(*iter, 0);
		solution.routeVec[rin].serveClientList.push_back(sc2);
	}
	printRoute(rin);
	calculateObjValue(rin);
	checkRoute(rin);
	// repair the mandatory order to adjust the MM optional order
	for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
		iter != solution.routeVec[rin].serveOrderList.end();)
	{
		if (vr.orderVec[vr.orderMap[*iter]].getOrderType() == OrderType::Mandatory)
		{
			iter++;
			continue;
		}
		list<ServeClient>::iterator start_sc_iter, end_sc_iter;
		findClientIDServeList(solution.routeVec[rin].serveClientList,
			vr.orderVec[vr.orderMap[*iter]].getApplierID(), start_sc_iter);
		findClientIDServeList(solution.routeVec[rin].serveClientList,
			vr.orderVec[vr.orderMap[*iter]].getRequestID(), end_sc_iter);
		// insert MM optional order
		if (start_sc_iter != solution.routeVec[rin].serveClientList.end() &&
			end_sc_iter != solution.routeVec[rin].serveClientList.end())
		{
			insertMMOrderToRoute(rin, iter, start_sc_iter, end_sc_iter);
		}
		else
			iter++;
	}
	printRoute(rin);
	calculateObjValue(rin);
	checkRoute(rin);
	// insert the MO and OM optional order to the already arranged orders
	int serve_order_cnt = solution.routeVec[rin].serveOrderList.size() + 1;
	while (serve_order_cnt > solution.routeVec[rin].serveOrderList.size())
	{
		serve_order_cnt = solution.routeVec[rin].serveOrderList.size();
		for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
			iter != solution.routeVec[rin].serveOrderList.end();)
		{
			if (vr.orderVec[vr.orderMap[*iter]].getOrderType() == OrderType::Mandatory)
			{
				iter++;
				continue;
			}
			list<ServeClient>::iterator start_sc_iter, end_sc_iter;
			findClientIDServeList(solution.routeVec[rin].serveClientList,
				vr.orderVec[vr.orderMap[*iter]].getApplierID(), start_sc_iter);
			findClientIDServeList(solution.routeVec[rin].serveClientList,
				vr.orderVec[vr.orderMap[*iter]].getRequestID(), end_sc_iter);
			// insert MO optional order
			if (start_sc_iter != solution.routeVec[rin].serveClientList.end() &&
				end_sc_iter == solution.routeVec[rin].serveClientList.end())
			{
				insertMOOrderToRoute(rin, iter, start_sc_iter);
			}// insert OM optional order
			else if (start_sc_iter == solution.routeVec[rin].serveClientList.end() &&
				end_sc_iter != solution.routeVec[rin].serveClientList.end())
			{
				insertOMOrderToRoute(rin, iter, end_sc_iter);
			}
			else
				iter++;
		}
	}
	printRoute(rin);
	calculateObjValue(rin);
	checkRoute(rin);
	for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
		iter != solution.routeVec[rin].serveOrderList.end();)
	{
		if (vr.orderVec[vr.orderMap[*iter]].getOrderType() == OrderType::Mandatory)
		{
			iter++;
			continue;
		}
		list<ServeClient>::iterator start_sc_iter, end_sc_iter;
		findClientIDServeList(solution.routeVec[rin].serveClientList,
			vr.orderVec[vr.orderMap[*iter]].getApplierID(), start_sc_iter);
		findClientIDServeList(solution.routeVec[rin].serveClientList,
			vr.orderVec[vr.orderMap[*iter]].getRequestID(), end_sc_iter);
		// insert OO optional order
		if (start_sc_iter == solution.routeVec[rin].serveClientList.end() &&
			end_sc_iter == solution.routeVec[rin].serveClientList.end())
		{
			insertOOOrderToRoute(rin, iter);
		}
		else
			iter++;
	}
	printRoute(rin);
	calculateObjValue(rin);
	checkRoute(rin);
}

void Solver::generateRoute1(const int &rin)
{
	// find the optional MM order with the in degree of the initial vertex is 0, 
	// note that the in degree of all MM order may be larger than 0
	set<ClientID> MMOrderClientIDSet;	// the ClientID set of mandatory order
	for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
		iter != solution.routeVec[rin].serveOrderList.end(); iter++)
	{
		if (vr.orderVec[vr.orderMap[*iter]].getOrderType() == OrderType::Mandatory)
		{
			MMOrderClientIDSet.insert(vr.orderVec[vr.orderMap[*iter]].getApplierID());
			MMOrderClientIDSet.insert(vr.orderVec[vr.orderMap[*iter]].getRequestID());
		}
		cout << *iter << "\t";
	}
	cout << solution.routeVec[rin].serveOrderList.size() << "\tclient id of mandatory order: \n";
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
		// NOTE: there exists a circle, at least one order 
		// should be served by other vehicles.
		// remove an order to other route
	}
	// arrange the mandatory and MM optional orders
	//cout << "arrange the order sequence: " << endl;
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
	MMOrderClientIDSet.erase(vr.clientVec[0].PriDCID);
	while (!MMOrderClientIDSet.empty())
	{
		ClientID sel_end_cid;
		DistanceType shortest_distance;
		vector<ClientID> shortest_cid_vec;
		dsp->getShortPathClientIDSet(init_cid, MMOrderClientIDSet, sel_end_cid, shortest_distance, shortest_cid_vec);
		MMOrderClientIDSet.erase(sel_end_cid);
		for (vector<ClientID>::iterator iter = ++shortest_cid_vec.begin(); iter != --shortest_cid_vec.end(); iter++)
		{
			// insert the client between init_cid and sel_end_cid
			ServeClient sc1(*iter, solution.routeVec[rin].serveClientList.back().currentQuantity);
			solution.routeVec[rin].serveClientList.push_back(sc1);
		}
		ServeClient sc1(sel_end_cid, solution.routeVec[rin].serveClientList.back().currentQuantity);
		for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
			iter != solution.routeVec[rin].serveOrderList.end();)
		{
			if (vr.orderVec[vr.orderMap.at(*iter)].getOrderType() == OrderType::Optional ||
				vr.orderVec[vr.orderMap.at(*iter)].getRequestID() != sel_end_cid)
			{
				iter++;
				continue;
			}
			sc1.unloadOrderID.push_back(*iter);
			sc1.currentQuantity -= vr.orderVec[vr.orderMap.at(*iter)].getQuantity();
			iter = solution.routeVec[rin].serveOrderList.erase(iter);
		}
		solution.routeVec[rin].serveClientList.push_back(sc1);
		init_cid = sel_end_cid;
	}
	// insert the last visiting client depot
	vector<ClientID> shortest_cid_vec;
	DistanceType shortest_distance;
	dsp->getShortPath(init_cid, vr.clientVec[0].PriDCID, shortest_distance, shortest_cid_vec);
	for (vector<ClientID>::iterator iter = ++shortest_cid_vec.begin(); iter != shortest_cid_vec.end(); iter++)
	{
		ServeClient sc2(*iter, 0);
		solution.routeVec[rin].serveClientList.push_back(sc2);
	}
	printRoute(rin);
	calculateObjValue(rin);
	checkRoute(rin);
	// insert the mandatory optional order to the route
	for (list<OrderID>::iterator iter = mandatoryOrderList.begin();
		iter != mandatoryOrderList.end();)
	{
		if (vr.orderVec[vr.orderMap[*iter]].getOrderType() == OrderType::Mandatory)
		{
			iter++;
			continue;
		}
		list<ServeClient>::iterator start_sc_iter, end_sc_iter;
		findClientIDServeList(solution.routeVec[rin].serveClientList,
			vr.orderVec[vr.orderMap[*iter]].getApplierID(), start_sc_iter);
		findClientIDServeList(solution.routeVec[rin].serveClientList,
			vr.orderVec[vr.orderMap[*iter]].getRequestID(), end_sc_iter);
		// insert MM optional order
		if (start_sc_iter != solution.routeVec[rin].serveClientList.end() &&
			end_sc_iter != solution.routeVec[rin].serveClientList.end())
		{
			// only insert the pass
			insertMMOrderToRoute1(rin, iter, start_sc_iter, end_sc_iter);
		}
		else
			iter++;
	}
	printRoute(rin);
	calculateObjValue(rin);
	checkRoute(rin);
}

void Solver::insertMMOrderToRoute1(const int &rin, list<OrderID>::iterator &iter,
	list<ServeClient>::iterator &start_sc_iter, list<ServeClient>::iterator &end_sc_iter)
{
	ClientID &start_cid = vr.orderVec[vr.orderMap[*iter]].getApplierID();
	ClientID &end_cid = vr.orderVec[vr.orderMap[*iter]].getRequestID();
	list<ServeClient>::iterator sc_iter;
	for (sc_iter = solution.routeVec[rin].serveClientList.begin();
		sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
	{
		if (sc_iter->visitClientID == start_cid || sc_iter->visitClientID == end_cid)
			break;
	}
	if (sc_iter->visitClientID == start_cid)	// the order is along with the route
	{
		// if adding the MM optional order does not exceed the vehicle capacity, add sc_iter
		if (start_sc_iter->currentQuantity + vr.orderVec[vr.orderMap[*iter]].getQuantity() <=
			vr.vehicleVec[vr.vehicleMap[solution.routeVec[rin].VehID]].capacity)
		{
			start_sc_iter->loadOrderID.push_back(*iter);
			end_sc_iter->unloadOrderID.push_back(*iter);
			for (list<ServeClient>::iterator it1 = start_sc_iter; it1 != end_sc_iter; it1++)
			{
				it1->currentQuantity += vr.orderVec[vr.orderMap[*iter]].getQuantity();
			}
			// erase the inserted order id from mandatory order list
			iter = mandatoryOrderList.erase(iter);	
		}
		else
			iter++;
		return;
	}	
}
void Solver::insertMMOrderToRoute(const int &rin, list<OrderID>::iterator &iter, 
	list<ServeClient>::iterator &start_sc_iter, list<ServeClient>::iterator &end_sc_iter)
{
	ClientID &start_cid = vr.orderVec[vr.orderMap[*iter]].getApplierID();
	ClientID &end_cid = vr.orderVec[vr.orderMap[*iter]].getRequestID();
	list<ServeClient>::iterator sc_iter;
	for (sc_iter = solution.routeVec[rin].serveClientList.begin();
		sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
	{
		if (sc_iter->visitClientID == start_cid || sc_iter->visitClientID == end_cid)
			break;
	}
	if (sc_iter->visitClientID == start_cid)	// the order is along with the route
	{
		// if adding the MM optional order does not exceed the vehicle capacity, add sc_iter
		if (start_sc_iter->currentQuantity + vr.orderVec[vr.orderMap[*iter]].getQuantity() <=
			vr.vehicleVec[vr.vehicleMap[solution.routeVec[rin].VehID]].capacity)
		{
			start_sc_iter->loadOrderID.push_back(*iter);
			end_sc_iter->unloadOrderID.push_back(*iter);
			for (list<ServeClient>::iterator it1 = start_sc_iter; it1 != end_sc_iter; it1++)
			{
				it1->currentQuantity += vr.orderVec[vr.orderMap[*iter]].getQuantity();
			}
			iter=solution.routeVec[rin].serveOrderList.erase(iter);
		}
		else
			iter++;
		return;
	}
	// the MM optional order is opposite to the arranged list
	// NOTE: impose the MM optional order to add to the arranged list
	// no matter it can improve the profit or not
	QuantityType start_unload_quantity = 0, end_unload_quantity = 0;
	for (vector<OrderID>::iterator oid_iter = start_sc_iter->unloadOrderID.begin();
		oid_iter != start_sc_iter->unloadOrderID.end(); oid_iter++)
		start_unload_quantity += vr.orderVec[vr.orderMap.at(*oid_iter)].getQuantity();
	for (vector<OrderID>::iterator oid_iter = end_sc_iter->unloadOrderID.begin();
		oid_iter != end_sc_iter->unloadOrderID.end(); oid_iter++)
		end_unload_quantity += vr.orderVec[vr.orderMap.at(*oid_iter)].getQuantity();
	// if adding the reversed MM optional order does not exceed the vehicle capacity, swap sc_iter and add sc_iter
	if (end_sc_iter->currentQuantity + end_unload_quantity - start_unload_quantity + vr.orderVec[vr.orderMap[*iter]].getQuantity() <=
		vr.vehicleVec[vr.vehicleMap[solution.routeVec[rin].VehID]].capacity)
	{
		start_sc_iter->loadOrderID.push_back(*iter);
		end_sc_iter->unloadOrderID.push_back(*iter);
		QuantityType temp_init_q = start_sc_iter->currentQuantity;
		start_sc_iter->currentQuantity = end_sc_iter->currentQuantity + end_unload_quantity - start_unload_quantity + vr.orderVec[vr.orderMap[*iter]].getQuantity();
		end_sc_iter->currentQuantity = temp_init_q;
		sc_iter = end_sc_iter;
		for (sc_iter++; sc_iter != start_sc_iter; sc_iter++)
		{
			sc_iter->currentQuantity += (end_unload_quantity - start_unload_quantity + vr.orderVec[vr.orderMap[*iter]].getQuantity());
		}
		swap(*start_sc_iter, *end_sc_iter);
		iter = solution.routeVec[rin].serveOrderList.erase(iter);
	}
	else
		iter++;
}
void Solver::insertMOOrderToRoute(const int &rin, list<OrderID>::iterator &iter, list<ServeClient>::iterator &start_sc_iter)
{
	if (start_sc_iter->currentQuantity + vr.orderVec[vr.orderMap[*iter]].getQuantity() >
		vr.vehicleVec[vr.vehicleMap[solution.routeVec[rin].VehID]].capacity)
	{
		iter++;
		return;	// exceed the capacity of the vehicle
	}
	DistanceType min_extra_distance = DBL_MAX, out_distance, in_distance;
	vector<ClientID> min_out_shortest_cid_vec, min_in_shortest_cid_vec, out_shortest_cid_vec, in_shortest_cid_vec;
	list<ServeClient>::iterator min_start_sc_iter, min_end_sc_iter;	// min_start_sc_iter is directly in front of min_end_sc_iter
	for (list<ServeClient>::iterator sc_iter = start_sc_iter;
		sc_iter != --solution.routeVec[rin].serveClientList.end(); sc_iter++)
	{
		list<ServeClient>::iterator next_sc_iter = sc_iter;
		next_sc_iter++;
		dsp->getShortPath(sc_iter->visitClientID, vr.orderVec[vr.orderMap[*iter]].getRequestID(),
			out_distance, out_shortest_cid_vec);
		dsp->getShortPath(vr.orderVec[vr.orderMap[*iter]].getRequestID(), next_sc_iter->visitClientID,
			in_distance, in_shortest_cid_vec);
		if (in_distance + out_distance < min_extra_distance)
		{
			min_extra_distance = in_distance + out_distance;
			min_start_sc_iter = sc_iter;
			min_end_sc_iter = next_sc_iter;
			// resize is needed for std::copy
			min_out_shortest_cid_vec.resize(out_shortest_cid_vec.size());
			min_in_shortest_cid_vec.resize(in_shortest_cid_vec.size());
			std::copy(out_shortest_cid_vec.begin(), out_shortest_cid_vec.end(), min_out_shortest_cid_vec.begin());
			std::copy(in_shortest_cid_vec.begin(), in_shortest_cid_vec.end(), min_in_shortest_cid_vec.begin());
		}
	}
	if (min_extra_distance == DBL_MAX)	// does not exist extra route
	{
		iter++;
		return;
	}
	// the order is worth doing
	if (min_extra_distance < vr.orderVec[vr.orderMap[*iter]].getOrderValue())
	{
		// load this MO optional order iter in start_sc_iter
		start_sc_iter->loadOrderID.push_back(*iter);
		// from start_sc_iter to min_start_sc_iter, add the weight of this MO optional order iter
		for (list<ServeClient>::iterator sc_iter = start_sc_iter; sc_iter != min_start_sc_iter; sc_iter++)
		{
			sc_iter->currentQuantity += vr.orderVec[vr.orderMap[*iter]].getQuantity();
		}
		min_start_sc_iter->currentQuantity += vr.orderVec[vr.orderMap[*iter]].getQuantity();
		// from min_start_sc_iter to iter->requestID, add new ServeClient to list
		// NOTE: the new ServeClient to be added may be already existed in the list,
		// this circumstance should be carefully treated.
		for (vector<ClientID>::iterator cid_iter = min_out_shortest_cid_vec.begin();
			cid_iter != min_out_shortest_cid_vec.end(); cid_iter++)
		{
			if (*cid_iter == min_out_shortest_cid_vec.front())
				continue;
			list<ServeClient>::iterator prior_min_end_sc_iter = min_end_sc_iter;
			prior_min_end_sc_iter--;	// prior of min_end_sc_iter
			ServeClient sc1(*cid_iter, prior_min_end_sc_iter->currentQuantity);
			if (*cid_iter == min_out_shortest_cid_vec.back())
			{
				sc1.unloadOrderID.push_back(*iter);
				sc1.currentQuantity -= vr.orderVec[vr.orderMap[*iter]].getQuantity();
			}
			// add sc1 to the front of min_end_sc_iter
			solution.routeVec[rin].serveClientList.insert(min_end_sc_iter, sc1);
		}
		// from iter->requestID to min_end_sc_iter, add new ServeClient to list
		for (vector<ClientID>::iterator cid_iter = min_in_shortest_cid_vec.begin();
			cid_iter != min_in_shortest_cid_vec.end(); cid_iter++)
		{
			if (*cid_iter == min_in_shortest_cid_vec.front() ||
				*cid_iter == min_in_shortest_cid_vec.back())
				continue;
			list<ServeClient>::iterator prior_min_end_sc_iter = min_end_sc_iter;
			prior_min_end_sc_iter--;	// prior of min_end_sc_iter
			ServeClient sc1(*cid_iter, prior_min_end_sc_iter->currentQuantity);
			solution.routeVec[rin].serveClientList.insert(min_end_sc_iter, sc1);
		}
		iter = solution.routeVec[rin].serveOrderList.erase(iter);
	}
}
void Solver::insertOMOrderToRoute(const int &rin, list<OrderID>::iterator &iter, list<ServeClient>::iterator &end_sc_iter)
{
	DistanceType min_extra_distance = DBL_MAX, out_distance, in_distance;
	vector<ClientID> min_out_shortest_cid_vec, min_in_shortest_cid_vec, out_shortest_cid_vec, in_shortest_cid_vec;
	list<ServeClient>::iterator min_start_sc_iter, min_end_sc_iter;	// min_start_sc_iter is directly in front of min_end_sc_iter
	for (list<ServeClient>::iterator sc_iter = solution.routeVec[rin].serveClientList.begin();
		sc_iter != end_sc_iter; sc_iter++)
	{
		list<ServeClient>::iterator next_sc_iter = sc_iter;
		next_sc_iter++;
		if (sc_iter->currentQuantity + vr.orderVec[vr.orderMap.at(*iter)].getQuantity() >
			vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].capacity)
			continue;	// exceed the capacity of the vehicle
		dsp->getShortPath(sc_iter->visitClientID, vr.orderVec[vr.orderMap.at(*iter)].getApplierID(), out_distance, out_shortest_cid_vec);
		dsp->getShortPath(vr.orderVec[vr.orderMap.at(*iter)].getApplierID(), next_sc_iter->visitClientID, in_distance, in_shortest_cid_vec);
		//NOTE: if there is client that already exists in the route, skip it
		if (checkClientInRoute(rin, out_shortest_cid_vec) || checkClientInRoute(rin, in_shortest_cid_vec))
			continue;
		if (in_distance + out_distance < min_extra_distance)
		{
			min_extra_distance = in_distance + out_distance;
			min_start_sc_iter = sc_iter;
			min_end_sc_iter = next_sc_iter;
			// resize is needed for std::copy
			min_out_shortest_cid_vec.resize(out_shortest_cid_vec.size());
			min_in_shortest_cid_vec.resize(in_shortest_cid_vec.size());
			std::copy(out_shortest_cid_vec.begin(), out_shortest_cid_vec.end(), min_out_shortest_cid_vec.begin());
			std::copy(in_shortest_cid_vec.begin(), in_shortest_cid_vec.end(), min_in_shortest_cid_vec.begin());
		}
	}
	// does not exist extra route or the order is not worth doing, skip it
	if (min_extra_distance == DBL_MAX ||
		min_extra_distance >= vr.orderVec[vr.orderMap.at(*iter)].getOrderValue())
	{
		iter++;
		return;
	}
	// frome min_start_sc_iter to iter->applierID, add new ServeClient to list
	// NOTE: the new ServeClient to be added may be already existed in the list,
	// this circumstance should be carefully treated.
	for (vector<ClientID>::iterator cid_iter = ++min_out_shortest_cid_vec.begin();
		cid_iter != min_out_shortest_cid_vec.end(); cid_iter++)
	{
		// skip the first element of the vector in for
		list<ServeClient>::iterator prior_min_end_sc_iter = min_end_sc_iter;
		prior_min_end_sc_iter--;	// prior of min_end_sc_iter
		ServeClient sc1(*cid_iter, prior_min_end_sc_iter->currentQuantity);
		if (cid_iter == --min_out_shortest_cid_vec.end())
		{
			sc1.loadOrderID.push_back(*iter);
			sc1.currentQuantity += vr.orderVec[vr.orderMap.at(*iter)].getQuantity();
		}
		// add sc1 to the front of min_end_sc_iter
		solution.routeVec[rin].serveClientList.insert(min_end_sc_iter, sc1);
	}
	// from iter->applierID to min_end_sc_iter, add new ServeClient to list
	for (vector<ClientID>::iterator cid_iter = ++min_in_shortest_cid_vec.begin();
		cid_iter != --min_in_shortest_cid_vec.end(); cid_iter++)
	{
		// skip the first and last element of the vector in for
		list<ServeClient>::iterator prior_min_end_sc_iter = min_end_sc_iter;
		prior_min_end_sc_iter--;
		ServeClient sc1(*cid_iter, prior_min_end_sc_iter->currentQuantity);
		solution.routeVec[rin].serveClientList.insert(min_end_sc_iter, sc1);
	}
	// from min_end_sc_iter to end_sc_iter, add the weight of the OM optional order iter
	for (list<ServeClient>::iterator sc_iter = min_end_sc_iter; sc_iter != end_sc_iter; sc_iter++)
	{
		sc_iter->currentQuantity += vr.orderVec[vr.orderMap.at(*iter)].getQuantity();
	}
	end_sc_iter->unloadOrderID.push_back(*iter);
	iter = solution.routeVec[rin].serveOrderList.erase(iter);
}
void Solver::insertOOOrderToRoute(const int &rin, list<OrderID>::iterator &iter)
{
	ClientID &start_cid = vr.orderVec[vr.orderMap.at(*iter)].getApplierID();
	ClientID &end_cid = vr.orderVec[vr.orderMap.at(*iter)].getRequestID();
	set<ClientID> route_cid_set;
	for (list<ServeClient>::iterator sc_iter = solution.routeVec[rin].serveClientList.begin();
		sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
	{
		if (sc_iter->currentQuantity + vr.orderVec[vr.orderMap.at(*iter)].getQuantity()<=
			vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].capacity)
			route_cid_set.insert(sc_iter->visitClientID);
	}
	if (route_cid_set.size() == 0)	// exceed the capacity for all clients
	{
		iter++;
		return;
	}
	ClientID sel_start_cid, sel_end_cid;
	DistanceType shortest_start_distance, shortest_end_distance, shortest_start_back_distance, shortest_end_back_distance, shortest_order_distance;
	vector<ClientID> shortest_start_cid_vec, shortest_end_cid_vec, shortest_start_back_cid_vec, shortest_end_back_cid_vec, shortest_order_cid_vec;
	dsp->getShortPathClientIDSet(start_cid, route_cid_set, sel_start_cid, shortest_start_distance, shortest_start_cid_vec);
	std::reverse(shortest_start_cid_vec.begin(), shortest_start_cid_vec.end());
	list<ServeClient>::iterator start_sc_iter, start_next_sc_iter, end_sc_iter, end_next_sc_iter;
	route_cid_set.clear();
	bool is_behind_cid_flag = false;
	for (list<ServeClient>::iterator sc_iter = solution.routeVec[rin].serveClientList.begin();
		sc_iter != --solution.routeVec[rin].serveClientList.end(); sc_iter++)
	{
		if (sc_iter->visitClientID == sel_start_cid)
		{
			start_sc_iter = start_next_sc_iter = sc_iter;
			start_next_sc_iter++;
			is_behind_cid_flag = true;
		}
		if (is_behind_cid_flag)
			route_cid_set.insert(sc_iter->visitClientID);
	}
	route_cid_set.erase(sel_start_cid);	// erase the sel_start_cid
	route_cid_set.erase(solution.routeVec[rin].serveClientList.back().visitClientID);	// erase the last client c0
	dsp->getShortPathClientIDSet(end_cid, route_cid_set, sel_end_cid, shortest_end_distance, shortest_end_cid_vec);
	std::reverse(shortest_end_cid_vec.begin(), shortest_end_cid_vec.end());
	for (list<ServeClient>::iterator sc_iter = solution.routeVec[rin].serveClientList.begin();
		sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
	{
		if (sc_iter->visitClientID == sel_end_cid)
		{
			end_sc_iter = end_next_sc_iter = sc_iter;
			end_next_sc_iter++;
			break;
		}
	}
	dsp->getShortPath(start_cid, start_next_sc_iter->visitClientID, shortest_start_back_distance, shortest_start_back_cid_vec);
	dsp->getShortPath(end_cid, end_next_sc_iter->visitClientID, shortest_end_back_distance, shortest_end_back_cid_vec);
	// if the order is not worth doing
	if (shortest_end_distance + shortest_end_back_distance + shortest_start_distance + shortest_start_back_distance >=
		vr.orderVec[vr.orderMap.at(*iter)].getOrderValue())
	{
		iter++;
		return;
	}
	if (end_sc_iter == start_next_sc_iter)	// triangle route
	{
		dsp->getShortPath(start_cid, end_cid, shortest_order_distance, shortest_order_cid_vec);
		// from start_sc_iter to start_cid, add new ServeClient to list
		for (vector<ClientID>::iterator cid_iter = ++shortest_start_cid_vec.begin();
			cid_iter != shortest_start_cid_vec.end(); cid_iter++)
		{
			start_next_sc_iter--;
			ServeClient sc(*cid_iter, start_next_sc_iter->currentQuantity);
			start_next_sc_iter++;
			if (cid_iter == --shortest_start_cid_vec.end())
			{
				// add the start_cid
				sc.loadOrderID.push_back(*iter);
				sc.currentQuantity += vr.orderVec[vr.orderMap.at(*iter)].getQuantity();
			}
			solution.routeVec[rin].serveClientList.insert(start_next_sc_iter, sc);
		}
		// from start_cid to end_cid, add new ServeClient to list
		for (vector<ClientID>::iterator cid_iter = ++shortest_order_cid_vec.begin();
			cid_iter != shortest_order_cid_vec.end(); cid_iter++)
		{
			start_next_sc_iter--;
			ServeClient sc(*cid_iter, start_next_sc_iter->currentQuantity);
			start_next_sc_iter++;
			if (cid_iter == --shortest_order_cid_vec.end())
			{
				sc.unloadOrderID.push_back(*iter);
				sc.currentQuantity -= vr.orderVec[vr.orderMap.at(*iter)].getQuantity();
			}
			solution.routeVec[rin].serveClientList.insert(start_next_sc_iter, sc);
		}
		// from end_cid to end_sc_iter
		for (vector<ClientID>::iterator cid_iter = ++shortest_end_cid_vec.begin();
			cid_iter != --shortest_end_cid_vec.end(); cid_iter++)
		{
			start_next_sc_iter--;
			ServeClient sc(*cid_iter, start_next_sc_iter->currentQuantity);
			start_next_sc_iter++;
			solution.routeVec[rin].serveClientList.insert(start_next_sc_iter, sc);
		}
	}
	else
	{	// rectangle route
		// from start_sc_iter to start_cid, add new ServeClient to list
		for (vector<ClientID>::iterator cid_iter = ++shortest_start_cid_vec.begin();
			cid_iter != shortest_start_cid_vec.end(); cid_iter++)
		{
			start_next_sc_iter--;
			ServeClient sc(*cid_iter, start_next_sc_iter->currentQuantity);
			start_next_sc_iter++;
			if (cid_iter == --shortest_start_cid_vec.end())
			{
				// add the start_cid
				sc.loadOrderID.push_back(*iter);
				sc.currentQuantity += vr.orderVec[vr.orderMap.at(*iter)].getQuantity();
			}
			solution.routeVec[rin].serveClientList.insert(start_next_sc_iter, sc);
		}
		// from start_cid to start_next_sc_iter, add new ServeClient to list
		for (vector<ClientID>::iterator cid_iter = ++shortest_start_back_cid_vec.begin();
			cid_iter != --shortest_start_back_cid_vec.end(); cid_iter++)
		{
			start_next_sc_iter--;
			ServeClient sc(*cid_iter, start_next_sc_iter->currentQuantity);
			start_next_sc_iter++;
			solution.routeVec[rin].serveClientList.insert(start_next_sc_iter, sc);
		}
		// from start_next_sc_iter to end_sc_iter, add quantity to existing clients
		for (list<ServeClient>::iterator sc_iter = start_next_sc_iter; sc_iter != end_sc_iter; sc_iter++)
		{
			sc_iter->currentQuantity += vr.orderVec[vr.orderMap.at(*iter)].getQuantity();
		}
		end_sc_iter->currentQuantity += vr.orderVec[vr.orderMap.at(*iter)].getQuantity();
		// from end_sc_iter to end_cid, add new ServeClient to list
		for (vector<ClientID>::iterator cid_iter = ++shortest_end_cid_vec.begin();
			cid_iter != shortest_end_cid_vec.end(); cid_iter++)
		{
			end_next_sc_iter--;
			ServeClient sc(*cid_iter, end_next_sc_iter->currentQuantity);
			end_next_sc_iter++;
			if (cid_iter == --shortest_end_cid_vec.end())
			{
				sc.unloadOrderID.push_back(*iter);
				sc.currentQuantity -= vr.orderVec[vr.orderMap[*iter]].getQuantity();
			}
			solution.routeVec[rin].serveClientList.insert(end_next_sc_iter, sc);
		}
		// from end_cid to end_next_sc_iter, add new ServeClient to list
		for (vector<ClientID>::iterator cid_iter = ++shortest_end_back_cid_vec.begin();
			cid_iter != --shortest_end_back_cid_vec.end(); cid_iter++)
		{
			end_next_sc_iter--;
			ServeClient sc(*cid_iter, end_next_sc_iter->currentQuantity);
			end_next_sc_iter++;
			solution.routeVec[rin].serveClientList.insert(end_next_sc_iter, sc);
		}
	}
	iter = solution.routeVec[rin].serveOrderList.erase(iter);
}
void Solver::findClientIDServeList(list<ServeClient> &sc_list, const ClientID &cid, list<ServeClient>::iterator &iter_cid)
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
		DistanceType cur_dis = vr.edgeVec[vr.orderEdge[vr.clientMap[init_cid]]
			[vr.clientMap[vr.orderVec[vr.orderMap[*iter]].getRequestID()]]].getDistance();
		if (least_dis > cur_dis)
		{
			least_dis = cur_dis;
			least_oid = *iter;
		}
	}
	return least_oid;
}
void Solver::printRoute(const int &rin)const
{
	cout << endl << "The information of route " << rin << " :" << endl;
	cout << "it visits " << solution.routeVec[rin].serveClientList.size() << " clients: " << endl;
	for (list<ServeClient>::const_iterator iter = solution.routeVec[rin].serveClientList.begin();
		iter != solution.routeVec[rin].serveClientList.end(); iter++)
	{
		//cout << iter->visitClientID;
		cout << vr.clientVec[vr.clientMap.at(iter->visitClientID)].clientName;
		if (iter != --solution.routeVec[rin].serveClientList.end())
			cout << " -> ";
	}
	cout << endl << "The detail information of each client:" << endl;
	for (list<ServeClient>::const_iterator iter = solution.routeVec[rin].serveClientList.begin();
		iter != solution.routeVec[rin].serveClientList.end(); iter++)
	{
		cout << vr.clientVec[vr.clientMap.at(iter->visitClientID)].clientName 
			<<"("<< iter->visitClientID 
			<< "), quantity: " << (*iter).currentQuantity
			<< ", load:" << (*iter).loadOrderID.size() << ": ";
		for (vector<OrderID>::const_iterator it = (*iter).loadOrderID.begin();
			it != (*iter).loadOrderID.end(); it++)
		{
			//cout << *it << " ";
			cout << *it << " (" << vr.orderVec[vr.orderMap.at(*it)] << ") ";
		}
		cout << "; unload: " << (*iter).unloadOrderID.size() << ":";
		for (vector<OrderID>::const_iterator it = (*iter).unloadOrderID.begin();
			it != (*iter).unloadOrderID.end(); it++)
		{
			cout << *it << " ";
		}
		cout << endl;
	}
	cout << "\nThe left orders:" << solution.routeVec[rin].serveOrderList.size() << endl;
	for (list<OrderID>::const_iterator iter = solution.routeVec[rin].serveOrderList.begin();
		iter != solution.routeVec[rin].serveOrderList.end(); iter++)
		cout << vr.orderVec[vr.orderMap.at(*iter)] << endl;
}
bool Solver::checkRoute(const int &rin)const
{
	set<ClientID> visit_cid_set;
	for (list<ServeClient>::const_iterator iter = solution.routeVec[rin].serveClientList.begin();
		iter != solution.routeVec[rin].serveClientList.end(); iter++)
	{
		visit_cid_set.insert(iter->visitClientID);
		QuantityType real_cur_quantity;
		if (iter == solution.routeVec[rin].serveClientList.begin())
			real_cur_quantity = 0;
		else
		{
			iter--;
			real_cur_quantity = iter++->currentQuantity;
		}
		//cout << iter->visitClientID << ", quantity: " << (*iter).currentQuantity
		//	<< ", load:" << (*iter).loadOrderID.size() << ": ";
		for (vector<OrderID>::const_iterator it = (*iter).loadOrderID.begin();
			it != (*iter).loadOrderID.end(); it++)
		{
			if (vr.orderVec[vr.orderMap.at(*it)].getApplierID() != iter->visitClientID)
			{
				cout << "Check route" << rin << " " << iter->visitClientID << " load WRONG: "
					<< *it << " (" << vr.orderVec[vr.orderMap.at(*it)] << ")\n";
				return false;
			}
			real_cur_quantity += vr.orderVec[vr.orderMap.at(*it)].getQuantity();
		}
		//cout << "; unload: " << (*iter).unloadOrderID.size() << ":";
		for (vector<OrderID>::const_iterator it = (*iter).unloadOrderID.begin();
			it != (*iter).unloadOrderID.end(); it++)
		{
			if (vr.orderVec[vr.orderMap.at(*it)].getRequestID()!=iter->visitClientID)
			{
				cout << "Check route " << rin << " " << iter->visitClientID << " unload WRONG : "
					<< *it << " (" << vr.orderVec[vr.orderMap.at(*it)] << ")\n";
				return false;
			}
			real_cur_quantity -= vr.orderVec[vr.orderMap.at(*it)].getQuantity();
		}
		if (real_cur_quantity != iter->currentQuantity)
		{
			cout << real_cur_quantity << "!=" << iter->currentQuantity << endl;
			return false;
		}
		if (real_cur_quantity >vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].capacity)
		{
			cout << "Check route " << rin << " " << iter->visitClientID << " unload WRONG : "
				<< real_cur_quantity << " exceeds capacity" 
				<< vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].capacity << endl;
			return false;
		}
	}
	/*if (visit_cid_set.size() != solution.routeVec[rin].serveClientList.size() - 1)
	{
		cout << "Check route " << rin << " is WRONG, multiple client" << endl;
		return false;
	}*/
	cout << endl << "Check route " << rin << " is RIGHT" << endl;
	return true;
}

bool Solver::checkClientInRoute(const int &rin,const vector<ClientID> &cid_vec)const
{
	for (vector<ClientID>::const_iterator cid_iter = ++cid_vec.begin(); 
		cid_iter != --cid_vec.end(); cid_iter++)
	{
		for (list<ServeClient>::const_iterator sc_iter = solution.routeVec[rin].serveClientList.begin();
			sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
		{
			if (*cid_iter == sc_iter->visitClientID)
				return true;
		}
	}
	return false;
}
void Solver::calculateObjValue(const int &rin)
{
	solution.routeVec[rin].routeObject = 0;
	solution.routeVec[rin].routeDistance = 0;
	solution.routeVec[rin].routeWeightDistance = 0;
	for (list<ServeClient>::const_iterator sc_iter = solution.routeVec[rin].serveClientList.begin();
		sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
	{
		for (vector<OrderID>::const_iterator oid_unload_iter = sc_iter->unloadOrderID.begin();
			oid_unload_iter != sc_iter->unloadOrderID.end(); oid_unload_iter++)
			solution.routeVec[rin].routeObject += vr.orderVec[vr.orderMap.at(*oid_unload_iter)].getOrderValue();
		if (sc_iter != solution.routeVec[rin].serveClientList.begin())
		{
			list<ServeClient>::const_iterator prior_iter = sc_iter;
			prior_iter--;
			DistanceType temp_dis = vr.edgeVec[vr.orderEdge[vr.clientMap.at(prior_iter->visitClientID)]
				[vr.clientMap.at(sc_iter->visitClientID)]].getDistance();
			solution.routeVec[rin].routeDistance += temp_dis;
			solution.routeVec[rin].routeWeightDistance += prior_iter->currentQuantity * temp_dis;
		}
	}
	solution.routeVec[rin].fullLoadRate = solution.routeVec[rin].routeWeightDistance / (solution.routeVec[rin].routeDistance *
		vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].capacity);
	cout << "route " << rin << " obj: "
		<< solution.routeVec[rin].routeObject << "\t" << solution.routeVec[rin].routeDistance << "\t"
		<< solution.routeVec[rin].routeWeightDistance << "\t" << solution.routeVec[rin].fullLoadRate << endl;
}