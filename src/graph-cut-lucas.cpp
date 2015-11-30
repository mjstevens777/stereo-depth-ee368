#include "graph-cut.h"
using namespace std;

GraphCutDisparity::edge_weight GraphCutDisparity::occ_cost(Correspondence c) {

	return 0;
} 

GraphCutDisparity::edge_weight GraphCutDisparity::smooth_cost(Correspondence c){

	return 0;
} 

vector<GraphCutDisparity::Correspondence> GraphCutDisparity::get_neighbors(Correspondence c, int alpha){
	vector<Correspondence> neighbors;
	vector<int> offset = {0, 0, 1, -1};

	// x +- 1, y +- 1 neighbors, where is_valid
	for (int i=0; i<offset.size(); i++) {
		Correspondence c_tmp = {c.x + offset[i], c.y + offset[offset.size()-1-i], c.d}; 

		if(is_valid(c_tmp, alpha) ) {
			neighbors.push_back(c_tmp);
		}
	}

	return neighbors;
} 

vector<GraphCutDisparity::Correspondence> GraphCutDisparity::get_conflicts(Correspondence c, int alpha){
	vector<Correspondence> conflicts;

	if(is_active(c) and c.d != alpha ) {
		// check shared pixel
		Correspondence c_alpha = {c.x, c.y, alpha};
		if (is_valid(c_alpha, alpha)) {
			conflicts.push_back(c_alpha);
		}

		// check shared mapped pixel
		Correspondence c_mapped = { c.x + c.d - alpha, c.y, alpha};
		if (is_valid(c_mapped, alpha)) {
			conflicts.push_back(c_mapped);
		}
	}

	return conflicts;
} 


void GraphCutDisparity::add_active_node(Correspondence c, int alpha){


	return;
} 

void GraphCutDisparity::add_alpha_node(Correspondence c, int alpha){

	return;
} 

void GraphCutDisparity::add_neighbor_edges(Correspondence c, int alpha){

	return;
} 

void GraphCutDisparity::add_conflict_edges(Correspondence c, int alpha){

	return;
} 

