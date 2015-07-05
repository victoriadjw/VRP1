#include"Input.h"

void Input::printGraph(const VehicleRouting &vr)
{
	cout << endl << "vertexNum: " << vr.vertexVec.size()<< ", edgeNum: " << vr.edgeVec.size() << endl;
	for (vector<VertexInfo>::const_iterator iter = vr.vertexVec.begin(); iter != vr.vertexVec.end(); iter++)
	{
		cout << "the adjacent vertex of node " << (*iter).id<< ":" <<(*iter).adjEdgeVec.size();
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
	readDataSection(vr);

}
void Input::readDataSection(VehicleRouting &vr)
{
	string temp;
	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	VertexInfo *vi=new VertexInfo;
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
		vi = new VertexInfo;
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
		Vehicle ve(0, vid, crid, cost, cap);
		vr.vehicleVec.push_back(ve);
		cout << vid << " " << cap << " " << cost << " " << crid << endl;
	}
	//CLIENTS
	getline(ifs, temp);
	getline(ifs, temp);
	getline(ifs, temp);
	vr.vertexVec.resize(vr.getNumClient());
	for (int i = 0; i < vr.getNumClient(); i++)
	{
		ifs >> vr.vertexVec[i];
		vr.vertexMap[vr.vertexVec[i].id] = i;
		cout << vr.vertexVec[i].id << endl;
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
	while (getline(ifs, temp) && temp != "END")
	{
		EdgeInfo e;
		ifs >> e;
		vr.edgeVec.push_back(e);
		vr.vertexVec[vr.vertexMap[e.getEdge().first]].adjEdgeVec.push_back(vr.edgeVec.size() - 1);
	}
	//printGraph(vr);
}
Input::~Input()
{
	ofs.close();
	ifs.close();
	cout << "deconstruct Input object, ofs and ifs." << endl;
}