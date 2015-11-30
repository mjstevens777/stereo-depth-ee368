#include "graph-cut.h"
using namespace std;

GraphCutDisparity::edge_weight GraphCutDisparity::occ_cost(Correspondence c) {
	
	
	return 0;
} 

GraphCutDisparity::edge_weight GraphCutDisparity::smooth_cost(Correspondence c){
	vector<Correspondence> neighbors = get_neighbors(c,c.d);

	return V_CONST * neighbors.size();
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
	edge_weight source_w = occ_cost(c);
	edge_weight sink_w = data_cost(c) + smooth_cost(c);

	add_node(c);
	add_source_edge(c, source_w);
	add_sink_edge(c, sink_w);
	return;
} 

void GraphCutDisparity::add_alpha_node(Correspondence c, int alpha){
	edge_weight source_w = data_cost(c);
	edge_weight sink_w = occ_cost(c);

	add_node(c);
	add_source_edge(c, source_w);
	add_sink_edge(c, sink_w);

	return;
} 

void GraphCutDisparity::add_neighbor_edges(Correspondence c, int alpha){
	vector<Correspondence> neighbors = get_neighbors(c,alpha);

	for (Correspondence c_tmp : neighbors) {
		add_edge(c, c_tmp, V_CONST);
		add_edge(c_tmp, c, V_CONST);
	}

	return;
} 

void GraphCutDisparity::add_conflict_edges(Correspondence c, int alpha){
	vector<Correspondence> conflicts = get_conflicts(c,alpha);

	for (Correspondence c_tmp : conflicts) {
		add_edge(c, c_tmp, INT_MAX);
		add_edge(c_tmp, c, C_CONST);
	}

	return;
} 

