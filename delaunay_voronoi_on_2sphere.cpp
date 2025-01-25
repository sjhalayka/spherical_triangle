#include "delaunay_voronoi_on_2sphere.h"


delaunay_voronoi_on_2sphere::delaunay_voronoi_on_2sphere(void)
{
}

bool delaunay_voronoi_on_2sphere::produce_tessellations(void)
{
	if (0 == vertices.size())
		return false;

	return construct_delaunay_voronoi();
}

void delaunay_voronoi_on_2sphere::clear_meshes(void)
{
	dtris.clear();
	dtri_normals.clear();
	dtri_centres.clear();
	dtri_angles.clear();
	//dtri_adjacencies.clear();

	dual_vertices.clear();
	vngons.clear();
	vtris.clear();
	//vngon_adjacencies.clear();
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

		dual_vertices.push_back(W);

		offset += 3;
	}

	for (size_t t = 0; t < out.numberofvfacets; t++)
	{
		tetgenio::vorofacet vf = out.vfacetlist[t];

		bool is_valid_facet = true;

		vector<size_t> facet_indices;

		size_t edge_count = vf.elist[0];

		vector<int> firsts;
		vector<int> seconds;

		for (size_t i = 1; i < edge_count + 1; i++)
		{
			const size_t edge_index = vf.elist[i];
			const size_t v1 = out.vedgelist[edge_index].v1;
			const size_t v2 = out.vedgelist[edge_index].v2;

			// Only use facets that are completely
			// finite in terms of location
			if (v1 == -1 || v2 == -1)
			{
				is_valid_facet = false;
				break;
			}

			// Don't use degenerate edges
			if (v1 == v2)
			{
				//is_valid_facet = false;
				continue;
				//break;
			}

			// If not found at all
			if (firsts.end() == find(firsts.begin(), firsts.end(), v1) && seconds.end() == find(seconds.begin(), seconds.end(), v2))
			{
				firsts.push_back(v1);
				seconds.push_back(v2);
				
				facet_indices.push_back(v1);
				facet_indices.push_back(v2);
				cout << "found neither" << endl;
			}
			// else if both found
			else if (firsts.end() != find(firsts.begin(), firsts.end(), v1) && seconds.end() != find(seconds.begin(), seconds.end(), v2))
			{
				cout << "found both" << endl;

				//facet_indices.push_back(v2);
				//facet_indices.push_back(v1);

				// Do nothing

			}
			else if (firsts.end() != find(firsts.begin(), firsts.end(), v1))
			{
				cout << "found first" << endl;

				seconds.push_back(v1);

				facet_indices.push_back(v2);
				facet_indices.push_back(v1);
			}
			else if (seconds.end() != find(seconds.begin(), seconds.end(), v2))
			{
				cout << "found second" << endl;

				firsts.push_back(v2);
				
				facet_indices.push_back(v2);
				facet_indices.push_back(v1);
			}
		}

		if (is_valid_facet)
		{
			indexed_ngon ngon;

			for (size_t j = 0; j < facet_indices.size(); j++)
			{
				//if (ngon.v.end() == find(ngon.v.begin(), ngon.v.end(), facet_indices[j]))
				ngon.v.push_back(facet_indices[j]);

				cout << facet_indices[j] << endl;
			}

			cout << endl;

			vngons.push_back(ngon);
		}
	}

	int x = 0;

	for (size_t i = 0; i < vngons.size(); i++)
	{
		bool found_matches = true;
		

		
		do
		{
			found_matches = false;

			vector<pair<size_t, size_t>> vp;

			for (size_t j = 0; j < vngons[i].v.size() - 1; j += 1)
			{
				pair<size_t, size_t> p(vngons[i].v[j], vngons[i].v[j + 1]);
				vp.push_back(p);
			}

			//sort(vp.begin(), vp.end());

			//pair<size_t, size_t> p(vngons[i].v[0], vngons[i].v[vngons[i].v.size() - 1]);
			//vp.push_back(p);

			if (vp.size() < 2)
				break;

			cout << "vp size: " << vp.size() << endl;

			vngons[i].v.clear();

			int previous_value = vp[0].first;

			while (vp.size() > 0)
			{
				for (size_t j = 0; j < vp.size(); j++)
				{
					if (vp[j].first == previous_value)
					{
						//if (vngons[i].v.end() == find(vngons[i].v.begin(), vngons[i].v.end(), vp[j].first))
						vngons[i].v.push_back(vp[j].first);

						previous_value = vp[j].second;

						vp.erase(vp.begin() + j);

						found_matches = true;

						break;
					}
				}
			}

			x++;

		} while (x < 100);
	}

	 
	for (size_t i = 0; i < vngons.size(); i++)
	{
		for (size_t j = 0; j < vngons[i].v.size() - 1; j += 1)
		{
			pair<size_t, size_t> p(vngons[i].v[j], vngons[i].v[j + 1]);


			cout << "pairs: " << p.first << " " << p.second << endl;
		}

		cout << endl;
	}

	cout << endl << endl;

	for (size_t i = 0; i < vngons.size(); i++)
	{
		vector_3 centre;

		for (size_t j = 0; j < vngons[i].v.size(); j++)
			centre += dual_vertices[vngons[i].v[j]];

		centre = centre / static_cast<double>(vngons[i].v.size());

		dual_vertices.push_back(centre);

		for (size_t j = 0; j < vngons[i].v.size() - 1; j += 1)
		{
			size_t v0 = 0, v1 = 0;

			v0 = vngons[i].v[j];
			v1 = vngons[i].v[j + 1];

			indexed_triangle tri;
			tri.i0 = v0;
			tri.i1 = v1;
			tri.i2 = dual_vertices.size() - 1;

			//if (tri.i0 < dual_vertices.size() && tri.i1 < dual_vertices.size() && tri.i2 < dual_vertices.size())
			vtris.push_back(tri);
		}
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

		if (tri.i0 < vertices.size() && tri.i1 < vertices.size() && tri.i2 < vertices.size())
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

