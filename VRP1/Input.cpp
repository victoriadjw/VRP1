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
	readDataSectionProject(vr);

}
void Input::readDataSection(VehicleRouting &vr)
{
}

void Input::readDataSectionProject(VehicleRouting &vr)
{
	string line;
	getline(ifs, line);
	getline(ifs, line);
	getline(ifs, line);
	//CLIENTS
	vr.clientVec.resize(vr.getNumClient());
	for (int i = 0; getline(ifs, line) && line != "END"; i++)
	{
		cout << line << endl;
		std::stringstream word(line);
		Client c;
		word >> c;
		
		vr.clientMap[c.PriDCID] = i;
		vr.clientNameMap[c.clientName] = c.PriDCID;
		vr.clientVec.push_back(c);
		//cout << c<< endl;
	}	
	//EDGES
	vr.orderEdge.resize(vr.clientVec.size(), vector<int>(vr.clientVec.size(), -1));
	getline(ifs, line);
	getline(ifs, line);
	while (getline(ifs, line) && line != "END")
	{
		cout << line << endl;
		std::stringstream word(line);
		Edge e;
		word >> e;
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
	vr.setNumClient(vr.clientVec.size());
	int num_vehicle = 7;
	int num_order = 540;
	CapacityType cap_base = 10000;
	SpeedType spe_base = 100;
	QuantityType qua_base = 300;
	vr.setNumVehicle(num_vehicle);
	for (int i = 0; i < num_vehicle; i++)
	{
		stringstream ss;
		ss << i;
		string str_i = ss.str();
		string vid = "v";
		vid.append(str_i);
		Vehicle v = Vehicle(vid, cap_base + rand() % 3000, spe_base + rand() % 20);
		vr.vehicleVec.push_back(v);
		vr.vehicleMap[v.VehID] = i;
	}
	for (int i = 0; i < num_order; i++)
	{
		stringstream ss;
		ss << i;
		string str_i = ss.str();
		string oid = "o";
		oid.append(str_i);
		Order o = Order(oid, qua_base + rand() % 1000);
		o.setOrderValue(300 + rand() % 500);
		vr.orderVec.push_back(o);
		vr.orderMap[o.getID()] = i;
	}
	printGraph(vr);
	//vr.dsp = new DijkstraShortPath(vr.clientVec, vr.edgeVec);
}
Input::~Input()
{
	ofs.close();
	ifs.close();
	cout << "deconstruct Input object, ofs and ifs." << endl;
}