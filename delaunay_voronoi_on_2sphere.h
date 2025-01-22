#ifndef delaunay_voronoi_on_2sphere_H
#define delaunay_voronoi_on_2sphere_H






#include "tetgen.h"

#include "custom_math.h"
using custom_math::vector_3;
using custom_math::pi;
using custom_math::indexed_triangle;
using custom_math::sorted_indexed_triangle;
using custom_math::sorted_indexed_edge;
using custom_math::indexed_ngon;


#include <vector>
using std::vector;

#include <deque>
using std::deque;

#include <map>
using std::map;

#include <set>
using std::set;

#include <fstream>
using std::ofstream;
using std::ifstream;

#include <sstream>
using std::istringstream;

#include <string>
using std::string;






class delaunay_voronoi_on_2sphere
{
public:
	delaunay_voronoi_on_2sphere(void);
	bool produce_tessellations(void); // Starts the tessellation process

	vector<vector_3> vertices; // Seed vertices
	vector<indexed_triangle> dtris; // Delaunay triangles
	vector<indexed_triangle> dtri_adjacencies; // 3 adjacent triangles per triangle

	vector<vector_3> dual_vertices; // Voronoi vertices
	vector<indexed_ngon> vngons; // Voronoi n-gons (cells)
	vector<vector<size_t>> vngon_adjacencies; // Voronoi n-gon adjacencies

public://protected:
	vector<vector_3> dtri_normals; // Temporary triangle normals
	vector<vector_3> dtri_centres; // Temporary triangle barycentres
	vector<vector_3> dtri_angles; // Temporary triangle flat angles

	void clear_meshes(void);
	bool construct_delaunay_voronoi(void); // Uses seed vertices to generate Delaunay/Voronoi tessellations 
};



#endif



