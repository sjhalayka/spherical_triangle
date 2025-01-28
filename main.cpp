#include "main.h"






int main(int argc, char** argv)
{
	const size_t num_points = 5000;

	tess.vertices.resize(num_points);

	cout << "Generating pseudorandom vertices" << endl;

	for (size_t i = 0; i < tess.vertices.size(); i++)
		tess.vertices[i] = RandomUnitVector();


	for (size_t x = 0; x < 100; x++)
	{
		cout << x << " " << 100 << endl;

		vector<vector_3> backup_points = tess.vertices;

		for (size_t i = 0; i < num_points; i++)
		{
			vector_3 a(0, 0, 0);

			for (size_t j = 0; j < num_points; j++)
			{
				if (i == j)
					continue;

				custom_math::vector_3 grav_dir = backup_points[j] - backup_points[i];

				double distance = grav_dir.length();
				grav_dir.normalize();
				custom_math::vector_3 accel = -grav_dir / pow(distance, 1.0);

				a += accel;
			}

			tess.vertices[i] += a;
			tess.vertices[i].normalize();
		}
	}




	cout << "Generating triangulation of vertices via TetGen" << endl;

	if (false == tess.produce_tessellations())
		return -1;

	// Get vertex to vertex map
	vertex_to_vertex.resize(tess.vertices.size());

	set<sorted_indexed_edge> edges;

	for (size_t i = 0; i < tess.dtris.size(); i++)
	{
		sorted_indexed_edge edge0(tess.dtris[i].i0, tess.dtris[i].i1);
		sorted_indexed_edge edge1(tess.dtris[i].i1, tess.dtris[i].i2);
		sorted_indexed_edge edge2(tess.dtris[i].i2, tess.dtris[i].i0);

		edges.insert(edge0);
		edges.insert(edge1);
		edges.insert(edge2);
	}

	for (set<sorted_indexed_edge>::const_iterator ci = edges.begin(); ci != edges.end(); ci++)
	{
		vertex_to_vertex[ci->v0].push_back(ci->v1);
		vertex_to_vertex[ci->v1].push_back(ci->v0);
	}


	cout << "Generating (approximately) spherical triangles" << endl;

	// Get edge length data to make for variable subdivision
	size_t max_subdivisions = 5;
	//double longest_edge = 0;

	//const double exponent = 1; // lesser value means more subdivision

	//for(size_t i = 0; i < tess.dtris.size(); i++)
	//{
	//	sorted_indexed_edge edge0(tess.dtris[i].i0, tess.dtris[i].i1);
	//	sorted_indexed_edge edge1(tess.dtris[i].i1, tess.dtris[i].i2);
	//	sorted_indexed_edge edge2(tess.dtris[i].i2, tess.dtris[i].i0);

	//	double dist0 = pow(d_3(tess.vertices[tess.dtris[i].i0], tess.vertices[tess.dtris[i].i1]), exponent);
	//	double dist1 = pow(d_3(tess.vertices[tess.dtris[i].i1], tess.vertices[tess.dtris[i].i2]), exponent);
	//	double dist2 = pow(d_3(tess.vertices[tess.dtris[i].i2], tess.vertices[tess.dtris[i].i0]), exponent);

	//	if(dist0 > longest_edge)
	//		longest_edge = dist0;
	//	
	//	if(dist1 > longest_edge)
	//		longest_edge = dist1;

	//	if(dist2 > longest_edge)
	//		longest_edge = dist2;
	//}



	ctris.resize(tess.dtris.size());

	for (size_t i = 0; i < tess.dtris.size(); i++)
	{
		//sorted_indexed_edge edge0(tess.dtris[i].i0, tess.dtris[i].i1);
		//sorted_indexed_edge edge1(tess.dtris[i].i1, tess.dtris[i].i2);
		//sorted_indexed_edge edge2(tess.dtris[i].i2, tess.dtris[i].i0);

		//double dist0 = pow(d_3(tess.vertices[tess.dtris[i].i0], tess.vertices[tess.dtris[i].i1]), exponent);
		//double dist1 = pow(d_3(tess.vertices[tess.dtris[i].i1], tess.vertices[tess.dtris[i].i2]), exponent);
		//double dist2 = pow(d_3(tess.vertices[tess.dtris[i].i2], tess.vertices[tess.dtris[i].i0]), exponent);

		//double local_longest = dist0;

		//if(dist1 > local_longest)
		//	local_longest = dist1;

		//if(dist2 > local_longest)
		//	local_longest = dist2;

		size_t subdivisions = 5;// ceil(static_cast<double>(max_subdivisions)* (local_longest / longest_edge));

		ctris[i].init_geometry(
			tess.dtris[i].i0, tess.vertices[tess.dtris[i].i0],
			tess.dtris[i].i1, tess.vertices[tess.dtris[i].i1],
			tess.dtris[i].i2, tess.vertices[tess.dtris[i].i2],
			subdivisions);
	}













	cout << "Generating (approximately) spherical hexagons (out of triangles)" << endl;

	// Get edge length data to make for variable subdivision
	//longest_edge = 0;


	//for (size_t i = 0; i < tess.vtris.size(); i++)
	//{
	//	sorted_indexed_edge edge0(tess.vtris[i].i0, tess.vtris[i].i1);
	//	sorted_indexed_edge edge1(tess.vtris[i].i1, tess.vtris[i].i2);
	//	sorted_indexed_edge edge2(tess.vtris[i].i2, tess.vtris[i].i0);

	//	double dist0 = pow(d_3(tess.dual_vertices[tess.vtris[i].i0], tess.dual_vertices[tess.vtris[i].i1]), exponent);
	//	double dist1 = pow(d_3(tess.dual_vertices[tess.vtris[i].i1], tess.dual_vertices[tess.vtris[i].i2]), exponent);
	//	double dist2 = pow(d_3(tess.dual_vertices[tess.vtris[i].i2], tess.dual_vertices[tess.vtris[i].i0]), exponent);

	//	if (dist0 > longest_edge)
	//		longest_edge = dist0;

	//	if (dist1 > longest_edge)
	//		longest_edge = dist1;

	//	if (dist2 > longest_edge)
	//		longest_edge = dist2;
	//}



	vctris.resize(tess.vtris.size());

	for (size_t i = 0; i < tess.vtris.size(); i++)
	{
		//sorted_indexed_edge edge0(tess.vtris[i].i0, tess.vtris[i].i1);
		//sorted_indexed_edge edge1(tess.vtris[i].i1, tess.vtris[i].i2);
		//sorted_indexed_edge edge2(tess.vtris[i].i2, tess.vtris[i].i0);

		//double dist0 = pow(d_3(tess.dual_vertices[tess.vtris[i].i0], tess.dual_vertices[tess.vtris[i].i1]), exponent);
		//double dist1 = pow(d_3(tess.dual_vertices[tess.vtris[i].i1], tess.dual_vertices[tess.vtris[i].i2]), exponent);
		//double dist2 = pow(d_3(tess.dual_vertices[tess.vtris[i].i2], tess.dual_vertices[tess.vtris[i].i0]), exponent);

		//double local_longest = dist0;

		//if (dist1 > local_longest)
		//	local_longest = dist1;

		//if (dist2 > local_longest)
		//	local_longest = dist2;

		size_t subdivisions = 5;// ceil(static_cast<double>(max_subdivisions)* (local_longest / longest_edge));

		vctris[i].init_geometry(
			tess.vtris[i].i0, tess.dual_vertices[tess.vtris[i].i0],
			tess.vtris[i].i1, tess.dual_vertices[tess.vtris[i].i1],
			tess.vtris[i].i2, tess.dual_vertices[tess.vtris[i].i2],
			subdivisions);
	}

	vgon_colours.resize(tess.vngons.size(), vector_3(1, 0, 0));



	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load("map.png", &image_width, &image_height, NULL, 4);

	if (image_data != NULL)
	{
		cout << "read map.png" << endl;

		for (size_t i = 0; i < vgon_colours.size(); i++)
		{
			const vector_3 location = tess.dual_centres[i];
			const pair<int, int> pixel_coords = mapToSphere(image_width, image_height, location.x, location.y, location.z);
			const size_t index = 4*(pixel_coords.second * image_width + pixel_coords.first);

			vgon_colours[i].x = image_data[index + 0] / 255.0;
			vgon_colours[i].y = image_data[index + 1] / 255.0;
			vgon_colours[i].z = image_data[index + 2] / 255.0;
		}

		stbi_image_free(image_data);
	}






	cout << "Drawing." << endl;

	// Draw
	glutInit(&argc, argv);
	init_opengl(win_x, win_y);
	glutReshapeFunc(reshape_func);
	glutIdleFunc(idle_func);
	glutDisplayFunc(display_func);
	glutKeyboardFunc(keyboard_func);
	glutMouseFunc(mouse_func);
	glutMotionFunc(motion_func);
	glutPassiveMotionFunc(passive_motion_func);
	//glutIgnoreKeyRepeat(1);
	glutMainLoop();
	glutDestroyWindow(win_id);

	return 0;
}

