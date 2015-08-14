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
int VehicleRouting::getNumMandaOrder()const{ return numMandaOrder; }
int VehicleRouting::getNumOptionalOrder()const{ return numOptionalOrder; }

class VehicleRouting::CmpDistance{
public:
	CmpDistance(const vector<Edge> &_edgeVec, const vector<Order> &_orderVec,
		const map<ClientID, int> &_clientMap,
		const vector<vector<int>> &_orderEdge)
		:edgeVec(_edgeVec), orderVec(_orderVec),
		clientMap(_clientMap), orderEdge(_orderEdge){}
	// sort to (least...greatest)
	bool operator()(const int &l, const int &r)
	{
		int le = orderEdge[clientMap.at(orderVec[l].getApplierID())][clientMap.at(orderVec[l].getRequestID())];
		int re = orderEdge[clientMap.at(orderVec[r].getApplierID())][clientMap.at(orderVec[r].getRequestID())];
		return edgeVec[le].getDistance()<edgeVec[re].getDistance();
	}
private:
	const vector<Edge> &edgeVec;
	const vector<Order> &orderVec;
	const map<ClientID, int> &clientMap;
	const vector<vector<int>> &orderEdge;
};

void VehicleRouting::modifyOrder()
{
	numMandaOrder = numOptionalOrder = 0;
	for (vector<Order>::iterator iter = orderVec.begin(); iter != orderVec.end(); iter++)
	{
		if (rand() % 6 == 0) // change the order to mandatory with probability of 1/6
		{
			(*iter).setOrderType(OrderType::Mandatory);
			int rand_cid = rand() % (clientVec.size() - 1) + 1;
			iter->setApplierID(clientVec[0].PriDCID);
			iter->setResquestID(clientVec[rand_cid].PriDCID);
			numMandaOrder += 1;
		}
		else
		{
			int index_aid = rand() % (clientVec.size() - 1) + 1;
			int index_rid = rand() % (clientVec.size() - 1) + 1;
			while (index_aid == index_rid) 
				index_rid = rand() % (clientVec.size() - 1) + 1;
			iter->setOrderType(OrderType::Optional);
			iter->setApplierID(clientVec[index_aid].PriDCID);
			iter->setResquestID(clientVec[index_rid].PriDCID);
			numOptionalOrder += 1;
		}
		iter->setReadyTime(Timer());
		iter->setDueTime(Timer(Timer::Duration(24 * 3)+Timer::Duration(10), iter->getReadyTime()));
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

	//cout << "mandatory order:" << endl;
	int i = 0;
	for (std::vector<Order>::iterator iter = orderVec.begin(); iter != orderVec.end(); iter++, i++)
	{
		if ((*iter).getOrderType() == OrderType::Mandatory)
		{
			//cout << i << " : " << *iter << endl;
			mandatoryOrderIndexVec.push_back(i);
		}
		else
		{
			optionalOrderIndexVec.push_back(i);
		}
	}
	//std::sort(mandatoryOrderIndexVec.begin(), mandatoryOrderIndexVec.end(), CmpDistance(edgeVec, orderVec, clientMap, orderEdge));
	//sort(optionalOrderIndexVec.begin(), optionalOrderIndexVec.end(), CmpDistance(edgeVec, orderVec, clientMap, orderEdge));
	//cout << "sorted mandatory order:" << mandatoryOrderIndexVec.size() << endl;
	CmpDistance cd(edgeVec, orderVec, clientMap, orderEdge);
	for (std::vector<int>::iterator iter = mandatoryOrderIndexVec.begin(); iter != mandatoryOrderIndexVec.end(); iter++)
	{
	//	cout << orderVec[*iter] << "\t" << *iter << "\t" << cd.getDistanceByOrderIndex(*iter) << endl;
	}
	//cout << "sorted optional order:" << optionalOrderIndexVec.size() << endl;
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
