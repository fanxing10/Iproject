#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED
#pragma once
#include<set>
#include<stdio.h>

using namespace std;

class Node{
public:
	int k_out;  //# of out edges
	int k_in;  //# of in edges

	int* id_array;  // array of out edges
	int* in_array;
	//set¼¯ºÏÈÝÆ÷:ÊµÏÖÁËºìºÚÊ÷µÄÆ½ºâ¶þ²æ¼ìË÷Ê÷µÄÊý¾Ý½á¹¹
    //¹¹Ôìset¼¯ºÏÖ÷ÒªÄ¿µÄÊÇÎªÁË¿ìËÙ¼ìË÷£¬²»¿ÉÖ±½ÓÈ¥ÐÞ¸Ä¼üÖµ
    set<int> *in_edges;
	set<int> *out_edges;  // set of out edges for temporal use


	Node();
	~Node();
};



#endif // NODE_H_INCLUDED
