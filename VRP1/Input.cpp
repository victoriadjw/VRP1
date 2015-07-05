#include"Input.h"

bool Input::readGraph(Graph<VertexInfo, EdgeInfo> &g)
{
	cout << "ifs the keyword of each vertex:" << endl;
	int id;
	while (cin >> id)
	{
		VertexInfo *vi = new VertexInfo;
		vi->id = id;
		g.addVertex(vi);
	}
	cin.clear();
	cout << "ifs the starting and ending vertex of each edge:" << endl;
	int vHead, vTail, weight;
	while (cin >> vHead >> vTail >> weight)
	{
		EdgeInfo *ai = new EdgeInfo;
		ai->distance = weight;
		g.insertEdge(vHead, vTail, ai);
		g.insertEdge(vTail, vHead, ai);
	}
	return true;
}
void Input::printGraph(const Graph<VertexInfo, EdgeInfo> &g)
{
	cout << endl << "vertexNum: " << g.vertexNum << ", edgeNum: " << g.edgeNum << endl;
	for (vector<Graph<VertexInfo, EdgeInfo>::Vertex *>::const_iterator iter = g.vertices.begin(); iter != g.vertices.end(); iter++)
	{
		cout << "the adjacent vertex of node " << (*iter)->vertexInfo->id << ":";
		for (list<Graph<VertexInfo, EdgeInfo>::EdgeNode *>::const_iterator iter_list = (*iter)->firstEdge.begin();
			iter_list != (*iter)->firstEdge.end(); iter_list++)
		{
			cout << " -> " << (*iter_list)->adjVertex
				<< "(" << (*iter_list)->edgeInfo->distance << ")";
		}
		cout << endl;
	}
	cout << "vertex num: " << g.vertexNum << " " << g.vertices.size() << endl
		<< "edge num: " << (g.edgeNum >> 1);
}

void Input::calculateDistance(Graph<VertexInfo, EdgeInfo> &g)
{
	for (vector<Graph<VertexInfo, EdgeInfo>::Vertex *>::const_iterator iter = g.vertices.begin(); iter != g.vertices.end(); iter++)
	{
		for (list<Graph<VertexInfo, EdgeInfo>::EdgeNode *>::const_iterator iter_list = (*iter)->firstEdge.begin();
			iter_list != (*iter)->firstEdge.end(); iter_list++)
		{
			(*iter_list)->edgeInfo->distance = getDistance((*iter)->vertexInfo->latitude, (*iter)->vertexInfo->longitude,
				(*iter)->vertexInfo->latitude, (*iter)->vertexInfo->longitude);
		}
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
	vr.g.addVertex(vi);

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
		vr.g.addVertex(vi);
		vr.regionMap[result[i]] = i;
	}
	printGraph(vr.g);
	getline(ifs, temp);
	//BILLINGS
	for (int i = 0; i < 16; i++)
		getline(ifs, temp);
	//CARRIERS
	getline(ifs, temp);
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
	vr.clientVec.resize(vr.getNumClient());
	for (int i = 0; i < vr.getNumClient(); i++)
	{
		ifs >> vr.clientVec[i];
		vr.clientMap[vr.clientVec[i].getID()] = i;
		cout << vr.clientVec[i].getID() << endl;
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
		cout << temp << endl;
	}

}
Input::~Input()
{
	ofs.close();
	ifs.close();
	cout << "deconstruct Input object, ofs and ifs." << endl;
}