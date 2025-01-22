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
	vngons.clear();
	vngon_adjacencies.clear();
}

bool delaunay_voronoi_on_2sphere::construct_delaunay_voronoi(void)
{
	clear_meshes();

	vertices.push_back(vector_3(0, 0, 0));
	size_t centre_vertex_index = vertices.size() - 1;

	tetgenio in, out;
	in.mesh_dim = 3;
	in.firstnumber = 0;
	in.numberofpoints = (int)vertices.size();
	in.pointlist = &(vertices[0].x);

	tetgenbehavior b;
	b.quiet = 0;
	b.verbose = 1;

	tetrahedralize(&b, &in, &out);

	// Manually clean up any input object memory that might have been allocated by TetGen now.
	// Don't forget to reset the in.pointlist pointer's value to zero so that our vertex subset doesn't get accidentally deallocated.
	in.pointlist = 0;
	in.clean_memory();

	size_t offset = 0;

	vector<custom_math::vector_3> new_points;

	for (size_t t = 0; t < out.numberofpoints; t++)
	{
		double x = out.pointlist[offset + 0];
		double y = out.pointlist[offset + 1];
		double z = out.pointlist[offset + 2];
		new_points.push_back(custom_math::vector_3(x, y, z));

		offset += 3;
	}

	vertices = new_points;

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

		//if (tri.i0 != centre_vertex_index && tri.i1 != centre_vertex_index && tri.i2 != centre_vertex_index)
			dtris.push_back(tri);

		tri.i0 = l;
		tri.i1 = j;
		tri.i2 = k;

		//if (tri.i0 != centre_vertex_index && tri.i1 != centre_vertex_index && tri.i2 != centre_vertex_index)
			dtris.push_back(tri);

		tri.i0 = l;
		tri.i1 = k;
		tri.i2 = i;

		//if (tri.i0 != centre_vertex_index && tri.i1 != centre_vertex_index && tri.i2 != centre_vertex_index)
			dtris.push_back(tri);

		tri.i0 = l;
		tri.i1 = i;
		tri.i2 = j;

		//if (tri.i0 != centre_vertex_index && tri.i1 != centre_vertex_index && tri.i2 != centre_vertex_index)
			dtris.push_back(tri);

		offset += 4;
	}


	return true;
}

