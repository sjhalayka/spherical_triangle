#include "main.h"


int main(int argc, char **argv)
{
	if(false == get_data(sd))
		return -1;

	min_year = 10000000;
	max_year = 0;

	for(size_t i = 0; i < sd.size(); i++)
	{
		for(map<short unsigned int, year_data>::const_iterator ci = sd[i].years.begin(); ci != sd[i].years.end(); ci++)
		{
			if(ci->first < min_year)
				min_year = ci->first;

			if(ci->first > max_year)
				max_year = ci->first;
		}
	}



	curr_year = 1969;
	curr_month = 0;
	trends_first_year = min_year;
	trends_last_year = max_year;

	tess.vertices.resize(sd.size());

	cout << "Generating vertices from latitude and longitude data." << endl;

	for(size_t i = 0; i < tess.vertices.size(); i++)
		latlon_to_xyz(sd[i].latitude, sd[i].longitude, tess.vertices[i].x, tess.vertices[i].y, tess.vertices[i].z);

	cout << "Generating triangulation of vertices via TetGen." << endl;

	if(false == tess.produce_tessellations())
		return -1;

	// Get vertex to vertex map
	vertex_to_vertex.resize(tess.vertices.size());

	set<sorted_indexed_edge> edges;

	for(size_t i = 0; i < tess.dtris.size(); i++)
	{
		sorted_indexed_edge edge0(tess.dtris[i].i0, tess.dtris[i].i1);
		sorted_indexed_edge edge1(tess.dtris[i].i1, tess.dtris[i].i2);
		sorted_indexed_edge edge2(tess.dtris[i].i2, tess.dtris[i].i0);

		edges.insert(edge0);
		edges.insert(edge1);
		edges.insert(edge2);
	}

	for(set<sorted_indexed_edge>::const_iterator ci = edges.begin(); ci != edges.end(); ci++)
	{
		vertex_to_vertex[ci->v0].push_back(ci->v1);
		vertex_to_vertex[ci->v1].push_back(ci->v0);
	}


	cout << "Generating (approximately) spherical triangles." << endl;

	// Get edge length data to make for variable subdivision
	size_t max_subdivisions = 2;
	double longest_edge = 0;

	const double exponent = 1; // lesser value means more subdivision

	for(size_t i = 0; i < tess.dtris.size(); i++)
	{
		sorted_indexed_edge edge0(tess.dtris[i].i0, tess.dtris[i].i1);
		sorted_indexed_edge edge1(tess.dtris[i].i1, tess.dtris[i].i2);
		sorted_indexed_edge edge2(tess.dtris[i].i2, tess.dtris[i].i0);

		double dist0 = pow(d_3(tess.vertices[tess.dtris[i].i0], tess.vertices[tess.dtris[i].i1]), exponent);
		double dist1 = pow(d_3(tess.vertices[tess.dtris[i].i1], tess.vertices[tess.dtris[i].i2]), exponent);
		double dist2 = pow(d_3(tess.vertices[tess.dtris[i].i2], tess.vertices[tess.dtris[i].i0]), exponent);

		if(dist0 > longest_edge)
			longest_edge = dist0;
		
		if(dist1 > longest_edge)
			longest_edge = dist1;

		if(dist2 > longest_edge)
			longest_edge = dist2;
	}



	ctris.resize(tess.dtris.size());

	for(size_t i = 0; i < tess.dtris.size(); i++)
	{
		sorted_indexed_edge edge0(tess.dtris[i].i0, tess.dtris[i].i1);
		sorted_indexed_edge edge1(tess.dtris[i].i1, tess.dtris[i].i2);
		sorted_indexed_edge edge2(tess.dtris[i].i2, tess.dtris[i].i0);

		double dist0 = pow(d_3(tess.vertices[tess.dtris[i].i0], tess.vertices[tess.dtris[i].i1]), exponent);
		double dist1 = pow(d_3(tess.vertices[tess.dtris[i].i1], tess.vertices[tess.dtris[i].i2]), exponent);
		double dist2 = pow(d_3(tess.vertices[tess.dtris[i].i2], tess.vertices[tess.dtris[i].i0]), exponent);

		double local_longest = dist0;

		if(dist1 > local_longest)
			local_longest = dist1;

		if(dist2 > local_longest)
			local_longest = dist2;

		size_t subdivisions = ceil(static_cast<double>(max_subdivisions)*(local_longest/longest_edge));

		ctris[i].init_geometry(tess.dtris[i].i0, tess.vertices[tess.dtris[i].i0], 
							tess.dtris[i].i1, tess.vertices[tess.dtris[i].i1], 
							tess.dtris[i].i2, tess.vertices[tess.dtris[i].i2],
							subdivisions);
	}




	local_mean_trends.resize(tess.vertices.size());
	local_trend_std_devs.resize(tess.vertices.size());


	vector<float> mat(4, 0.0f);
	materials.resize(tess.vertices.size(), mat);
	
	generate_materials();







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



// Produces huge drop in 1991

	// get number of valid stations per year
	//map<size_t, size_t> year_count;

	//for(size_t i = min_year; i <= max_year; i++)
	//	year_count[i] = 0;

	//for(size_t i = 0; i < sd.size(); i++)
	//	for(map<short unsigned int, year_data>::const_iterator ci = sd[i].years.begin(); ci != sd[i].years.end(); ci++)
	//		year_count[ci->first]++;

	//for(map<size_t, size_t>::const_iterator ci = year_count.begin(); ci != year_count.end(); ci++)
	//	cout << ci->first << ' ' << ci->second << endl;

