#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED
#pragma once
#include "Node.h"
#include<string>

using namespace std;

class Graph{
public:
	int num_nodes;
	int num_edges;
	Node *node_array;

	Graph(string data);
	~Graph();
	void print();
};



#endif // GRAPH_H_INCLUDED
