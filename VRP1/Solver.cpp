#include"Solver.h"

Solver::Solver(const VehicleRouting &_vr, ostream &_os) :vr(_vr), os(_os)
{
	dsp = new DijkstraShortPath(vr.clientVec, vr.edgeVec, vr.clientNameMap);
}

bool Solver::initSolution(const int &sol_num)
{
	// try to find the least number of vehicle required to complete the delivery
	
	for (int least_num_vehi = 1; least_num_vehi <= vr.vehicleVec.size(); least_num_vehi++)
	{
		Solution solution(0, 0, 0, 0, 0, 0);
		for (std::vector<Order>::const_iterator iter = vr.orderVec.begin();
			iter != vr.orderVec.end(); iter++)
		{
			if ((*iter).getOrderType() == OrderType::Mandatory)
				solution.mandatoryOrderIDSet.insert(iter->getID());
			else
				solution.optionalOrderIDSet.insert(iter->getID());
		}
		for (int i = 0; i < least_num_vehi;i++)
		{
			Route r;
			r.VehID = vr.vehicleVec[i].VehID;
			r.mandaQuantity = 0;
			solution.routeVec.push_back(r);
		}
		// assign mandatory order to each route, randomly
		for (vector<Order>::const_iterator iter = vr.orderVec.begin();
			iter != vr.orderVec.end(); iter++)
		{
			if (iter->getOrderType() == OrderType::Optional)
				continue;
			int rd = rand() % solution.routeVec.size();

			solution.routeVec[rd].mandaQuantity += iter->getQuantity();
			solution.routeVec[rd].serveOrderList.push_back(iter->getID());
		}
		// arrrange mandatory order
		for (int rin = 0; rin < solution.routeVec.size(); rin++)
		{
			arrangeMandatoryOrder(solution, rin);
			//checkRoute(solution, rin);
		}
		for (int rin = 0; rin < solution.routeVec.size(); rin++)
		{
			for (list<OrderID>::iterator oid_iter = solution.routeVec[rin].serveOrderList.begin();
				oid_iter != solution.routeVec[rin].serveOrderList.end();)
			{
				bool is_inserted = false;
				for (int rin_op = 0; rin_op < solution.routeVec.size(); rin_op++)
				{
					if (rin_op == rin)
						continue;
					if (insertMandatoryOrder(solution, rin_op, *oid_iter))
					{
						//checkRoute(solution, rin_op);
						//calculateObjValue(solution, rin_op);
						oid_iter = solution.routeVec[rin].serveOrderList.erase(oid_iter);
						is_inserted = true;
						break;
					}
				}
				if (!is_inserted)
					oid_iter++;
			}

		}
		int arranged_mand_order_cnt = 0;
		for (int rin = 0; rin < solution.routeVec.size(); rin++)
		{
			arranged_mand_order_cnt += solution.routeVec[rin].serveClientList.front().loadOrderID.size();
		}
		if (arranged_mand_order_cnt == vr.getNumMandaOrder() || least_num_vehi == vr.vehicleVec.size())
		{
			for (int rin = 0; rin < solution.routeVec.size(); rin++)
			{
				arrangeOptionalOrder(solution, rin);
				calculateObjValue(solution, rin);
				printRoute(solution, rin);
				//checkRoute(solution, rin);
			}
			checkSolution(solution);
			calculateTotalObjValue(solution);
			solutionVec.push_back(solution);
			Solution solution1(0, 0, 0, 0, 0, 0);
			solutionVec.push_back(solution1);
			if (arranged_mand_order_cnt == vr.getNumMandaOrder())
				return true;
			else
				return false;
		}
	}
}
void Solver::localSearch(const int &sol_num,const int &iteration)
{
	os <<"STARTING SCHEDULING, num of mandatory order: "<< vr.getNumMandaOrder()
		<<", num of optional order: "<< vr.getNumOptionalOrder() << endl;
	bool manage_all_mandatory_order = initSolution(sol_num);
	
	int best_sol_index = 0, current_sol_index = 1;
	solutionVec[current_sol_index] = solutionVec[best_sol_index];

	bool need_local_search_iterate = true;
	if (solutionVec[current_sol_index].routeVec.size() == 1)
		need_local_search_iterate = false;
	int more_than_two_route_cnt = 0;
	for (vector < Route > ::iterator r_iter = solutionVec[current_sol_index].routeVec.begin();
		r_iter != solutionVec[current_sol_index].routeVec.end(); r_iter++)
	{
		if (r_iter->serveClientList.size() > 1)
			more_than_two_route_cnt += 1;
	}
	if (more_than_two_route_cnt <= 1)
		need_local_search_iterate = false;
	if (need_local_search_iterate)
	{
		for (int i = 0; i < iteration; i++)
		{
			int rin1, rin2;
			makeMove(solutionVec[current_sol_index], rin1, rin2);
			for (int rin = 0; rin < solutionVec[current_sol_index].routeVec.size(); rin++)
			{
				calculateObjValue(solutionVec[current_sol_index], rin);
				//printRoute(solutionVec[best_sol_index], rin);
				//checkRoute(solutionVec[current_sol_index], rin);
				/*if (solutionVec[current_sol_index].routeVec[rin].serveOrderList.size() > 0)
				{
					system("pause");
				}*/
			}

			calculateTotalObjValue(solutionVec[current_sol_index]);

			/*checkSolution(solutionVec[best_sol_index]);
			checkSolution(solutionVec[current_sol_index]);*/

			if (solutionVec[current_sol_index].averagefullLoadRate >
				solutionVec[best_sol_index].averagefullLoadRate)
			{
				solutionVec[best_sol_index] = solutionVec[current_sol_index];
				os << "refine " << solutionVec[best_sol_index].serveMandaOrderCnt << endl;
			}
			else
			{
				solutionVec[current_sol_index] = solutionVec[best_sol_index];
				os << "not improved " << solutionVec[best_sol_index].serveMandaOrderCnt << endl;
			}
			os << "iteration "<<i<<", solution " << best_sol_index << ", total information:" << vr.getNumMandaOrder() << "\t" << vr.getNumOptionalOrder() << endl;
			os << solutionVec[best_sol_index].totalDistance << "\t" << solutionVec[best_sol_index].totalWeightDistance << "\t"
				<< solutionVec[best_sol_index].totalObject << "\t" << solutionVec[best_sol_index].averagefullLoadRate << "\t"
				<< solutionVec[best_sol_index].serveMandaOrderCnt << "\t" << solutionVec[best_sol_index].servOptionalOrderCnt << "\t"
				<< solutionVec[best_sol_index].serveMandaOrderCnt + solutionVec[best_sol_index].servOptionalOrderCnt << "\t"
				<< solutionVec[best_sol_index].mandatoryOrderIDSet.size() << endl;
			for (int rin = 0; rin < solutionVec[best_sol_index].routeVec.size(); rin++)
			{
				/*calculateObjValue(solutionVec[best_sol_index], rin);
				printRoute(solutionVec[best_sol_index], rin);
				checkRoute(solutionVec[best_sol_index], rin);*/
				
			}
			//checkSolution(solutionVec[best_sol_index]);
			//checkSolution(solutionVec[current_sol_index]);
		}	
	}
	for (int rin = 0; rin < solutionVec[best_sol_index].routeVec.size(); rin++)
	{
		calculateObjValue(solutionVec[best_sol_index], rin);
		printRoute(solutionVec[best_sol_index], rin);
		//checkRoute(solutionVec[best_sol_index], rin);
	}
	checkSolution(solutionVec[best_sol_index]);
}
void Solver::makeMove(Solution &solution,int &rin1,int &rin2)
{
	rin1 = rand() % solution.routeVec.size();
	while (solution.routeVec[rin1].serveClientList.size() == 1)
		rin1 = rand() % solution.routeVec.size();
	rin2 = rand() % solution.routeVec.size();
	while (solution.routeVec[rin2].serveClientList.size() == 1 || rin1 == rin2)
		rin2 = rand() % solution.routeVec.size();

	solution.routeVec[rin1].serveOrderList.clear();
	solution.routeVec[rin2].serveOrderList.clear();
	for (vector<OrderID>::iterator oid_iter = solution.routeVec[rin1].serveClientList.front().loadOrderID.begin();
		oid_iter != solution.routeVec[rin1].serveClientList.front().loadOrderID.end(); oid_iter++)
		solution.routeVec[rin1].serveOrderList.push_back(*oid_iter);
	for (vector<OrderID>::iterator oid_iter = solution.routeVec[rin2].serveClientList.front().loadOrderID.begin();
		oid_iter != solution.routeVec[rin2].serveClientList.front().loadOrderID.end(); oid_iter++)
		solution.routeVec[rin2].serveOrderList.push_back(*oid_iter);
	int oin1, oin2;
	oin1 = rand()% solution.routeVec[rin1].serveOrderList.size();
	oin2 = rand() % solution.routeVec[rin2].serveOrderList.size();
	list<OrderID>::iterator oid_iter1, oid_iter2;
	int ind_pos;
	for (oid_iter1 = solution.routeVec[rin1].serveOrderList.begin(), ind_pos = 0;
		oid_iter1 != solution.routeVec[rin1].serveOrderList.end() && ind_pos != oin1; oid_iter1++, ind_pos++){
	}
	for (oid_iter2 = solution.routeVec[rin2].serveOrderList.begin(), ind_pos = 0;
		oid_iter2 != solution.routeVec[rin2].serveOrderList.end() && ind_pos != oin2; oid_iter2++, ind_pos++){
	}	
	solution.routeVec[rin1].serveOrderList.push_back(*oid_iter2);
	solution.routeVec[rin2].serveOrderList.push_back(*oid_iter1);
	solution.routeVec[rin1].serveOrderList.remove(*oid_iter1);
	solution.routeVec[rin2].serveOrderList.remove(*oid_iter2);
	// remove the arranged optional order
	for (list<ServeClient>::iterator sc_iter = solution.routeVec[rin1].serveClientList.begin();
		sc_iter != solution.routeVec[rin1].serveClientList.end(); sc_iter++)
	{
		for (vector<OrderID>::iterator oid_iter = sc_iter->loadOrderID.begin(); oid_iter != sc_iter->loadOrderID.end(); oid_iter++)
		{
			if (vr.orderVec[vr.orderMap.at(*oid_iter)].getOrderType() == OrderType::Optional)
				solution.optionalOrderIDSet.insert(*oid_iter);
		}
	}
	for (list<ServeClient>::iterator sc_iter = solution.routeVec[rin2].serveClientList.begin();
	sc_iter != solution.routeVec[rin2].serveClientList.end(); sc_iter++)
	{
		for (vector<OrderID>::iterator oid_iter = sc_iter->loadOrderID.begin(); oid_iter != sc_iter->loadOrderID.end(); oid_iter++)
		{
			if (vr.orderVec[vr.orderMap.at(*oid_iter)].getOrderType() == OrderType::Optional)
				solution.optionalOrderIDSet.insert(*oid_iter);
		}
	}
	arrangeMandatoryOrder(solution, rin1); 
	arrangeMandatoryOrder(solution, rin2);
	for (list<OrderID>::iterator oid_iter = solution.routeVec[rin1].serveOrderList.begin();
		oid_iter != solution.routeVec[rin1].serveOrderList.end();)
	{
		bool is_inserted = false;
		for (int rin_op = 0; rin_op < solution.routeVec.size(); rin_op++)
		{
			if (rin_op == rin1)
				continue;
			if (insertMandatoryOrder(solution, rin_op, *oid_iter))
			{
				//checkRoute(solution, rin_op);
				oid_iter = solution.routeVec[rin1].serveOrderList.erase(oid_iter);
				is_inserted = true;
				break;
			}
		}
		if (!is_inserted)
			oid_iter++;
	}
	for (list<OrderID>::iterator oid_iter = solution.routeVec[rin2].serveOrderList.begin();
		oid_iter != solution.routeVec[rin2].serveOrderList.end();)
	{
		bool is_inserted = false;
		for (int rin_op = 0; rin_op < solution.routeVec.size(); rin_op++)
		{
			if (rin_op == rin2)
				continue;
			if (insertMandatoryOrder(solution, rin_op, *oid_iter))
			{
				//checkRoute(solution, rin_op);
				oid_iter = solution.routeVec[rin2].serveOrderList.erase(oid_iter);
				is_inserted = true;
				break;
			}
		}
		if (!is_inserted)
			oid_iter++;
	}
	arrangeOptionalOrder(solution, rin1);
	arrangeOptionalOrder(solution, rin2);
	//printRoute(solution, rin1);
	//calculateObjValue(solution, rin1);
	//checkRoute(solution, rin1);
	//printRoute(solution, rin2);
	//calculateObjValue(solution, rin2);
	//checkRoute(solution, rin2);

	/*for (int rin = 0; rin < solution.routeVec.size(); rin++)
	{
		printRoute(solution, rin);
		checkRoute(solution, rin);
	}*/
}
void Solver::generateRoute(Solution &solution, const int &rin)
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
		os << *iter << "\t";
	}
	os << solution.routeVec[rin].serveOrderList.size() << "\tclient id of mandatory order: \n";
	for (set<ClientID>::iterator iter = MMOrderClientIDSet.begin();
		iter != MMOrderClientIDSet.end(); iter++)
	{
		os << *iter << "\t";
	}
	vector<OrderID> MMOptionalOrderVec;		// the optional MM order
	set<ClientID> MMOrderInitClientIDSet;	// the initial ClientID set of the optional MM order
	os << endl << "the optional MM order:" << endl;
	for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
		iter != solution.routeVec[rin].serveOrderList.end(); iter++)
	{
		if (vr.orderVec[vr.orderMap[*iter]].getOrderType() == OrderType::Optional&&
			MMOrderClientIDSet.count(vr.orderVec[vr.orderMap[*iter]].getApplierID()) == 1 &&
			MMOrderClientIDSet.count(vr.orderVec[vr.orderMap[*iter]].getRequestID()) == 1)
		{
			MMOptionalOrderVec.push_back(*iter);
			MMOrderInitClientIDSet.insert(vr.orderVec[vr.orderMap[*iter]].getApplierID());
			os << *iter << "\t";
		}
	}
	os << endl;
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
	//os << "arrange the order sequence: " << endl;
	ClientID init_cid = vr.clientVec[vr.depot].PriDCID;
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
	MMOrderClientIDSet.erase(vr.clientVec[vr.depot].PriDCID);
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
	dsp->getShortPath(init_cid, vr.clientVec[vr.depot].PriDCID, shortest_distance, shortest_cid_vec);
	for (vector<ClientID>::iterator iter = ++shortest_cid_vec.begin(); iter != shortest_cid_vec.end(); iter++)
	{
		ServeClient sc2(*iter, 0);
		solution.routeVec[rin].serveClientList.push_back(sc2);
	}
	calculateObjValue(solution, rin);
	printRoute(solution, rin);
	checkRoute(solution, rin);
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
			insertMMOrderToRoute(solution, rin, iter, start_sc_iter, end_sc_iter);
		}
		else
			iter++;
	}
	calculateObjValue(solution, rin);
	printRoute(solution, rin);
	checkRoute(solution, rin);
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
				insertMOOrderToRoute(solution, rin, iter, start_sc_iter);
			}// insert OM optional order
			else if (start_sc_iter == solution.routeVec[rin].serveClientList.end() &&
				end_sc_iter != solution.routeVec[rin].serveClientList.end())
			{
				insertOMOrderToRoute(solution, rin, iter, end_sc_iter);
			}
			else
				iter++;
		}
	}
	calculateObjValue(solution, rin);
	printRoute(solution, rin);
	checkRoute(solution, rin);
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
			insertOOOrderToRoute(solution, rin, iter);
		}
		else
			iter++;
	}
	calculateObjValue(solution, rin);
	printRoute(solution, rin);
	checkRoute(solution, rin);
}
void Solver::cancelOrder(Solution &solution, const int &rin, const OrderID &oid)
{
	for (vector<OrderID>::iterator iter_oid = solution.routeVec[rin].serveClientList.front().loadOrderID.begin();
		iter_oid != solution.routeVec[rin].serveClientList.front().loadOrderID.end(); iter_oid++)
	{
		if (*iter_oid !=oid)
			continue;
		// erase the corresponding load order id 
		solution.routeVec[rin].serveClientList.front().loadOrderID.erase(iter_oid);
		break;
	}
	// substract the corresponding quantity from the serveClientList
	for (list<ServeClient>::iterator iter_sc = solution.routeVec[rin].serveClientList.begin();
		iter_sc != solution.routeVec[rin].serveClientList.end(); iter_sc++)
		iter_sc->currentQuantity -= vr.orderVec[vr.orderMap.at(oid)].getQuantity();	
}
bool Solver::isFeasibleAddServeTime(Solution &solution, const int &rin,  const list<ServeClient>::iterator &start_sc_iter, 
	const list<ServeClient>::iterator &end_sc_iter, const set<OrderID>::iterator &oid_iter)
{	
	/*if (rin == 6 && *oid_iter == "o149")
		system("pause");*/
	Timer::Duration start_end_dr(0), end_dr(0);
	if (start_sc_iter->arrivalTime.isEqual(start_sc_iter->departureTime))
		start_end_dr += vr.serveTimeDuration;
	// if the arrival time exceeds the due time of the order, skip it
	if (!Timer(start_end_dr, end_sc_iter->arrivalTime).isAhead(
		vr.orderVec[vr.orderMap.at(*oid_iter)].getDueTime()))
		return false;
	end_dr += start_end_dr;
	if (end_sc_iter->arrivalTime.isEqual(end_sc_iter->departureTime))
		end_dr += vr.serveTimeDuration;
	// check all the ServeClient to see whether it becomes tardy
	for (list<ServeClient>::iterator sc_iter = start_sc_iter; 
		sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
	{
		if (sc_iter == end_sc_iter)
		{
			start_end_dr = end_dr;
		}
		for (vector<OrderID>::iterator oid_iter = sc_iter->unloadOrderID.begin();
			oid_iter != sc_iter->unloadOrderID.end(); oid_iter++)
		{
			if (!Timer(start_end_dr, sc_iter->arrivalTime).isAhead(
				vr.orderVec[vr.orderMap.at(*oid_iter)].getDueTime()))
				return false;
		}
	}
	return true;
}
void Solver::generateRoute1(Solution &solution,const int &rin)
{
	// arrange the mandatory and MM optional orders
	ClientID init_cid = vr.clientVec[vr.depot].PriDCID;
	Timer now_time = Timer();
	ServeClient sc(init_cid, 0, now_time, Timer(vr.serveTimeDuration, now_time),0);
	set<ClientID> MMOrderClientIDSet;	// the ClientID set of mandatory order
	for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
		iter != solution.routeVec[rin].serveOrderList.end(); iter++)
	{
		if (vr.orderVec[vr.orderMap.at(*iter)].getOrderType() != OrderType::Mandatory)
			continue;
		// if the due time of the order  is ahead of the arrival time, skip this order
		if (!sc.arrivalTime.isAhead(vr.orderVec[vr.orderMap.at(*iter)].getDueTime()))
			continue;
		MMOrderClientIDSet.insert(vr.orderVec[vr.orderMap.at(*iter)].getApplierID());
		MMOrderClientIDSet.insert(vr.orderVec[vr.orderMap.at(*iter)].getRequestID());
		sc.loadOrderID.push_back(*iter);
		sc.currentQuantity += vr.orderVec[vr.orderMap.at(*iter)].getQuantity();
	}
	solution.routeVec[rin].serveClientList.push_back(sc);
	MMOrderClientIDSet.erase(vr.clientVec[vr.depot].PriDCID);
	while (!MMOrderClientIDSet.empty())
	{
		ClientID sel_end_cid;
		DistanceType shortest_distance;
		vector<ClientID> shortest_cid_vec;
		dsp->getShortPathClientIDSet(init_cid, MMOrderClientIDSet, sel_end_cid, shortest_distance, shortest_cid_vec);
		MMOrderClientIDSet.erase(sel_end_cid);
		if (sel_end_cid == "")
		{
			// no feasible path between init_cid to sel_end_cid
			continue;
		}
		// find all the order id corresponding to the same sel_end_cid
		bool is_exist_one_feasible_order = false;
		Timer::Duration tm_init_end = Timer::Duration((int)ceil(shortest_distance / vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].speed));
		list<OrderID>::iterator iter_end_oid = solution.routeVec[rin].serveOrderList.begin();
		for (; iter_end_oid != solution.routeVec[rin].serveOrderList.end();	)
		{
			if (vr.orderVec[vr.orderMap.at(*iter_end_oid)].getOrderType() == OrderType::Mandatory &&
				vr.orderVec[vr.orderMap.at(*iter_end_oid)].getRequestID() == sel_end_cid)
			{
				if (!Timer(tm_init_end + (/*solution.routeVec[rin].serveClientList.size() - add_mandatory_client_cnt +*/
					shortest_cid_vec.size() - 2)*vr.serveTimeDuration,	// time needed from inti client to end client
					solution.routeVec[rin].serveClientList.back().departureTime).isAhead(	// the departure time of previous client
					vr.orderVec[vr.orderMap.at(*iter_end_oid)].getDueTime()))	//	the due time of end client 
				{
					//  the arrival time is not ahead of the due time of the end client
					cancelOrder(solution, rin, *iter_end_oid);
					iter_end_oid = solution.routeVec[rin].serveOrderList.erase(iter_end_oid);
				}
				else
				{
					is_exist_one_feasible_order = true;
					iter_end_oid++;
				}
			}else
				iter_end_oid++;
		}
		// if all the orders which have the same sel_end_cid are tardy, skip it
		if (!is_exist_one_feasible_order)
			continue;
		for (vector<ClientID>::iterator iter = ++shortest_cid_vec.begin(); iter != --shortest_cid_vec.end(); iter++)
		{
			// insert the client between init_cid and sel_end_cid
			DistanceType dis_temp = vr.edgeVec[vr.orderEdge[vr.clientMap.at(solution.routeVec[rin].serveClientList.back().visitClientID)]
				[vr.clientMap.at(*iter)]].getDistance();
			Timer::Duration dr_temp = Timer::Duration((int)ceil(dis_temp / vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].speed));
			Timer ar_tm_temp = Timer(dr_temp, solution.routeVec[rin].serveClientList.back().departureTime);
			/*ServeClient sc1(*iter, solution.routeVec[rin].serveClientList.back().currentQuantity,
				ar_tm_temp, Timer(vr.serveTimeDuration, ar_tm_temp.getCurrentTimePoint()));*/
			ServeClient sc1(*iter, solution.routeVec[rin].serveClientList.back().currentQuantity, ar_tm_temp, ar_tm_temp,
				solution.routeVec[rin].serveClientList.back().alreadyDriveDistance + dis_temp);
			solution.routeVec[rin].serveClientList.push_back(sc1);
		}
		// insert the sel_end_cid and corresponding mandatory order which has the same sel_end_cid
		DistanceType dis = vr.edgeVec[vr.orderEdge[vr.clientMap.at(solution.routeVec[rin].serveClientList.back().visitClientID)]
			[vr.clientMap.at(sel_end_cid)]].getDistance();
		Timer::Duration dr = Timer::Duration((int)ceil(dis / vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].speed));
		Timer ar_tm = Timer(dr, solution.routeVec[rin].serveClientList.back().departureTime);

		ServeClient sc1(sel_end_cid, solution.routeVec[rin].serveClientList.back().currentQuantity,
			ar_tm, Timer(vr.serveTimeDuration, ar_tm), solution.routeVec[rin].serveClientList.back().alreadyDriveDistance + dis);
		for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
			iter != solution.routeVec[rin].serveOrderList.end();)
		{
			if (vr.orderVec[vr.orderMap.at(*iter)].getOrderType() == OrderType::Optional ||
				vr.orderVec[vr.orderMap.at(*iter)].getRequestID() != sel_end_cid)
			{
				iter++;
				continue;
			}
			// if the departure time is not ahead of the due time of the order, abandon this mandatory order
			if (!sc1.arrivalTime.isAhead(vr.orderVec[vr.orderMap.at(*iter)].getDueTime()))
			{
				// cancel the tardy order
				cancelOrder(solution, rin, *iter);
				// substract the corresponding quantity from the already established sc1
				sc1.currentQuantity -= vr.orderVec[vr.orderMap.at(*iter)].getQuantity();
			}
			else
			{
				sc1.unloadOrderID.push_back(*iter);
				sc1.currentQuantity -= vr.orderVec[vr.orderMap.at(*iter)].getQuantity();
			}
			iter = solution.routeVec[rin].serveOrderList.erase(iter);
		}
		solution.routeVec[rin].serveClientList.push_back(sc1);
		init_cid = sel_end_cid;
	}
	// insert the last visiting client depot
	vector<ClientID> shortest_cid_vec;
	DistanceType shortest_distance;
	dsp->getShortPath(init_cid, vr.clientVec[vr.depot].PriDCID, shortest_distance, shortest_cid_vec);
	for (vector<ClientID>::iterator iter = ++shortest_cid_vec.begin(); iter != shortest_cid_vec.end(); iter++)
	{
		vector<ClientID>::iterator iter_prev = iter;
		iter_prev--;
		// insert the client between init_cid and the last visiting client depot
		DistanceType dis_temp = vr.edgeVec[vr.orderEdge[vr.clientMap.at(*iter_prev)][vr.clientMap.at(*iter)]].getDistance();
		Timer::Duration dr_temp = Timer::Duration((int)ceil(dis_temp / vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].speed));
		Timer ar_tm_temp = Timer(dr_temp, solution.routeVec[rin].serveClientList.back().departureTime);
		/*ServeClient sc1(*iter, solution.routeVec[rin].serveClientList.back().currentQuantity,
		ar_tm_temp, Timer(vr.serveTimeDuration, ar_tm_temp.getCurrentTimePoint()));*/
		ServeClient sc1(*iter, 0, ar_tm_temp, ar_tm_temp, solution.routeVec[rin].serveClientList.back().alreadyDriveDistance + dis_temp);
		//ServeClient sc1(*iter, 0);
		solution.routeVec[rin].serveClientList.push_back(sc1);
	}
	if (solution.routeVec[rin].serveClientList.size() == 1)
	{
		solution.routeVec[rin].serveClientList.front().departureTime =
			solution.routeVec[rin].serveClientList.front().arrivalTime;
	}
	calculateObjValue(solution, rin);
	printRoute(solution, rin);
	checkRoute(solution, rin);
	// insert the mandatory optional order to the route
	for (set<OrderID>::iterator iter = solution.optionalOrderIDSet.begin();
		iter != solution.optionalOrderIDSet.end();)
	{
		list<ServeClient>::iterator start_sc_iter, end_sc_iter;
		bool is_operated = false;
		for (start_sc_iter = solution.routeVec[rin].serveClientList.begin();
			start_sc_iter != solution.routeVec[rin].serveClientList.end(); start_sc_iter++)
		{
			if (start_sc_iter->visitClientID != vr.orderVec[vr.orderMap.at(*iter)].getApplierID())
				continue;
			
			for (end_sc_iter = start_sc_iter; end_sc_iter != solution.routeVec[rin].serveClientList.end(); end_sc_iter++)
			{
				// if adding the MM optional order exceeds the vehicle capacity, skip it
				if (end_sc_iter->currentQuantity + vr.orderVec[vr.orderMap[*iter]].getQuantity() >
					vr.vehicleVec[vr.vehicleMap[solution.routeVec[rin].VehID]].capacity)
					break;
				if (end_sc_iter->visitClientID != vr.orderVec[vr.orderMap.at(*iter)].getRequestID())
					continue;
				if (!isFeasibleAddServeTime(solution, rin, start_sc_iter, end_sc_iter, iter))
					continue;
				// pick up this order
				start_sc_iter->loadOrderID.push_back(*iter);
				end_sc_iter->unloadOrderID.push_back(*iter);
				for (list<ServeClient>::iterator sc_iter = start_sc_iter; sc_iter != end_sc_iter; sc_iter++)
				{
					sc_iter->currentQuantity += vr.orderVec[vr.orderMap[*iter]].getQuantity();
				}
				// the start ServeClient has nothing to load or unload, add this serve time duration
				if (start_sc_iter->arrivalTime.isEqual(start_sc_iter->departureTime))
				{
					for (list<ServeClient>::iterator sc_iter = start_sc_iter;
						sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
					{
						if (sc_iter!=start_sc_iter)
							sc_iter->arrivalTime.addDuration(vr.serveTimeDuration);
						sc_iter->departureTime.addDuration(vr.serveTimeDuration);
					}
				}
				// the end ServeClient has nothing to load or unload, add this serve time duration
				if (end_sc_iter->arrivalTime.isEqual(end_sc_iter->departureTime))
				{
					for (list<ServeClient>::iterator sc_iter = end_sc_iter;
						sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
					{
						if (sc_iter != end_sc_iter)
							sc_iter->arrivalTime.addDuration(vr.serveTimeDuration);
						sc_iter->departureTime.addDuration(vr.serveTimeDuration);
					}
				}
				// erase the inserted order id from optioanl order list
				iter = solution.optionalOrderIDSet.erase(iter);
				is_operated = true;
				break;
			}
			if (is_operated)
				break;
		}
		if (!is_operated)
		{			
			iter++;
		}
	}
	calculateObjValue(solution, rin);
	printRoute(solution, rin);
	checkRoute(solution, rin);
}
void Solver::arrangeMandatoryOrder(Solution &solution, const int &rin)
{
	solution.routeVec[rin].serveClientList.clear();
	solution.routeVec[rin].visitClientIDSet.clear();
	// arrange the mandatory and MM optional orders
	ClientID init_cid = vr.clientVec[vr.depot].PriDCID;
	Timer now_time = Timer();
	ServeClient sc(init_cid, 0, now_time, Timer(vr.serveTimeDuration, now_time), 0);
	set<ClientID> MMOrderClientIDSet;	// the ClientID set of mandatory order
	for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
		iter != solution.routeVec[rin].serveOrderList.end(); )
	{
		if (sc.currentQuantity + vr.orderVec[vr.orderMap.at(*iter)].getQuantity()>
			vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].capacity)
		{
			iter = solution.routeVec[rin].serveOrderList.erase(iter);
			//iter++;
			continue;
		}
		// if the due time of the order  is ahead of the arrival time, skip this order
		if (!sc.arrivalTime.isAhead(vr.orderVec[vr.orderMap.at(*iter)].getDueTime()))
		{
			iter = solution.routeVec[rin].serveOrderList.erase(iter);
			//iter++;
			continue;
		}
		MMOrderClientIDSet.insert(vr.orderVec[vr.orderMap.at(*iter)].getApplierID());
		MMOrderClientIDSet.insert(vr.orderVec[vr.orderMap.at(*iter)].getRequestID());
		sc.loadOrderID.push_back(*iter);		
		sc.currentQuantity += vr.orderVec[vr.orderMap.at(*iter)].getQuantity();
		iter++;
	}
	solution.routeVec[rin].serveClientList.push_back(sc);
	solution.routeVec[rin].visitClientIDSet.insert(sc.visitClientID);
	MMOrderClientIDSet.erase(vr.clientVec[vr.depot].PriDCID);
	while (!MMOrderClientIDSet.empty())
	{
		ClientID sel_end_cid;
		DistanceType shortest_distance;
		vector<ClientID> shortest_cid_vec;
		dsp->getShortPathClientIDSet(init_cid, MMOrderClientIDSet, sel_end_cid, shortest_distance, shortest_cid_vec);
		MMOrderClientIDSet.erase(sel_end_cid);
		if (sel_end_cid == "")
		{
			// no feasible path between init_cid to sel_end_cid
			continue;
		}
		// find all the order id corresponding to the same sel_end_cid
		bool is_exist_one_feasible_order = false;
		Timer::Duration tm_init_end = Timer::Duration((int)ceil(shortest_distance / vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].speed));
		list<OrderID>::iterator iter_end_oid = solution.routeVec[rin].serveOrderList.begin();
		for (; iter_end_oid != solution.routeVec[rin].serveOrderList.end(); iter_end_oid++)
		{
			if (vr.orderVec[vr.orderMap.at(*iter_end_oid)].getOrderType() == OrderType::Mandatory &&
				vr.orderVec[vr.orderMap.at(*iter_end_oid)].getRequestID() == sel_end_cid)
			{
				if (!Timer(tm_init_end + (/*solution.routeVec[rin].serveClientList.size() - add_mandatory_client_cnt +*/
					shortest_cid_vec.size() - 2)*vr.serveTimeDuration,	// time needed from inti client to end client
					solution.routeVec[rin].serveClientList.back().departureTime).isAhead(	// the departure time of previous client
					vr.orderVec[vr.orderMap.at(*iter_end_oid)].getDueTime()))	//	the due time of end client 
				{
					//  the arrival time is not ahead of the due time of the end client
					cancelOrder(solution, rin, *iter_end_oid);
					//iter_end_oid = solution.routeVec[rin].serveOrderList.erase(iter_end_oid);					
				}
				else
					is_exist_one_feasible_order = true;				
			}
		}
		// if all the orders which have the same sel_end_cid are tardy, skip it
		if (!is_exist_one_feasible_order)
			continue;
		for (vector<ClientID>::iterator iter = ++shortest_cid_vec.begin(); iter != --shortest_cid_vec.end(); iter++)
		{
			// insert the client between init_cid and sel_end_cid
			DistanceType dis_temp = vr.edgeVec[vr.orderEdge[vr.clientMap.at(solution.routeVec[rin].serveClientList.back().visitClientID)]
				[vr.clientMap.at(*iter)]].getDistance();
			Timer::Duration dr_temp = Timer::Duration((int)ceil(dis_temp / vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].speed));
			Timer ar_tm_temp = Timer(dr_temp, solution.routeVec[rin].serveClientList.back().departureTime);
			/*ServeClient sc1(*iter, solution.routeVec[rin].serveClientList.back().currentQuantity,
			ar_tm_temp, Timer(vr.serveTimeDuration, ar_tm_temp.getCurrentTimePoint()));*/
			ServeClient sc1(*iter, solution.routeVec[rin].serveClientList.back().currentQuantity, ar_tm_temp, ar_tm_temp,
				solution.routeVec[rin].serveClientList.back().alreadyDriveDistance + dis_temp);
			solution.routeVec[rin].serveClientList.push_back(sc1);
			solution.routeVec[rin].visitClientIDSet.insert(sc1.visitClientID);
		}
		// insert the sel_end_cid and corresponding mandatory order which has the same sel_end_cid
		DistanceType dis = vr.edgeVec[vr.orderEdge[vr.clientMap.at(solution.routeVec[rin].serveClientList.back().visitClientID)]
			[vr.clientMap.at(sel_end_cid)]].getDistance();
		Timer::Duration dr = Timer::Duration((int)ceil(dis / vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].speed));
		Timer ar_tm = Timer(dr, solution.routeVec[rin].serveClientList.back().departureTime);

		ServeClient sc1(sel_end_cid, solution.routeVec[rin].serveClientList.back().currentQuantity,
			ar_tm, Timer(vr.serveTimeDuration, ar_tm), solution.routeVec[rin].serveClientList.back().alreadyDriveDistance + dis);
		for (list<OrderID>::iterator iter = solution.routeVec[rin].serveOrderList.begin();
			iter != solution.routeVec[rin].serveOrderList.end();)
		{
			if (vr.orderVec[vr.orderMap.at(*iter)].getOrderType() == OrderType::Optional ||
				vr.orderVec[vr.orderMap.at(*iter)].getRequestID() != sel_end_cid)
			{
				iter++;
				continue;
			}
			// if the departure time is not ahead of the due time of the order, abandon this mandatory order
			if (!sc1.arrivalTime.isAhead(vr.orderVec[vr.orderMap.at(*iter)].getDueTime()))
			{
				// cancel the tardy order
				cancelOrder(solution, rin, *iter);
				// substract the corresponding quantity from the already established sc1
				sc1.currentQuantity -= vr.orderVec[vr.orderMap.at(*iter)].getQuantity();
			}
			else
			{
				sc1.unloadOrderID.push_back(*iter);
				sc1.currentQuantity -= vr.orderVec[vr.orderMap.at(*iter)].getQuantity();
			}
			
			iter = solution.routeVec[rin].serveOrderList.erase(iter);
		}
		solution.routeVec[rin].serveClientList.push_back(sc1);
		solution.routeVec[rin].visitClientIDSet.insert(sc1.visitClientID);
		init_cid = sel_end_cid;
	}
	// insert the last visiting client depot
	vector<ClientID> shortest_cid_vec;
	DistanceType shortest_distance;
	dsp->getShortPath(init_cid, vr.clientVec[vr.depot].PriDCID, shortest_distance, shortest_cid_vec);
	for (vector<ClientID>::iterator iter = ++shortest_cid_vec.begin(); iter != shortest_cid_vec.end(); iter++)
	{
		vector<ClientID>::iterator iter_prev = iter;
		iter_prev--;
		// insert the client between init_cid and the last visiting client depot
		DistanceType dis_temp = vr.edgeVec[vr.orderEdge[vr.clientMap.at(*iter_prev)][vr.clientMap.at(*iter)]].getDistance();
		Timer::Duration dr_temp = Timer::Duration((int)ceil(dis_temp / vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].speed));
		Timer ar_tm_temp = Timer(dr_temp, solution.routeVec[rin].serveClientList.back().departureTime);
		ServeClient sc1(*iter, 0, ar_tm_temp, ar_tm_temp, solution.routeVec[rin].serveClientList.back().alreadyDriveDistance + dis_temp);
		//ServeClient sc1(*iter, 0);
		solution.routeVec[rin].serveClientList.push_back(sc1);
		solution.routeVec[rin].visitClientIDSet.insert(sc1.visitClientID);
	}
	// if this route has on clients except the depot
	if (solution.routeVec[rin].serveClientList.size() == 1)
	{
		solution.routeVec[rin].serveClientList.front().departureTime =
			solution.routeVec[rin].serveClientList.front().arrivalTime;
	}

}
void Solver::arrangeOptionalOrder(Solution &solution, const int &rin)
{
	// insert the mandatory optional order to the route
	for (set<OrderID>::iterator iter = solution.optionalOrderIDSet.begin();
		iter != solution.optionalOrderIDSet.end();)
	{
		list<ServeClient>::iterator start_sc_iter, end_sc_iter;
		bool is_operated = false;
		for (start_sc_iter = solution.routeVec[rin].serveClientList.begin();
			start_sc_iter != solution.routeVec[rin].serveClientList.end(); start_sc_iter++)
		{
			if (start_sc_iter->visitClientID != vr.orderVec[vr.orderMap.at(*iter)].getApplierID())
				continue;

			for (end_sc_iter = start_sc_iter; end_sc_iter != solution.routeVec[rin].serveClientList.end(); end_sc_iter++)
			{
				// if adding the MM optional order exceeds the vehicle capacity, skip it
				if (end_sc_iter->currentQuantity + vr.orderVec[vr.orderMap[*iter]].getQuantity() >
					vr.vehicleVec[vr.vehicleMap[solution.routeVec[rin].VehID]].capacity)
					break;
				if (end_sc_iter->visitClientID != vr.orderVec[vr.orderMap.at(*iter)].getRequestID())
					continue;
				if (!isFeasibleAddServeTime(solution, rin, start_sc_iter, end_sc_iter, iter))
					continue;			
				// pick up this order
				start_sc_iter->loadOrderID.push_back(*iter);
				end_sc_iter->unloadOrderID.push_back(*iter);
				for (list<ServeClient>::iterator sc_iter = start_sc_iter; sc_iter != end_sc_iter; sc_iter++)
				{
					sc_iter->currentQuantity += vr.orderVec[vr.orderMap[*iter]].getQuantity();
				}
				// the start ServeClient has nothing to load or unload, add this serve time duration
				if (start_sc_iter->arrivalTime.isEqual(start_sc_iter->departureTime))
				{
					for (list<ServeClient>::iterator sc_iter = start_sc_iter;
						sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
					{
						if (sc_iter != start_sc_iter)
							sc_iter->arrivalTime.addDuration(vr.serveTimeDuration);
						sc_iter->departureTime.addDuration(vr.serveTimeDuration);
					}
				}
				// the end ServeClient has nothing to load or unload, add this serve time duration
				if (end_sc_iter->arrivalTime.isEqual(end_sc_iter->departureTime))
				{
					for (list<ServeClient>::iterator sc_iter = end_sc_iter;
						sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
					{
						if (sc_iter != end_sc_iter)
							sc_iter->arrivalTime.addDuration(vr.serveTimeDuration);
						sc_iter->departureTime.addDuration(vr.serveTimeDuration);
					}
				}
				// erase the inserted order id from optioanl order list
				iter = solution.optionalOrderIDSet.erase(iter);
				is_operated = true;
				break;
			}
			if (is_operated)
				break;
		}
		if (!is_operated)
		{
			iter++;
		}
	}
}

bool Solver::removeMandatoryOrder(Solution &solution, const int &rin, const ClientID &cid)
{
	DistanceType dis;
	vector<ClientID> cid_vec;
	list<ServeClient>::iterator sc_iter_prev, sc_iter_next, sc_iter;
	for (sc_iter = solution.routeVec[rin].serveClientList.begin();
		sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
	{
		if (sc_iter->visitClientID == cid)
			break;
	}
	//for (list<ServeClient>::iterator sc_itr = sc_iter; sc_itr != solution.routeVec[rin].serveClientList.rbegin();)
	sc_iter_prev--;
	sc_iter_next++;
	dsp->getShortPath(sc_iter_prev->visitClientID, sc_iter_next->visitClientID, dis, cid_vec);
	// if the client needed to be removed is in the route from prev to next, do not remove it
	for (vector<ClientID>::iterator cid_iter = cid_vec.begin(); cid_iter != cid_vec.end(); cid_iter++)
		if (*cid_iter == cid)
			return false;

	for (list<ServeClient>::iterator sc_iter = solution.routeVec[rin].serveClientList.begin();
		sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
	{
		if (sc_iter->visitClientID == cid)
		{
			for (vector<OrderID>::iterator oid_load_iter = sc_iter->loadOrderID.begin();
				oid_load_iter != sc_iter->loadOrderID.end(); oid_load_iter++)
			{
				if (vr.orderVec[vr.orderMap.at(*oid_load_iter)].getOrderType() == OrderType::Mandatory)
					solution.routeVec[rin].serveOrderList.push_back(*oid_load_iter);
				else
					solution.optionalOrderIDSet.insert(*oid_load_iter);
			}
			for (vector<OrderID>::iterator oid_unload_iter = sc_iter->unloadOrderID.begin();
				oid_unload_iter != sc_iter->unloadOrderID.end(); oid_unload_iter++)
			{
				if (vr.orderVec[vr.orderMap.at(*oid_unload_iter)].getOrderType() == OrderType::Mandatory)
					solution.routeVec[rin].serveOrderList.push_back(*oid_unload_iter);
				else
					solution.optionalOrderIDSet.insert(*oid_unload_iter);
			}
		}
	}
	return true;
}
bool Solver::insertMandatoryOrder(Solution &solution, const int &rin, const OrderID &oid)
{
	// this client id has already existed in this route
	if (solution.routeVec[rin].visitClientIDSet.count(vr.orderVec[vr.orderMap.at(oid)].getRequestID()) == 1)
	{		
		// check if the order in the back is tardy
		for (list<ServeClient>::iterator sc_iter = solution.routeVec[rin].serveClientList.begin();
			sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
		{
			// the order exceeds the quantity of the vehicle, from request and suplier client id of the oid
			if (sc_iter->currentQuantity + vr.orderVec[vr.orderMap.at(oid)].getQuantity() >
				vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].capacity)
				return false;
			if (sc_iter->visitClientID == vr.orderVec[vr.orderMap.at(oid)].getRequestID())
			{
				// check if this order oid is tardy
				if (!sc_iter->arrivalTime.isAhead(vr.orderVec[vr.orderMap.at(oid)].getDueTime()))
					return false;
				if (sc_iter->loadOrderID.size() == 0 && sc_iter->unloadOrderID.size() == 0)
				{
					for (list<ServeClient>::iterator sc_add_iter = sc_iter;
						sc_add_iter != solution.routeVec[rin].serveClientList.end(); sc_add_iter++)
					{
						for (vector<OrderID>::iterator oid_iter = sc_add_iter->unloadOrderID.begin();
							oid_iter != sc_add_iter->unloadOrderID.end(); oid_iter++)
						{
							if (!Timer(vr.serveTimeDuration, sc_add_iter->arrivalTime).isAhead(vr.orderVec[vr.orderMap.at(*oid_iter)].getDueTime()))
							{
								return false;
							}
						}
					}
				}
				else
					break;
			}
		}
		for (list<ServeClient>::iterator sc_iter = solution.routeVec[rin].serveClientList.begin();
			sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
		{
			if (sc_iter == solution.routeVec[rin].serveClientList.begin())
				sc_iter->loadOrderID.push_back(oid);
			if (sc_iter->visitClientID == vr.orderVec[vr.orderMap.at(oid)].getRequestID())
			{
				if (sc_iter->loadOrderID.size() == 0 && sc_iter->unloadOrderID.size() == 0)
				{
					for (list<ServeClient>::iterator sc_add_iter = sc_iter;
						sc_add_iter != solution.routeVec[rin].serveClientList.end(); sc_add_iter++)
					{
						if (sc_add_iter != sc_iter)
							sc_add_iter->arrivalTime.addDuration(vr.serveTimeDuration);
						sc_add_iter->departureTime.addDuration(vr.serveTimeDuration);
					}
				}
				sc_iter->unloadOrderID.push_back(oid);
				return true;
			}
			sc_iter->currentQuantity += vr.orderVec[vr.orderMap.at(oid)].getQuantity();
		}
	}
	DistanceType shortest_to_distance, shortest_back_distance;
	vector<ClientID> shortest_to_cid_vec, shortest_back_cid_vec;
	ClientID sel_cid;
	dsp->getShortPathClientIDSet(vr.orderVec[vr.orderMap.at(oid)].getRequestID(), solution.routeVec[rin].visitClientIDSet,
		sel_cid, shortest_to_distance, shortest_to_cid_vec);
	std::reverse(shortest_to_cid_vec.begin(), shortest_to_cid_vec.end());
	list<ServeClient>::iterator sel_sc_iter, sel_sc_iter_next;
	for (sel_sc_iter = solution.routeVec[rin].serveClientList.begin();
		sel_sc_iter != solution.routeVec[rin].serveClientList.end(); sel_sc_iter++)
	{
		// the order exceeds the quantity of the vehicle, from request and suplier client id of the oid
		if (sel_sc_iter->currentQuantity + vr.orderVec[vr.orderMap.at(oid)].getQuantity() >
			vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].capacity)
			return false;

		if (sel_sc_iter->visitClientID == sel_cid)
			break;
	}
	//Timer::Duration extra_to_dr((int)ceil(shortest_to_distance / vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].speed));
	Timer::Duration extra_to_dr(0);
	DistanceType extra_to_dis(0);
	for (vector<ClientID>::iterator cid_iter = shortest_to_cid_vec.begin();
		cid_iter != --shortest_to_cid_vec.end(); cid_iter++)
	{
		vector<ClientID>::iterator cid_iter_next = cid_iter;
		cid_iter_next++;
		extra_to_dis += vr.edgeVec[vr.orderEdge[vr.clientMap.at(*cid_iter)]
			[vr.clientMap.at(*cid_iter_next)]].getDistance();
		extra_to_dr += Timer::Duration((int)ceil(vr.edgeVec[vr.orderEdge[vr.clientMap.at(*cid_iter)]
			[vr.clientMap.at(*cid_iter_next)]].getDistance() / vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].speed));
	}
	// check if this order is tardy
	if (!Timer(extra_to_dr, sel_sc_iter->departureTime).isAhead(vr.orderVec[vr.orderMap.at(oid)].getDueTime()))
		return false;
	sel_sc_iter_next = sel_sc_iter;
	sel_sc_iter_next++;
	dsp->getShortPath(vr.orderVec[vr.orderMap.at(oid)].getRequestID(), sel_sc_iter_next->visitClientID, shortest_back_distance, shortest_back_cid_vec);
	//Timer::Duration extra_back_dr((int)ceil(shortest_back_distance / vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].speed));
	Timer::Duration extra_back_dr(0);
	DistanceType extra_back_dis(0);
	for (vector<ClientID>::iterator cid_iter = shortest_back_cid_vec.begin();
		cid_iter != --shortest_back_cid_vec.end(); cid_iter++)
	{
		vector<ClientID>::iterator cid_iter_next = cid_iter;
		cid_iter_next++;
		extra_back_dis += vr.edgeVec[vr.orderEdge[vr.clientMap.at(*cid_iter)]
			[vr.clientMap.at(*cid_iter_next)]].getDistance();
		extra_back_dr += Timer::Duration((int)ceil(vr.edgeVec[vr.orderEdge[vr.clientMap.at(*cid_iter)]
			[vr.clientMap.at(*cid_iter_next)]].getDistance() / vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].speed));
	}
	for (list<ServeClient>::iterator sc_iter = sel_sc_iter_next;
		sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
	{
		for (vector<OrderID>::iterator oid_iter = sc_iter->unloadOrderID.begin();
			oid_iter != sc_iter->unloadOrderID.end(); oid_iter++)
		{
			if (!Timer(extra_to_dr + extra_back_dr + vr.serveTimeDuration, sc_iter->arrivalTime).isAhead(
				vr.orderVec[vr.orderMap.at(*oid_iter)].getDueTime()))
				return false;
		}
	}
	// insert this order to the route
	// from the front of the route to sel_sc_iter_next
	for (list<ServeClient>::iterator sc_iter = solution.routeVec[rin].serveClientList.begin();
		sc_iter != sel_sc_iter_next; sc_iter++)
	{
		if (sc_iter == solution.routeVec[rin].serveClientList.begin())
			sc_iter->loadOrderID.push_back(oid);
		sc_iter->currentQuantity += vr.orderVec[vr.orderMap.at(oid)].getQuantity();
	}
	// from sel_sc_iter to oid
	for (vector<ClientID>::iterator cid_iter = ++shortest_to_cid_vec.begin(); cid_iter != shortest_to_cid_vec.end(); cid_iter++)
	{
		vector<ClientID>::iterator cid_iter_prev = cid_iter;
		cid_iter_prev--;
		DistanceType dis_temp = vr.edgeVec[vr.orderEdge[vr.clientMap.at(*cid_iter_prev)][vr.clientMap.at(*cid_iter)]].getDistance();
		Timer::Duration dr_temp((int)ceil(dis_temp / vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].speed));
		list<ServeClient>::iterator sc_iter_next_prev = sel_sc_iter_next;
		sc_iter_next_prev--;
		Timer tm_temp = Timer(dr_temp, sc_iter_next_prev->departureTime);
		ServeClient sc = ServeClient(*cid_iter, sel_sc_iter->currentQuantity, tm_temp, tm_temp,
			sc_iter_next_prev->alreadyDriveDistance + dis_temp);
		// the last one in the vector
		if (cid_iter == --shortest_to_cid_vec.end())
		{
			sc.unloadOrderID.push_back(oid);
			sc.currentQuantity -= vr.orderVec[vr.orderMap.at(oid)].getQuantity();
			sc.departureTime.addDuration(vr.serveTimeDuration);
		}
		solution.routeVec[rin].serveClientList.insert(sel_sc_iter_next, sc);
		solution.routeVec[rin].visitClientIDSet.insert(*cid_iter);
	}
	// from oid to sel_sc_iter_next, add ServeClient
	for (vector<ClientID>::iterator cid_iter = ++shortest_back_cid_vec.begin(); cid_iter != --shortest_back_cid_vec.end(); cid_iter++)
	{
		vector<ClientID>::iterator cid_iter_prev = cid_iter;
		cid_iter_prev--;
		DistanceType dis_temp = vr.edgeVec[vr.orderEdge[vr.clientMap.at(*cid_iter_prev)][vr.clientMap.at(*cid_iter)]].getDistance();
		Timer::Duration dr_temp((int)ceil(dis_temp / vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].speed));
		list<ServeClient>::iterator sc_iter_next_prev = sel_sc_iter_next;
		sc_iter_next_prev--;
		Timer tm_temp = Timer(dr_temp, sc_iter_next_prev->departureTime);
		ServeClient sc = ServeClient(*cid_iter, sc_iter_next_prev->currentQuantity, tm_temp, tm_temp,
			sc_iter_next_prev->alreadyDriveDistance + dis_temp);
		solution.routeVec[rin].serveClientList.insert(sel_sc_iter_next, sc);
		solution.routeVec[rin].visitClientIDSet.insert(*cid_iter);
	}
	// from sel_sc_iter_next to the end of the route, add time and distance
	DistanceType dis_prev = vr.edgeVec[vr.orderEdge[vr.clientMap.at(sel_sc_iter->visitClientID)]
		[vr.clientMap.at(sel_sc_iter_next->visitClientID)]].getDistance();
	Timer::Duration dr_prev((int)ceil(dis_prev / vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].speed));
	for (list<ServeClient>::iterator sc_iter = sel_sc_iter_next;
		sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
	{
		sc_iter->arrivalTime.addDuration(extra_to_dr + extra_back_dr - dr_prev + vr.serveTimeDuration);
		sc_iter->departureTime.addDuration(extra_to_dr + extra_back_dr - dr_prev + vr.serveTimeDuration);
		sc_iter->alreadyDriveDistance += extra_back_dis + extra_to_dis - dis_prev;
	}	
	return true;
}
void Solver::insertMMOrderToRoute1(Solution &solution, const int &rin, list<OrderID>::iterator &iter,
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
	if (sc_iter->visitClientID == start_cid&&	// the order is along with the route 
		start_sc_iter->currentQuantity + vr.orderVec[vr.orderMap[*iter]].getQuantity() <=
			vr.vehicleVec[vr.vehicleMap[solution.routeVec[rin].VehID]].capacity)
	{
		// if adding the MM optional order does not exceed the vehicle capacity, add sc_iter
		start_sc_iter->loadOrderID.push_back(*iter);
		end_sc_iter->unloadOrderID.push_back(*iter);
		for (list<ServeClient>::iterator it1 = start_sc_iter; it1 != end_sc_iter; it1++)
		{
			it1->currentQuantity += vr.orderVec[vr.orderMap[*iter]].getQuantity();
		}
		// erase the inserted order id from mandatory order list
		//iter = solution.optionalOrderIDSet.erase(iter);
	}
	else
		iter++;
		
}
void Solver::insertMMOrderToRoute(Solution &solution, const int &rin, list<OrderID>::iterator &iter,
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
void Solver::insertMOOrderToRoute(Solution &solution, const int &rin, list<OrderID>::iterator &iter, list<ServeClient>::iterator &start_sc_iter)
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
void Solver::insertOMOrderToRoute(Solution &solution, const int &rin, list<OrderID>::iterator &iter, list<ServeClient>::iterator &end_sc_iter)
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
		if (checkClientInRoute(solution, rin, out_shortest_cid_vec) || checkClientInRoute(solution, rin, in_shortest_cid_vec))
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
void Solver::insertOOOrderToRoute(Solution &solution, const int &rin, list<OrderID>::iterator &iter)
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
void Solver::printRoute(const Solution &solution, const int &rin)const
{
	os << endl << "The information of route " << rin << " :" << endl;
	os << "it visits " << solution.routeVec[rin].serveClientList.size() << " clients: " << endl;
	for (list<ServeClient>::const_iterator iter = solution.routeVec[rin].serveClientList.begin();
		iter != solution.routeVec[rin].serveClientList.end(); iter++)
	{
		//os << iter->visitClientID;
		os << vr.clientVec[vr.clientMap.at(iter->visitClientID)].clientName;
		if (iter != --solution.routeVec[rin].serveClientList.end())
			os << " -> ";
	}
	os << endl << "The detail information of each client:" << endl;
	for (list<ServeClient>::const_iterator iter = solution.routeVec[rin].serveClientList.begin();
		iter != solution.routeVec[rin].serveClientList.end(); iter++)
	{
		os << vr.clientVec[vr.clientMap.at(iter->visitClientID)].clientName 
			<< "(" << iter->visitClientID << "),"
			<< iter->arrivalTime << ","<<iter->departureTime
			<< ", already driven distance: "<<iter->alreadyDriveDistance
			<< ", quantity: " << (*iter).currentQuantity
			<< ", load:" << (*iter).loadOrderID.size() << ": ";
		for (vector<OrderID>::const_iterator it = (*iter).loadOrderID.begin();
			it != (*iter).loadOrderID.end(); it++)
		{
			//os << *it << " ";
			os << *it << " (" << vr.orderVec[vr.orderMap.at(*it)] << ") ";
		}
		os << "; unload: " << (*iter).unloadOrderID.size() << ":";
		for (vector<OrderID>::const_iterator it = (*iter).unloadOrderID.begin();
			it != (*iter).unloadOrderID.end(); it++)
		{
			os << *it << " ";
		}
		os << endl;
	}
	os << "\nThe left mandatory orders:" << solution.routeVec[rin].serveOrderList.size() << endl;
	for (list<OrderID>::const_iterator iter = solution.routeVec[rin].serveOrderList.begin();
		iter != solution.routeVec[rin].serveOrderList.end(); iter++)
		os << vr.orderVec[vr.orderMap.at(*iter)] << endl;
}
bool Solver::checkSolution(const Solution &solution)const
{
	//vector<set<OrderID>> serveOrderIDSet(solution.routeVec.size());
	set<OrderID> serveOrderIDSet;
	for (int rin = 0; rin < solution.routeVec.size(); rin++)
	{
		if (!checkRoute(solution, rin))
			return false;
		for (list<ServeClient>::const_iterator sc_iter = solution.routeVec[rin].serveClientList.begin();
			sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
		{
			for (vector<OrderID>::const_iterator oid_iter = sc_iter->loadOrderID.begin();
				oid_iter != sc_iter->loadOrderID.end(); oid_iter++)
				if (serveOrderIDSet.count(*oid_iter) == 0)
					serveOrderIDSet.insert(*oid_iter);
				else
				{
					os << "Check solution WRONG multiple order served " 
						<< *oid_iter << endl;
					system("pause");
					return false;
				}
		}
	}
	return true;
}
bool Solver::checkRoute(const Solution &solution, const int &rin)const
{
	set<OrderID> handle_oid_set;
	QuantityType real_cur_quantity = 0;
	DistanceType already_drive_dis = 0;
	Timer real_cur_time = solution.routeVec[rin].serveClientList.front().arrivalTime;
	for (list<ServeClient>::const_iterator iter = solution.routeVec[rin].serveClientList.begin();
		iter != solution.routeVec[rin].serveClientList.end(); iter++)
	{
		if (iter != solution.routeVec[rin].serveClientList.begin())
		{
			list<ServeClient>::const_iterator iter_prev = iter;
			iter_prev--;
			real_cur_time.addDuration(Timer::Duration((int)ceil(vr.edgeVec[vr.orderEdge[vr.clientMap.at(iter_prev->visitClientID)]
				[vr.clientMap.at(iter->visitClientID)]].getDistance()/vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].speed)));
			already_drive_dis += vr.edgeVec[vr.orderEdge[vr.clientMap.at(iter_prev->visitClientID)]
				[vr.clientMap.at(iter->visitClientID)]].getDistance();
		}
		if (abs(iter->alreadyDriveDistance - already_drive_dis) > ABS_EQUAL_DEV)
		{
			os << "Check route " << rin << " " << iter->visitClientID << " already drived distance WRONG: "
				<< already_drive_dis << "\t" << iter->alreadyDriveDistance << endl;
			system("pause");
			return false;
		}
		if (!real_cur_time.isEqual(iter->arrivalTime))
		{
			os << "Check route " << rin << " " << iter->visitClientID << " arrival time WRONG: "
				<< real_cur_time << "\t" << iter->arrivalTime << endl;
			system("pause");
			return false;
		}
		if (iter->loadOrderID.size() != 0 || iter->unloadOrderID.size() != 0)
			real_cur_time.addDuration(vr.serveTimeDuration);
		if (!real_cur_time.isEqual(iter->departureTime))
		{
			os << "Check route " << rin << " " << iter->visitClientID << " departure time WRONG: "
				<< real_cur_time << "\t" << iter->departureTime<< endl;
			system("pause");
			return false;
		}
		//os << iter->visitClientID << ", quantity: " << (*iter).currentQuantity
		//	<< ", load:" << (*iter).loadOrderID.size() << ": ";
		for (vector<OrderID>::const_iterator it = (*iter).loadOrderID.begin();
			it != (*iter).loadOrderID.end(); it++)
		{
			if (handle_oid_set.count(*it) == 1)
			{
				os << "Check route " << rin << " " << iter->visitClientID << " WRONG multiple loaded order "
					<< *it << endl;
				system("pause");
			}
			handle_oid_set.insert(*it);
			if (!vr.orderVec[vr.orderMap.at(*it)].getReadyTime().isAhead(iter->departureTime))
			{
				os << "Check route " << rin << " " << iter->visitClientID << " load time WRONG: "
					<< *it << " (" << vr.orderVec[vr.orderMap.at(*it)]
					<< ") ready time is not ahead of depature time: " << iter->departureTime << endl;
				system("pause");
				return false;
			}
			if (vr.orderVec[vr.orderMap.at(*it)].getApplierID() != iter->visitClientID)
			{
				os << "Check route " << rin << " " << iter->visitClientID << " load WRONG: "
					<< *it << " (" << vr.orderVec[vr.orderMap.at(*it)] << ")\n";
				system("pause");
				return false;
			}
			real_cur_quantity += vr.orderVec[vr.orderMap.at(*it)].getQuantity();
		}
		for (vector<OrderID>::const_iterator it = (*iter).unloadOrderID.begin();
			it != (*iter).unloadOrderID.end(); it++)
		{
			if (handle_oid_set.count(*it) == 0)
			{
				os << "Check route " << rin << " " << iter->visitClientID << " WRONG order not load "
					<< *it << endl;
				system("pause");
			}
			handle_oid_set.erase(*it);
			if (!iter->arrivalTime.isAhead(vr.orderVec[vr.orderMap.at(*it)].getDueTime()))
			{
				os << "Check route " << rin << " " << iter->visitClientID << " unload time WRONG: "
					<< *it << " (" << vr.orderVec[vr.orderMap.at(*it)]
					<< ") arrival time is not ahead of due time of the order: " << iter->arrivalTime << endl;
				system("pause");
				return false;
			}
			if (vr.orderVec[vr.orderMap.at(*it)].getRequestID()!=iter->visitClientID)
			{
				os << "Check route " << rin << " " << iter->visitClientID << " unload WRONG : "
					<< *it << " (" << vr.orderVec[vr.orderMap.at(*it)] << ")\n";
				system("pause");
				return false;
			}
			real_cur_quantity -= vr.orderVec[vr.orderMap.at(*it)].getQuantity();
		}
		if (real_cur_quantity != iter->currentQuantity)
		{
			os << real_cur_quantity << "!=" << iter->currentQuantity << endl;
			system("pause");
			return false;
		}
		if (real_cur_quantity >vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].capacity)
		{
			os << "Check route " << rin << " " << iter->visitClientID << " unload WRONG : "
				<< real_cur_quantity << " exceeds capacity " 
				<< vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].capacity << endl;
			system("pause");
			return false;
		}
	}
	if (handle_oid_set.size() != 0)
	{
		os << "Check route " << rin << " WRONG, order unmatched ";
		system("pause");
		return false;
	}
	os << endl << "Check route " << rin << " is RIGHT" << endl;
	return true;
}

bool Solver::checkClientInRoute(const Solution &solution, const int &rin, const vector<ClientID> &cid_vec)const
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
void Solver::calculateObjValue(Solution &solution, const int &rin)
{
	solution.routeVec[rin].routeObject = 0;
	solution.routeVec[rin].routeWeightDistance = 0;
	solution.routeVec[rin].serveMandaOrderCnt = 0;
	solution.routeVec[rin].servOptionalOrderCnt = 0;
	solution.routeVec[rin].routeDistance = solution.routeVec[rin].serveClientList.back().alreadyDriveDistance;
	for (list<ServeClient>::const_iterator sc_iter = solution.routeVec[rin].serveClientList.begin();
		sc_iter != solution.routeVec[rin].serveClientList.end(); sc_iter++)
	{
		for (vector<OrderID>::const_iterator oid_unload_iter = sc_iter->unloadOrderID.begin();
			oid_unload_iter != sc_iter->unloadOrderID.end(); oid_unload_iter++)
		{
			solution.routeVec[rin].routeObject += vr.orderVec[vr.orderMap.at(*oid_unload_iter)].getOrderValue();
			if (vr.orderVec[vr.orderMap.at(*oid_unload_iter)].getOrderType() == OrderType::Mandatory)
				solution.routeVec[rin].serveMandaOrderCnt += 1;
			else
				solution.routeVec[rin].servOptionalOrderCnt += 1;
		}
		if (sc_iter != solution.routeVec[rin].serveClientList.begin())
		{
			list<ServeClient>::const_iterator prior_iter = sc_iter;
			prior_iter--;
			DistanceType temp_dis = vr.edgeVec[vr.orderEdge[vr.clientMap.at(prior_iter->visitClientID)]
				[vr.clientMap.at(sc_iter->visitClientID)]].getDistance();
			//solution.routeVec[rin].routeDistance += temp_dis;			
			solution.routeVec[rin].routeWeightDistance += prior_iter->currentQuantity * temp_dis;
		}
	}
	if (solution.routeVec[rin].routeDistance == 0)
		solution.routeVec[rin].fullLoadRate = 0;
	else
		solution.routeVec[rin].fullLoadRate = solution.routeVec[rin].routeWeightDistance / (solution.routeVec[rin].routeDistance *
		vr.vehicleVec[vr.vehicleMap.at(solution.routeVec[rin].VehID)].capacity);
	os << "route " << rin << " obj: "
		<< solution.routeVec[rin].routeObject << "\t" << solution.routeVec[rin].routeDistance << "\t"
		<< solution.routeVec[rin].routeWeightDistance << "\t" << solution.routeVec[rin].fullLoadRate << endl;
}
void Solver::calculateTotalObjValue(Solution &solution)
{
	solution.reset();
	for (vector<Route>::iterator iter = solution.routeVec.begin();
		iter != solution.routeVec.end();iter++)
	{
		solution.totalDistance += iter->routeDistance;
		solution.totalWeightDistance += iter->routeWeightDistance;
		solution.totalObject += iter->routeObject;
		solution.averagefullLoadRate += iter->fullLoadRate;
		solution.serveMandaOrderCnt += iter->serveMandaOrderCnt;
		solution.servOptionalOrderCnt += iter->servOptionalOrderCnt;
	}
	solution.averagefullLoadRate /= solution.routeVec.size();
}