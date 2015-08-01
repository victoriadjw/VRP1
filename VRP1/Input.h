#ifndef INPUT_H
#define INPUT_H

#include"VehicleRouting.h"

class Input
{
public:
	Input(const string &_inputFileName, const string &_errorLogFileName, VehicleRouting &vr);
	~Input();
	bool readGraph();
	void printGraph(const VehicleRouting &);
private:
	void errorLog(const string &msg);
	void readDataSection(VehicleRouting &);
	void readDataSectionProject(VehicleRouting &);
	ifstream ifs;
	ofstream ofs;
	Timer currentTime;
};

#endif