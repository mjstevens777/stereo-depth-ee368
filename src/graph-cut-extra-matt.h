long correspondence_hash(Correspondence c);

int num_iters = 2;

typedef boost::vecS OutEdgeListT;
typedef boost::vecS VertexListT;
typedef boost::directedS DirectedT;

typedef boost::adjacency_list_traits <OutEdgeListT, VertexListT, DirectedT> Traits;

typedef Traits::edge_descriptor Edge;
typedef Traits::vertex_descriptor Vertex;

typedef boost::default_color_type Color;

typedef
    boost::property < boost::vertex_index_t, node_index,
    boost::property < boost::vertex_color_t, Color,
    boost::property < boost::vertex_distance_t, edge_weight,
    boost::property < boost::vertex_predecessor_t, Edge> > > >
  VertexPropertiesT;

typedef
    boost::property < boost::edge_capacity_t, edge_weight,
    boost::property < boost::edge_residual_capacity_t, edge_weight,
    boost::property < boost::edge_reverse_t, Edge> > >
  EdgePropertiesT;

typedef boost::adjacency_list <
    OutEdgeListT, VertexListT, DirectedT,
    VertexPropertiesT, EdgePropertiesT >
  Graph;

typedef boost::property_map<Graph, boost::edge_capacity_t>::type CapacityMap;
typedef boost::property_map<Graph, boost::edge_residual_capacity_t>::type ResidualMap;
typedef boost::property_map<Graph, boost::edge_reverse_t>::type ReverseMap;
typedef boost::property_map<Graph, boost::vertex_color_t>::type ColorMap;
typedef boost::property_map<Graph, boost::vertex_index_t>::type VertexIndexMap;

VertexIndexMap vertex_indices;
CapacityMap capacities;
ResidualMap residual_capacities;
ReverseMap reverse_edges;
ColorMap colors;
Graph g;

Vertex source;
Vertex sink;

Vertex get_vertex(Correspondence c);


std::map<long, node_index> hash_to_graph_index;

void for_each_active(std::function<void(Correspondence)> fn, int alpha);
void for_each_alpha(std::function<void(Correspondence)> fn, int alpha);

cv::Mat left_occlusion_count, right_occlusion_count;
void record_occlusion_count(Correspondence c, int alpha);
void record_occlusion_counts(int alpha);

bool within_bounds(Correspondence c);