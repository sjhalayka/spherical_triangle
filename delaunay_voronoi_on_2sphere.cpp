#include "delaunay_voronoi_on_2sphere.h"


delaunay_voronoi_on_2sphere::delaunay_voronoi_on_2sphere(void)
{
}

bool delaunay_voronoi_on_2sphere::produce_tessellations(void)
{
	if(0 == vertices.size())
		return false;

	return construct_delaunay_voronoi();
}

void delaunay_voronoi_on_2sphere::clear_meshes(void)
{
	dtris.clear();
	dtri_normals.clear();
	dtri_centres.clear();
	dtri_angles.clear();
	dtri_adjacencies.clear();

	dual_vertices.clear();
	voronoi_edges.clear();
	vngons.clear();
	vngon_adjacencies.clear();
}

bool delaunay_voronoi_on_2sphere::construct_delaunay_voronoi(void)
{
	clear_meshes();

	vertices.push_back(vector_3(0, 0, 0));
	//size_t centre_vertex_index = vertices.size() - 1;

	tetgenio in, out;
	in.mesh_dim = 3;
	in.firstnumber = 0;
	in.numberofpoints = (int)vertices.size();
	in.pointlist = &(vertices[0].x);

	tetgenbehavior b;
	b.quiet = 0;
	b.verbose = 1;
	b.voroout = 1;

	tetrahedralize(&b, &in, &out);

	// Manually clean up any input object memory that might have been allocated by TetGen now.
	// Don't forget to reset the in.pointlist pointer's value to zero so that our vertex subset doesn't get accidentally deallocated.
	in.pointlist = 0;
	in.clean_memory();

	size_t offset = 0;

	vector<custom_math::vector_3> new_points;

	for (size_t t = 0; t < out.numberofpoints; t++)
	{
		const double x = out.pointlist[offset + 0];
		const double y = out.pointlist[offset + 1];
		const double z = out.pointlist[offset + 2];

		const custom_math::vector_3 W = vector_3(x, y, z).normalize();

		new_points.push_back(W);

		offset += 3;	
	}


	offset = 0;

	for (size_t t = 0; t < out.numberofvpoints; t++)
	{
		const double x = out.vpointlist[offset + 0];
		const double y = out.vpointlist[offset + 1];
		const double z = out.vpointlist[offset + 2];

		const custom_math::vector_3 W = vector_3(x, y, z).normalize();

		//if(W.length() > 0.0001)
		dual_vertices.push_back(W);

		offset += 3;
	}


	for (size_t t = 0; t < out.numberofvfacets; t++)
	{
		tetgenio::vorofacet vf = out.vfacetlist[t];

		bool is_valid = true;

		vector<vector_3> facet_vertices;
		vector<sorted_indexed_edge> facet_edges;
		
		size_t edge_count = vf.elist[0];

		for (size_t i = 1; i < edge_count + 1; i++)
		{
			size_t edge_index = vf.elist[i];

			if (out.vedgelist[edge_index].v1 == -1 || out.vedgelist[edge_index].v2 == -1)
			{
				is_valid = false;
				break;
			}

			if(is_valid)
			{
				facet_vertices.push_back(out.vpointlist[out.vedgelist[edge_index].v1]);
				facet_vertices.push_back(out.vpointlist[out.vedgelist[edge_index].v2]);
				sorted_indexed_edge edge(out.vedgelist[edge_index].v1, out.vedgelist[edge_index].v2);
				facet_edges.push_back(edge);
			}
		}

		for (size_t j = 0; j < facet_edges.size(); j++)
			voronoi_edges.push_back(facet_edges[j]);
	}


	vertices = new_points;
	vertices.pop_back();

	offset = 0;

	for (size_t t = 0; t < out.numberoftetrahedra; t++)
	{
		size_t i = out.tetrahedronlist[offset + 0];
		size_t j = out.tetrahedronlist[offset + 1];
		size_t k = out.tetrahedronlist[offset + 2];
		size_t l = out.tetrahedronlist[offset + 3];

		indexed_triangle tri;

		tri.i0 = k;
		tri.i1 = j;
		tri.i2 = i;

		if (tri.i0 < vertices.size()  && tri.i1 < vertices.size() && tri.i2 < vertices.size() )
			dtris.push_back(tri);

		tri.i0 = l;
		tri.i1 = j;
		tri.i2 = k;

		if (tri.i0 < vertices.size() && tri.i1 < vertices.size() && tri.i2 < vertices.size())
			dtris.push_back(tri);

		tri.i0 = l;
		tri.i1 = k;
		tri.i2 = i;

		if (tri.i0 < vertices.size() && tri.i1 < vertices.size() && tri.i2 < vertices.size())
			dtris.push_back(tri);

		tri.i0 = l;
		tri.i1 = i;
		tri.i2 = j;

		if (tri.i0 < vertices.size() && tri.i1 < vertices.size() && tri.i2 < vertices.size())
			dtris.push_back(tri);

		offset += 4;
	}


	return true;
}

