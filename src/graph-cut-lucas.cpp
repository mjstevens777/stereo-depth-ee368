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

	// x +- 1, y +- 1 neighbors, where is_valid
	for (int x_offset=-1; x_offset<2; x_offset+=2) {
		for (int y_offset=-1; y_offset<2; y_offset+=2) {
	
			Correspondence c_tmp = {c.x + x_offset, c.y + y_offset, c.d}; 

			if(this->is_valid(c_tmp, alpha) ) {
				neighbors.push_back(c_tmp);
			}
		}
	}

	return neighbors;
} 

vector<GraphCutDisparity::Correspondence> GraphCutDisparity::get_conflicts(Correspondence c, int alpha){
	vector<Correspondence> conflicts;



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

