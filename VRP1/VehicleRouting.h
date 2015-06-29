/*
*		usage: 1. contain data for identifying a Vehicle Routing Problem (VRP).
*			   2. provid solver interface.
*/

#ifndef VEHICLEROUTING_H
#define VEHICLEROUTING_H

#include<map>
#include<vector>
#include<list>
#include<string>
#include<sstream>
#include<iostream>


using namespace std;
class VehicleRouting
{
public:
	typedef int IterCount;	// iteration count for meta-heuristic solver
	typedef int ObjValue;	// unit for objective value
	typedef int ArcInfoType;	// information type of arc
	typedef int VertexType;		// information type of vertex
	
	// graph kind, DG: directed graph; WDG: weighted directed graph;
	// UDG: undirected graph; WUDG: weighted undirected graph
	enum GraphKind{DG,WDG,UDG,WUDG};
	class Graph
	{
	public:
		Graph(GraphKind _kind) :graphKind(_kind)
		{}
		Graph(int _verNum, GraphKind _kind) :vertexNum(_verNum), arcNum(0), graphKind(_kind)
		{
			//vertices = new Vertex[vertexNum];
			for (int i = 0; i < vertexNum; i++)
			{
				Vertex v;
				v.firstArc = NULL;
				vertices.push_back(v);
				//vertices[i].firstArc = NULL;
			}
		}
		void createGraph()
		{
			switch (graphKind)
			{
			case DG:{createDG(); break; }
			case WDG:{createWDG(); break; }
			case UDG:{createUDG(); break; }
			case WUDG:{createWUDG(); break; }
			default:
				break;
			}
		}
		void printGraph()
		{
			for (int i = 0; i < vertexNum; i++)
			{
				cout << "the adjacent vertex of node " << vertices[i].data << ":";
				ArcNode *p = vertices[i].firstArc;
				while (p != NULL)
				{
					cout << " -> " << vertices[p->adjVertex].data
						<< "(" << p->arcInfo << ")";
					p = p->nextArc;
				}
				cout << endl;
			}
		}
	private:
		void initVertices()
		{
			cout << "input the keyword of each vertex:" << endl;
			VertexType val;
			while (cin >> val)
			{
				Vertex v;
				v.data = val;
				v.firstArc = NULL;
				vertices.push_back(v);
				vertexNum += 1;
			}
		}
		void insertArc(int vHead, int vTail, ArcInfoType arcInfo)
		{
			ArcNode *an=new ArcNode;
			an->adjVertex = vTail;
			an->nextArc = NULL;
			an->arcInfo = arcInfo;

			ArcNode *p = vertices[vHead].firstArc;
			if (p == NULL)
			{
				vertices[vHead].firstArc = an;
			}
			else
			{
				while (p->nextArc != NULL)
					p = p->nextArc;
				p->nextArc = an;
			}
			arcNum += 1;
		}
		void createDG()
		{
			initVertices();
			int vHead, vTail;
			cout << "input the starting and ending vertex of each arc:" << endl;
			cin.clear();
			while (cin >> vHead >> vTail)
			{
				insertArc(vHead, vTail, 0);
			}
		}
		void createWDG()
		{
			initVertices();
			int vHead, vTail;
			ArcInfoType w;
			cout << "input the starting and ending vertex, weight of each arc:" << endl;
			cin.clear();
			while (cin >> vHead >> vTail >> w)
			{
				insertArc(vHead, vTail, w);
			}
		}
		void createUDG()
		{
			initVertices();
			int vHead, vTail;
			cout << "input the starting and ending vertex of each arc:" << endl;
			cin.clear();
			while (cin >> vHead >> vTail)
			{
				insertArc(vHead, vTail, 0);
				insertArc(vTail, vHead, 0);
			}
		}
		void createWUDG()
		{
			initVertices();
			int vHead, vTail;
			ArcInfoType w;
			cout << "input the starting and ending vertex ,weight of each arc:" << endl;
			cin.clear();
			while (cin >> vHead >> vTail >> w)
			{
				insertArc(vHead, vTail, w);
				insertArc(vTail, vHead, w);
			}

		}
		struct ArcNode
		{
			int adjVertex;		// adjecent vertex of arc
			ArcNode *nextArc;	// next arc
			ArcInfoType arcInfo;	// information of arc
		};
		struct Vertex
		{
			VertexType data;	// information of vertex
			ArcNode *firstArc;	// first arc adjacent to the vertex
		};
		vector<Vertex> vertices;
		int vertexNum;	// number of vertex
		int arcNum;		// number of arc
		GraphKind graphKind;	// kind of graph
	};
	class Scenario;
	class Order;
	class Vehilce;

};
#endif