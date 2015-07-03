#ifndef INPUT_H
#define INPUT_H

#include"VehicleRouting.h"

class Input
{
public:
	Input(const string &_inputFileName, const string &_errorLogFileName, VehicleRouting &vr);
	~Input();
	bool readGraph(Graph<VertexInfo, EdgeInfo> &);
	void printGraph(const Graph<VertexInfo, EdgeInfo> &);
private:
	void errorLog(const string &msg);
	void calculateDistance(Graph<VertexInfo, EdgeInfo> &);
	ifstream ifs;
	ofstream ofs;
	Timer currentTime;
};

#endif