#include"Input.h"

void Input::printGraph(const VehicleRouting &vr)
{
	cout << endl << "vertexNum: " << vr.clientVec.size()<< ", edgeNum: " << vr.edgeVec.size() << endl;
	for (vector<Client>::const_iterator iter = vr.clientVec.begin(); iter != vr.clientVec.end(); iter++)
	{
		cout << "the adjacent vertex of node " << (*iter).PriDCID << ":" << (*iter).adjEdgeVec.size() << endl;
		for (vector<int>::const_iterator iter_e = (*iter).adjEdgeVec.begin();
			iter_e != (*iter).adjEdgeVec.end(); iter_e++)
		{
			cout << " -> " << vr.edgeVec[*iter_e] << endl;
		}
		cout << endl;
	}
}


void Input::errorLog(const string &msg)
{
	ofs << Timer::getCurrentTime() + " : " + msg;
}

Input::Input(const string &inputFileName, const string &errorLogFileName, VehicleRouting &vr)
{
	ofs.open(errorLogFileName, std::ofstream::out | std::ofstream::app);
	if (!ofs.is_open())
	{
		cerr << "fail to open file : " + errorLogFileName;
		return;
	}
	ifs.open(inputFileName, std::ifstream::in);
	if (!ifs.is_open())
	{
		errorLog("fail to open file : " + inputFileName);
		return;
	}
	string strTemp;
	char buffer[BUFF_SIZE];
	int num,num1;

	ifs.getline(buffer, BUFF_SIZE, ':');
	ifs >> strTemp;
	vr.setName(strTemp);
	getline(ifs, strTemp);

	getline(ifs, strTemp);
	getline(ifs, strTemp);

	ifs.getline(buffer, BUFF_SIZE, ':');
	ifs >> num;
	vr.setNumClient(num);
	getline(ifs, strTemp);

	ifs.getline(buffer, BUFF_SIZE, ':');
	ifs >> num;
	vr.setNumOrder(num);
	getline(ifs, strTemp);

	ifs.getline(buffer, BUFF_SIZE, ':');
	ifs >> num;
	vr.setNumVehicle(num);
	getline(ifs, strTemp);

	ifs.getline(buffer, BUFF_SIZE, ':');
	ifs >> num;
	vr.setNumCarrier(num);
	getline(ifs, strTemp);

	ifs.getline(buffer, BUFF_SIZE, ':');
	ifs >> num;
	vr.setNumBilling(num);
	getline(ifs, strTemp);

	ifs.getline(buffer, BUFF_SIZE, ':');
	ifs >> num;
	vr.setNumRegion(num);
	getline(ifs, strTemp);

	getline(ifs, strTemp);  // EDGE_WEIGHT_TYPE
	getline(ifs, strTemp);  // EDGE_WEIGHT_FORMAT

	ifs.getline(buffer, BUFF_SIZE, ':');
	ifs >> num >> strTemp >> num1;
	vr.setPlanHorizon(num, num1);
	getline(ifs, strTemp);
	//readDataSection(vr);
	readDataSectionProject(vr);

}
void Input::readDataSection(VehicleRouting &vr)
{
	string temp;
	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	Client *vi=new Client;
	vi->nameOfDeliveryCenter = temp;

	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	// REGIONS
	string pattern = " ";
	vector<string> result = split(temp, pattern);
	for (int i = 0; i < result.size(); i++)
	{
		cout << result[i] << ", ";
		vi = new Client;
		vi->nameOfDeliveryCenter = result[i];
		vr.regionMap[result[i]] = i;
	}
	//BILLINGS
	for (int i = 0; i < 18; i++)
		getline(ifs, temp);
	//CARRIERS
	string crid, bid;
	for (int i = 0; i < vr.getNumCarrier(); i++)
	{
		int numIncomRegion;
		ifs >> crid >> bid >> numIncomRegion;
		Carrier cr(crid, bid, numIncomRegion);
		string regionID;
		for (int j = 0; j < numIncomRegion; j++)
		{
			ifs >> regionID;
			cr.addIncompatRegion(regionID);
		}
		vr.carrierVec.push_back(cr);
		vr.carrierMap[crid] = i;
	}
	//VEHICLES
	ifs >> temp >> temp;
	string vid;
	CapacityType cap;
	CostType cost;
	for (int i = 0; i < vr.getNumVehicle(); i++)
	{
		ifs >> vid >> cap >> cost >> crid;
		Vehicle ve(vid, crid, cost, cap);
		vr.vehicleVec.push_back(ve);
		vr.vehicleMap[vid] = i;
		cout << vid << " " << cap << " " << cost << " " << crid << endl;
	}
	//CLIENTS
	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	vr.clientVec.resize(vr.getNumClient());
	for (int i = 0; i < vr.getNumClient(); i++)
	{
		ifs >> vr.clientVec[i];
		vr.clientMap[vr.clientVec[i].regionID] = i;
		cout << vr.clientVec[i].PriDCID << endl;
	}
	//ORDERS
	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	vr.orderVec.resize(vr.getNumOrder());
	for (int i = 0; i < vr.getNumOrder(); i++)
	{
		ifs >> vr.orderVec[i];
		vr.orderVec[i].setApplierID("c0");	// the applier id is depot c0
		vr.orderMap[vr.orderVec[i].getID()] = i;
		cout << vr.orderVec[i].getID() << endl;
	}
	//EDGES
	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	vr.orderEdge.resize(vr.clientVec.size(), vector<int>(vr.clientVec.size(), -1));
	while (getline(ifs, temp) && temp != "END")
	{
		Edge e;
		ifs >> e;
		vr.edgeVec.push_back(e);
		// add the index of e in edgeVec to adjEdgeVec of clientVec
		vr.clientVec[vr.clientMap[e.getEdge().first]].adjEdgeVec.push_back(vr.edgeVec.size() - 1);
		vr.orderEdge[vr.clientMap[e.getEdge().first]][vr.clientMap[e.getEdge().second]] = vr.edgeVec.size() - 1;
	}
	printGraph(vr);
	//vr.dsp = new DijkstraShortPath(vr.clientVec, vr.edgeVec);
}

void Input::readDataSectionProject(VehicleRouting &vr)
{
	string temp;
	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	Client *vi = new Client;
	vi->nameOfDeliveryCenter = temp;

	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	// REGIONS
	string pattern = " ";
	vector<string> result = split(temp, pattern);
	for (int i = 0; i < result.size(); i++)
	{
		cout << result[i] << ", ";
		vi = new Client;
		vi->nameOfDeliveryCenter = result[i];
		vr.regionMap[result[i]] = i;
	}
	//BILLINGS
	for (int i = 0; i < 18; i++)
		getline(ifs, temp);
	//CARRIERS
	string crid, bid;
	for (int i = 0; i < vr.getNumCarrier(); i++)
	{
		int numIncomRegion;
		ifs >> crid >> bid >> numIncomRegion;
		Carrier cr(crid, bid, numIncomRegion);
		string regionID;
		for (int j = 0; j < numIncomRegion; j++)
		{
			ifs >> regionID;
			cr.addIncompatRegion(regionID);
		}
		vr.carrierVec.push_back(cr);
		vr.carrierMap[crid] = i;
	}
	//VEHICLES
	ifs >> temp >> temp;
	vr.vehicleVec.resize(vr.getNumVehicle());
	for (int i = 0; i < vr.getNumVehicle(); i++)
	{
		ifs >> vr.vehicleVec[i];
		vr.vehicleMap[vr.vehicleVec[i].VehID] = i;
	}
	//CLIENTS
	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	vr.clientVec.resize(vr.getNumClient());
	for (int i = 0; i < vr.getNumClient(); i++)
	{
		ifs >> vr.clientVec[i];
		vr.clientMap[vr.clientVec[i].PriDCID] = i;
		vr.clientNameMap[vr.clientVec[i].clientName] = vr.clientVec[i].PriDCID;
		cout << vr.clientVec[i].regionID << endl;
	}
	//ORDERS
	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	vr.orderVec.resize(vr.getNumOrder());
	for (int i = 0; i < vr.getNumOrder(); i++)
	{
		ifs >> vr.orderVec[i];
		vr.orderVec[i].setApplierID("c0");	// the applier id is depot c0
		vr.orderMap[vr.orderVec[i].getID()] = i;
		cout << vr.orderVec[i].getID() << endl;
	}
	//EDGES
	getline(ifs, temp);
	getline(ifs, temp);
	//getline(ifs, temp);
	vr.orderEdge.resize(vr.clientVec.size(), vector<int>(vr.clientVec.size(), -1));
	while (getline(ifs, temp) && temp != "END")
	{
		Edge e;
		ifs >> e;
		if (e.getEdge().first == "END")
			break;
		e.setDistance(calculateDistance(
			vr.clientVec[vr.clientMap.at(vr.clientNameMap.at(e.getEdge().first))].latitude,
			vr.clientVec[vr.clientMap.at(vr.clientNameMap.at(e.getEdge().first))].longitude,
			vr.clientVec[vr.clientMap.at(vr.clientNameMap.at(e.getEdge().second))].latitude,
			vr.clientVec[vr.clientMap.at(vr.clientNameMap.at(e.getEdge().second))].longitude));
		vr.edgeVec.push_back(e);
		// add the index of e in edgeVec to adjEdgeVec of clientVec
		vr.clientVec[vr.clientMap.at(vr.clientNameMap.at(e.getEdge().first))].adjEdgeVec.push_back(vr.edgeVec.size() - 1);
		vr.orderEdge[vr.clientMap.at(vr.clientNameMap.at(e.getEdge().first))]
			[vr.clientMap.at(vr.clientNameMap.at(e.getEdge().second))] = vr.edgeVec.size() - 1;
		//ifs >> temp;
	}
	vr.serveTimeDuration = Timer::Duration(2);	// 2 hours of service time
	printGraph(vr);
	//vr.dsp = new DijkstraShortPath(vr.clientVec, vr.edgeVec);
}
Input::~Input()
{
	ofs.close();
	ifs.close();
	cout << "deconstruct Input object, ofs and ifs." << endl;
}