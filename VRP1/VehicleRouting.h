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
//template<class VertexInfo,class ArcInfo>
class VehicleRouting
{
public:
	typedef int IterCount;	// iteration count for meta-heuristic solver
	typedef int ObjValue;	// unit for objective value
	typedef int ArcInfoType;	// information type of arc
	typedef int VertexType;		// information type of vertex

	// graph kind, DG: directed graph; WDG: weighted directed graph;
	// UDG: undirected graph; WUDG: weighted undirected graph
	enum GraphKind{ DG, WDG, UDG, WUDG };
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
				//Vertex v;
				//v.firstArc = NULL;
				//vertices.push_back(v);
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
			for (vector<Vertex<VertexInfo>>::iterator iter=vertices.begin();iter!=vertices.end();iter++)
			{
				cout << "the adjacent vertex of node " << (*iter).vertexInfo.id<< ":";
				for (list<ArcNode<ArcInfo>>::iterator iter_list = (*iter).firstArc.begin(); iter_list != (*iter).firstArc.end(); iter_list++)
				{
					cout << " -> " << (*iter_list).adjVertex
						<< "(" << (*iter_list).arcInfo.weight << ")";
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
				Vertex<VertexInfo> v;
				v.vertexInfo.id = val;
				vertices.push_back(v);
				vertexNum += 1;
			}
		}
		void insertArc(int vHead, int vTail, ArcInfoType arcInfo)
		{
			ArcNode<ArcInfo> an;
			an.adjVertex = vTail;
			an.arcInfo.weight = arcInfo;
			vertices[vHead].firstArc.push_back(an);
			
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
		class ArcInfo
		{
		public:
			int weight;
		};
		class VertexInfo
		{
		public:
			int id;
			string name;
		};
		template <class ArcInfo>
		class ArcNode
		{
		public:
			int adjVertex;		// adjecent vertex of arc
			ArcInfo arcInfo;	// information of arc
		};
		template <class VertexInfo>
		class Vertex
		{
		public:
			VertexInfo vertexInfo;	// information of vertex
			list<ArcNode<ArcInfo>> firstArc;	// first arc adjacent to the vertex
		};
		vector<Vertex<VertexInfo>> vertices;
		int vertexNum;	// number of vertex
		int arcNum;		// number of arc
		GraphKind graphKind;	// kind of graph
	};
	class Scenario;
	class Order;
	class Vehilce;

};
#endif