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
	ifs >> strTemp >> strTemp;
	
	getline(ifs, strTemp);
	getline(ifs, strTemp);
	getline(ifs, strTemp);

	char buffer[BUFF_SIZE];
	ifs.getline(buffer, BUFF_SIZE, ':');
	//ifs >> num_client;
	getline(ifs, strTemp);

	ifs.getline(buffer, BUFF_SIZE, ':');
	//ifs >> num_order;
	getline(ifs, strTemp);

	ifs.getline(buffer, BUFF_SIZE, ':');
	//ifs >> num_vehicle;
	getline(ifs, strTemp);

	ifs.getline(buffer, BUFF_SIZE, ':');
	//ifs >> num_carrier;
	getline(ifs, strTemp);

	ifs.getline(buffer, BUFF_SIZE, ':');
	//ifs >> num_billing;
	getline(ifs, strTemp);

	ifs.getline(buffer, BUFF_SIZE, ':');
	//ifs >> num_region;
	getline(ifs, strTemp);

	getline(ifs, strTemp);  // EDGE_WEIGHT_TYPE
	getline(ifs, strTemp);  // EDGE_WEIGHT_FORMAT

	ifs.getline(buffer, BUFF_SIZE, ':');
	//ifs >> plan_horizon.first >> strTemp >> plan_horizon.second;
	getline(ifs, strTemp);


}
Input::~Input()
{
	ofs.close();
	ifs.close();
	cout << "deconstruct Input object, ofs and ifs." << endl;
}