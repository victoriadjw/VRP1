#include "VehicleRouting.h"
#include<algorithm>

void VehicleRouting::setName(const string &_name){ name = _name; }
void VehicleRouting::setNumBilling(const int &_numBilling){	numBilling = _numBilling;}
void VehicleRouting::setNumCarrier(const int &_numCarrier){	numCarrier = _numCarrier;}
void VehicleRouting::setNumClient(const int &_numClient){ numClient = _numClient;}
void VehicleRouting::setNumOrder(const int &_numOrder){	numOrder = _numOrder;}
void VehicleRouting::setNumVehicle(const int &_numVehicle){	numVehicle = _numVehicle;}
void VehicleRouting::setNumRegion(const int &_numRegion){ numRegion = _numRegion;}
void VehicleRouting::setPlanHorizon(const int &start, const int &end)
{
	CyclePlan .first = start;
	CyclePlan .second = end;
}

int VehicleRouting::getNumClient()const{ return numClient; }
int VehicleRouting::getNumOrder()const{ return numOrder; }
int VehicleRouting::getNumVehicle()const{ return numVehicle; }
int VehicleRouting::getNumCarrier()const{ return numCarrier; }
int VehicleRouting::getNumBilling()const{ return numBilling; }
int VehicleRouting::getNumRegion()const{ return numRegion; }

void VehicleRouting::calculateDistance()
{
	for (vector<Edge>::iterator iter = edgeVec.begin(); iter != edgeVec.end(); iter++)
	{
		(*iter).setDistance(getDistance(clientVec[clientMap[(*iter).getEdge().first]].latitude,
			clientVec[clientMap[(*iter).getEdge().first]].longitude,
			clientVec[clientMap[(*iter).getEdge().second]].latitude,
			clientVec[clientMap[(*iter).getEdge().second]].longitude));
	}
}
class VehicleRouting::CmpDistance{
public:
	CmpDistance(const vector<Edge> &_edgeVec, const vector<Order> &_orderVec,
		map<ClientID, int> &_clientMap,
		const vector<vector<int>> &_orderEdge)
		:edgeVec(_edgeVec), orderVec(_orderVec),
		clientMap(_clientMap),orderEdge(_orderEdge){}
	// sort to (least...greatest)
	bool operator()(const int &l, const int &r)
	{
		int le = orderEdge[clientMap[orderVec[l].getApplierID()]][clientMap[orderVec[l].getRequestID()]];
		int re = orderEdge[clientMap[orderVec[r].getApplierID()]][clientMap[orderVec[r].getRequestID()]];
		return edgeVec[le].getDistance()<edgeVec[re].getDistance();
	}
private:
	const vector<Edge> &edgeVec;
	const vector<Order> &orderVec;
	map<ClientID, int> &clientMap;
	const vector<vector<int>> &orderEdge;
};

void VehicleRouting::modifyOrder()
{
	for (vector<Order>::iterator iter = orderVec.begin(); iter != orderVec.end(); iter++)
	{
		if ((*iter).getOrderType() == OrderType::Optional)
		{
			if (rand() % 4 == 0) // change the order to mandatory with probability of 1/6
			{
				(*iter).setOrderType(OrderType::Mandatory);
			}
			else
			{
				int vindex = rand() % clientVec.size();
				while (vindex == 0 || clientVec[vindex].PriDCID == (*iter).getRequestID())
					vindex = rand() % clientVec.size();
				(*iter).setApplierID(clientVec[vindex].PriDCID);
			}
		}
		cout << *iter << endl;
	}
}

DistanceType VehicleRouting::getDistanceByOrderIndex(const OrderID &oin)const
{
	ClientID &apid = orderVec[orderMap[oin]].getApplierID();
	ClientID &rqid = orderVec[orderMap[oin]].getRequestID();

	for (std::vector<Edge>::const_iterator iter = edgeVec.begin(); iter != edgeVec.end(); iter++)
	{
		if ((*iter).getEdge().first == apid && (*iter).getEdge().second == rqid)
			return (*iter).getDistance();
	}
	return -1;
}

void VehicleRouting::setMandOptionOrder()
{

	cout << "mandatory order:" << endl;
	int i = 0;
	for (std::vector<Order>::iterator iter = orderVec.begin(); iter != orderVec.end(); iter++, i++)
	{
		if ((*iter).getOrderType() == OrderType::Mandatory)
		{
			cout << i << " : " << *iter << endl;
			mandatoryOrderIndexVec.push_back(i);
		}
		else
		{
			optionalOrderIndexVec.push_back(i);
		}
	}
	std::sort(mandatoryOrderIndexVec.begin(), mandatoryOrderIndexVec.end(), CmpDistance(edgeVec, orderVec, clientMap, orderEdge));
	sort(optionalOrderIndexVec.begin(), optionalOrderIndexVec.end(), CmpDistance(edgeVec, orderVec, clientMap, orderEdge));
	cout << "sorted mandatory order:" << mandatoryOrderIndexVec.size() << endl;
	CmpDistance cd(edgeVec, orderVec, clientMap, orderEdge);
	for (std::vector<int>::iterator iter = mandatoryOrderIndexVec.begin(); iter != mandatoryOrderIndexVec.end(); iter++)
	{
	//	cout << orderVec[*iter] << "\t" << *iter << "\t" << cd.getDistanceByOrderIndex(*iter) << endl;
	}
	cout << "sorted optional order:" << optionalOrderIndexVec.size() << endl;
	for (std::vector<int>::iterator iter = optionalOrderIndexVec.begin(); iter != optionalOrderIndexVec.end(); iter++)
	{
	//	cout << orderVec[*iter] << "\t" << *iter << "\t" << cd.getDistanceByOrderIndex(*iter) << endl;
	}
}
vector<int> VehicleRouting::getMandatoryOrderIndexVec()const
{
	return mandatoryOrderIndexVec;
}
vector<int> VehicleRouting::getOptionalOrderIndexVec()const
{
	return optionalOrderIndexVec;
}
