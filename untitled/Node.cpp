#include "Node.h"

Node::Node(){
	k_out = 0;
	k_in = 0;
	id_array = NULL;  
        in_array = NULL;           //array of out edges
	out_edges = new set<int>();  //set of out edges for temporal use
	in_edges = new set<int>();
}

Node::~Node(){
	if(out_edges != NULL){
		out_edges->clear();
		delete out_edges;
	}
	if(in_edges != NULL){
		in_edges->clear();
		delete in_edges;
	}
	if(id_array != NULL)
		delete id_array;
        if(id_array != NULL)
		delete in_array;
}
