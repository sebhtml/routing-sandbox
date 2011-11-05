#include <stdlib.h>
#include <math.h>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <assert.h>
using namespace std;

/*
#define CONFIG_ROUTING_VERBOSITY
#define ASSERT
*/

void viewConnections(int n,map<int,set<int> >*connections){
	for(int i=0;i<n;i++){
		cout<<i<<"	"<<(*connections)[i].size()<<"	";
		for(set<int>::iterator j=(*connections)[i].begin();j!=(*connections)[i].end();j++){
			if(j!=(*connections)[i].begin())
				cout<<" ";
			cout<<*j;
		}
		cout<<endl;
	}
}

void makeConnections(int n,map<int,set<int> >*connections){

	srand(4);

	// insert self
	for(int i=0;i<n;i++)
		(*connections)[i].insert(i);

	int connectionsPerVertex=log(n)/log(2)/2;

	cout<<"vertices: "<<n<<endl;
	cout<<"connectionsPerVertex: "<<connectionsPerVertex<<endl;

	//
	for(int connectionNumber=0;connectionNumber<connectionsPerVertex;connectionNumber++){

		for(int source=0;source<n;source++){

			// add an edge bool added=false;
			bool added=false;
			while(!added){
				int destination=rand()%n;

				// if already set, find another one
				if((*connections)[source].count(destination)>0)
					continue;
			
				(*connections)[source].insert(destination);
				(*connections)[destination].insert(source);
				added=true;
			}
		}
	}
}

/**
 * Dijkstra's_algorithm
 */
void findShortestPath(int n,int source,int destination,map<int,set<int> >*connections,vector<int>*route){

	// assign tentative distances
	map<int,int> tentativeDistances;
	
	for(int i=0;i<n;i++)
		tentativeDistances[i]=9999;

	tentativeDistances[source]=0;

	map<int,int> previousVertices;

	// create a set of unvisited vertices
	set<int> unvisited;

	for(int i=0;i<n;i++)
		unvisited.insert(i);

	// create a current vertex
	int current=source;

	// create an index of distances
	map<int,set<int> > verticesWithDistance;

	for(map<int,int>::iterator i=tentativeDistances.begin();i!=tentativeDistances.end();i++){
		verticesWithDistance[i->second].insert(i->first);
	}

	while(!unvisited.empty()){
	
		// calculate the tentative distance
		// of each neighbors of the current
		for(set<int>::iterator neighbor=(*connections)[current].begin();
			neighbor!=(*connections)[current].end();neighbor++){
			int theNeighbor=*neighbor;

			// we are only interested in unvisited neighbors
			if(unvisited.count(theNeighbor)>0){
				int distance=tentativeDistances[current]+1;
				int oldDistance=tentativeDistances[theNeighbor];

				// the new distance is better
				if(distance < oldDistance){
					tentativeDistances[theNeighbor]=distance;
					previousVertices[theNeighbor]=current;

					// update the distance index
					verticesWithDistance[oldDistance].erase(theNeighbor);
					verticesWithDistance[distance].insert(theNeighbor);
				}
			}
		}

		// mark the current vertex as not used
		unvisited.erase(current);

		// remove it as well from the index
		int theDistance=tentativeDistances[current];
		verticesWithDistance[theDistance].erase(current);

		if(verticesWithDistance[theDistance].size()==0)
			verticesWithDistance.erase(theDistance);

		// the next current is the one in unvisited vertices
		// with the lowest distance
		
		int bestDistance=-1;

		// find it using the index
		// the index contains only unvisited vertices
		for(map<int,set<int> >::iterator myIterator=verticesWithDistance.begin();
			myIterator!=verticesWithDistance.end();myIterator++){

			int theDistance=myIterator->first;

			// we are done if all the remaining distances are greater
			if(bestDistance!=-1 && theDistance > bestDistance)
				break;

			// find a vertex with the said distance
			for(set<int>::iterator i=myIterator->second.begin();
				i!=myIterator->second.end();i++){
				int vertex=*i;

				if(theDistance < bestDistance || bestDistance==-1){
					current=vertex;
					bestDistance=tentativeDistances[vertex];

					// we can break because all the other remaining 
					// for this distance have the same distance (obviously)
					break;
				}
			}
		}
	}

	// generate the route
	current=destination;
	while(current!=source){
		route->push_back(current);
		current=previousVertices[current];
	}

	route->push_back(source);

	// invert the route
	int left=0;
	int right=route->size()-1;
	while(left<right){
		int t=(*route)[left];
		(*route)[left]=(*route)[right];
		(*route)[right]=t;
		left++;
		right--;
	}

	#ifdef CONFIG_ROUTING_VERBOSITY
	// print the best distance
	cout<<"Shortest path from "<<source<<" to "<<destination<<" is "<<tentativeDistances[destination]<<"	";
	cout<<"Path:	"<<route->size()<<"	";
	for(int i=0;i<(int)route->size();i++){
		cout<<" "<<route->at(i);
	}
	cout<<endl;
	#endif
}

void getRoute(int source,int destination,map<int,map<int,map<int,int> > >*routes,vector<int>*route){
	set<int> processed;
	
	int current=source;

	while(current!=destination){
		processed.insert(current);

		route->push_back(current);

		#ifdef ASSERT
		assert((*routes)[source][destination].count(current)>0);
		#endif

		current=(*routes)[source][destination][current];
	
		#ifdef ASSERT
		assert(processed.count(current)==0);
		#endif
	}

	route->push_back(current);
}

void printRoute(int source,int destination,map<int,map<int,map<int,int> > >*routes){
	cout<<"[printRoute] Source: "<<source<<"	Destination: "<<destination<<"	";

	vector<int> route;
	getRoute(source,destination,routes,&route);

	cout<<"Size: "<<route.size()<<"	Route: ";

	for(int i=0;i<(int)route.size();i++){
		if(i!=0)
			cout<<" ";
		cout<<route[i];
	}
	cout<<"	Hops: "<<route.size()-1<<endl;
}

void makeRoutes(int n,map<int,map<int,map<int,int> > >*routes,map<int,set<int> >*connections){

	int step=n/60;

	for(int source=0;source<n;source++){
		#ifndef CONFIG_ROUTING_VERBOSITY
		cout<<source<<" ";
		cout.flush();
		#endif

		for(int destination=0;destination<n;destination++){
			#ifndef CONFIG_ROUTING_VERBOSITY
			if(destination%step==0){
				cout<<"*";
				cout.flush();
			}
			#endif

			if(destination<source)
				continue;

			vector<int> route;
			findShortestPath(n,source,destination,connections,&route);

			for(int i=0;i<(int)route.size()-1;i++){
				// add the route
				(*routes)[source][destination][route[i]]=route[i+1];

				// add the reverse route
				(*routes)[destination][source][route[i+1]]=route[i];

			}

			#ifdef CONFIG_ROUTING_VERBOSITY
			printRoute(source,destination,routes);

			printRoute(destination,source,routes);
			#endif
		}

		#ifndef CONFIG_ROUTING_VERBOSITY
		double ratio=source*100.0/n;
		cout<<" "<<ratio<<"%"<<endl;
		#endif
	}
}

void viewRoutes(int n,map<int,map<int,map<int,int> > >*routes){
	for(int i=0;i<n;i++)
		for(int j=0;j<n;j++)
			printRoute(i,j,routes);
}

int main(int argc,char**argv){
	map<int,set<int> > connections;
	map<int,map<int,map<int,int> > > routes;
	int n=atoi(argv[1]);

	makeConnections(n,&connections);
	makeRoutes(n,&routes,&connections);

	viewConnections(n,&connections);
	viewRoutes(n,&routes);

	return 0;
}
