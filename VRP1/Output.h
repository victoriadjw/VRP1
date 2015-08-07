#ifndef OUTPUT_H
#define OUTPUT_H
#include"VehicleRouting.h"
class Output
{
public:
	Output(const string &opf,const VehicleRouting &_vr):vr(_vr)
	{
		ofs.open(opf, ios::app);
	}
	~Output(){ ofs.close();}
	ofstream ofs;
private:
	const VehicleRouting &vr;
};
#endif