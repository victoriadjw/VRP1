#ifndef OUTPUT_H
#define OUTPUT_H
#include"VehicleRouting.h"
class Output
{
public:
	
	Output(ostream &opf, const VehicleRouting &_vr) :ofs(opf), vr(_vr)
	{
	}
	~Output(){ /*ofs.close();*/}
	
private:
	ostream &ofs;
	const VehicleRouting &vr;
};
#endif