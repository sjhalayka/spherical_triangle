
#ifndef main_H
#define main_H

#include "station_data.h"
#include "delaunay_voronoi_on_2sphere.h"
#include "uv_camera.h"


#include "custom_math.h"
using custom_math::indexed_curved_triangle;
using custom_math::d_3;
using custom_math::d_3_sq;


#include <ctime>


#include <sstream>
using std::ostringstream;

#include <string>
using std::string;

#include <iostream>
using std::cout;
using std::endl;

#include <fstream>
using std::ofstream;

#include <ios>
using std::ios_base;
using std::ios;

#include <iomanip>
using std::setiosflags;
using std::setprecision;



// If you merge this code into a larger project, you should probably
// at least wrap these global variables/functions in a namespace.

vector<station_data> sd;
delaunay_voronoi_on_2sphere tess;
vector<vector<float>> materials;
vector<vector<size_t>> vertex_to_vertex;
vector<indexed_curved_triangle> ctris;


float global_min_temp, global_max_temp, global_mean_temp, global_mean_temp_std_dev;
float global_min_trend, global_max_trend, global_mean_trend, global_mean_trend_std_dev;




vector<float> local_mean_trends;
vector<float> local_trend_std_devs;

size_t trend_minimum_samples = 20;


bool spatial_interpolation = true;
bool curved_triangles = true;

size_t min_year, max_year, curr_year, trends_first_year, trends_last_year;
size_t curr_month = 0;
size_t selected_vertex = 0;
char month_names[12][4] = {
	"Jan", "Feb", "Mar", "Apr",
	"May", "Jun", "Jul", "Aug",
	"Sep", "Oct", "Nov", "Dec"
};







// OpenGL data and functions
vector_3 background_colour(0.6, 0.6, 0.6);
float vertex_colour0[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float selected_vertex_colour[] = { 1.0f, 1.0f, 1.0f, 0.2 };


float vertex_size = 0.0025;
size_t vertex_slices = 5;
size_t vertex_stacks = 4;

float outline_width = 1;
float outline_colour[] = {0, 0, 0};

vector_3 control_list_colour(0, 0, 0);
float triangle_colour[] = { 0.5f, 0.5f, 0.5f, 1.0f };

bool draw_axis = true;
bool draw_control_list = false;
bool draw_tri_outlines = true;
bool disable_lighting = true;
bool draw_vertices = false;
bool draw_trend_data = false;

uv_camera main_camera;

GLint win_id = 0;
GLint win_x = 800, win_y = 600;
float camera_w = 3;
float camera_fov = 45;
float camera_x_transform = 0;
float camera_y_transform = 0;
double u_spacer = 0.01;
double v_spacer = 0.5*u_spacer;
double w_spacer = 0.1;
double camera_near = 0.01;
double camera_far = 10.0;

GLUquadricObj* glu_obj = gluNewQuadric(); // Probably should delete this before app exit... :)

bool lmb_down = false;
bool mmb_down = false;
bool rmb_down = false;
int mouse_x = 0;
int mouse_y = 0;

void idle_func(void);
void init_opengl(const int &width, const int &height);
void reshape_func(int width, int height);
void display_func(void);
void keyboard_func(unsigned char key, int x, int y);
void mouse_func(int button, int state, int x, int y);
void motion_func(int x, int y);
void passive_motion_func(int x, int y);
void draw_objects(void);






class RGB
{
public:
	unsigned char r, g, b;
};

RGB HSBtoRGB(unsigned short int hue_degree, unsigned char sat_percent, unsigned char bri_percent)
{
	float R = 0.0f;
	float G = 0.0f;
	float B = 0.0f;

	if(hue_degree > 359)
		hue_degree = 359;

	if(sat_percent > 100)
		sat_percent = 100;

	if(bri_percent > 100)
		bri_percent = 100;

	float hue_pos = 6.0f - ((static_cast<float>(hue_degree) / 359.0f) * 6.0f);

	if(hue_pos >= 0.0f && hue_pos < 1.0f)
	{
		R = 255.0f;
		G = 0.0f;
		B = 255.0f * hue_pos;
	}
	else if(hue_pos >= 1.0f && hue_pos < 2.0f)
	{
		hue_pos -= 1.0f;

		R = 255.0f - (255.0f * hue_pos);
		G = 0.0f;
		B = 255.0f;
	}
	else if(hue_pos >= 2.0f && hue_pos < 3.0f)
	{
		hue_pos -= 2.0f;

		R = 0.0f;
		G = 255.0f * hue_pos;
		B = 255.0f;
	}
	else if(hue_pos >= 3.0f && hue_pos < 4.0f)
	{
		hue_pos -= 3.0f;

		R = 0.0f;
		G = 255.0f;
		B = 255.0f - (255.0f * hue_pos);
	}
	else if(hue_pos >= 4.0f && hue_pos < 5.0f)
	{
		hue_pos -= 4.0f;

		R = 255.0f * hue_pos;
		G = 255.0f;
		B = 0.0f;
	}
	else
	{
		hue_pos -= 5.0f;

		R = 255.0f;
		G = 255.0f - (255.0f * hue_pos);
		B = 0.0f;
	}

	if(100 != sat_percent)
	{
		if(0 == sat_percent)
		{
			R = 255.0f;
			G = 255.0f;
			B = 255.0f;
		}
		else
		{
			if(255.0f != R)
				R += ((255.0f - R) / 100.0f) * (100.0f - sat_percent);
			if(255.0f != G)
				G += ((255.0f - G) / 100.0f) * (100.0f - sat_percent);
			if(255.0f != B)
				B += ((255.0f - B) / 100.0f) * (100.0f - sat_percent);
		}
	}

	if(100 != bri_percent)
	{
		if(0 == bri_percent)
		{
			R = 0.0f;
			G = 0.0f;
			B = 0.0f;
		}
		else
		{
			if(0.0f != R)
				R *= static_cast<float>(bri_percent)/100.0f;
			if(0.0f != G)
				G *= static_cast<float>(bri_percent)/100.0f;
			if(0.0f != B)
				B *= static_cast<float>(bri_percent)/100.0f;
		}
	}

	if(R < 0.0f)
		R = 0.0f;
	else if(R > 255.0f)
		R = 255.0f;

	if(G < 0.0f)
		G = 0.0f;
	else if(G > 255.0f)
		G = 255.0f;

	if(B < 0.0f)
		B = 0.0f;
	else if(B > 255.0f)
		B = 255.0f;

	RGB rgb;

	rgb.r = static_cast<unsigned char>(R);
	rgb.g = static_cast<unsigned char>(G);
	rgb.b = static_cast<unsigned char>(B);

	return rgb;
}


void generate_trend_materials(void)
{
	global_mean_trend = 0;

	global_min_trend = 1000;
	global_max_trend = -1000;
	vector<float> global_trends;

	for(size_t i = 0; i < sd.size(); i++)
	{
		vector<float> station_trends;
		get_local_trends(sd, i, trends_first_year, trends_last_year, station_trends, trend_minimum_samples);

		if(station_trends.size() == 0)
		{
			local_mean_trends[i] = -99;
			continue;
		}

		local_mean_trends[i] = 0;

#define DONT_AVERAGE_LOCAL_TRENDS

		for(size_t j = 0; j < station_trends.size(); j++)
		{

#ifdef DONT_AVERAGE_LOCAL_TRENDS

			if(station_trends[j] < global_min_trend)
				global_min_trend = station_trends[j];

			if(station_trends[j] > global_max_trend)
				global_max_trend = station_trends[j];

			global_trends.push_back(station_trends[j]);
			global_mean_trend += station_trends[j];

#endif

			local_mean_trends[i] += station_trends[j];
		}

		local_mean_trends[i] /= static_cast<float>(station_trends.size());
		local_trend_std_devs[i] = standard_deviation(station_trends);

#ifndef DONT_AVERAGE_LOCAL_TRENDS

		if(local_mean_trends[i] < global_min_trend)
			global_min_trend = local_mean_trends[i];

		if(local_mean_trends[i] > global_max_trend)
			global_max_trend = local_mean_trends[i];

		global_trends.push_back(local_mean_trends[i]);
		global_mean_trend += local_mean_trends[i];

#endif


	}

	if(0 != global_trends.size())
	{
		global_mean_trend /= static_cast<float>(global_trends.size());
		global_mean_trend_std_dev = standard_deviation(global_trends);
	}



	static const double invalid_data_shade = 0.2;

	set<size_t> invalid_vertices;

	for(size_t i = 0; i < tess.vertices.size(); i++)
	{
		if(-99 == local_mean_trends[i])
		{
			invalid_vertices.insert(i);

			materials[i][0] = invalid_data_shade;
			materials[i][1] = invalid_data_shade;
			materials[i][2] = invalid_data_shade;
			materials[i][3] = 1;
		}
		else
		{
			double trend = local_mean_trends[i];

			static const double trend_cap = 0.02;
			static RGB rgb;

			if(trend > trend_cap)
				trend = trend_cap;

			if(trend < -trend_cap)
				trend = -trend_cap;

			//trend = -trend;

			//trend += trend_cap;
			//trend /= trend_cap*2.0;

			//rgb = HSBtoRGB(static_cast<short unsigned int>(trend*260.0), 66, 100);

			//materials[i][0] = static_cast<double>(rgb.r)/255.0;
			//materials[i][1] = static_cast<double>(rgb.g)/255.0;
			//materials[i][2] = static_cast<double>(rgb.b)/255.0;
			//materials[i][3] = 1;

			if(0 == trend)
			{
				materials[i][0] = 1;
				materials[i][1] = 1;
				materials[i][2] = 1;
				materials[i][3] = 1;
			}
			else if(trend > 0)
			{
				materials[i][0] = 1;
				materials[i][1] = (1 - trend/trend_cap);
				materials[i][2] = (1 - trend/trend_cap);
				materials[i][3] = 1;
			}
			else
			{
				materials[i][0] = (1 + trend/trend_cap);
				materials[i][1] = (1 + trend/trend_cap);
				materials[i][2] = 1;
				materials[i][3] = 1;
			}

			materials[i][0] *= 2;
			materials[i][1] *= 2;
			materials[i][2] *= 2;

			materials[i][0] += 1;
			materials[i][1] += 1;
			materials[i][2] += 1;

			materials[i][0] /= 3.0f;
			materials[i][1] /= 3.0f;
			materials[i][2] /= 3.0f;
		}
	}

	if(true == spatial_interpolation)
	{
		size_t last_invalid_vertices_size = invalid_vertices.size();

		while(0 != invalid_vertices.size())
		{
			set<size_t> new_invalid_vertices;

			for(set<size_t>::const_iterator ci = invalid_vertices.begin(); ci != invalid_vertices.end(); ci++)
			{
				size_t valid_neighbour_count = 0;
				
				materials[*ci][0] = 0;
				materials[*ci][1] = 0;
				materials[*ci][2] = 0;
				materials[*ci][3] = 0;

				for(size_t j = 0; j < vertex_to_vertex[*ci].size(); j++)
				{
					if(invalid_vertices.end() == invalid_vertices.find(vertex_to_vertex[*ci][j]))
					{
						valid_neighbour_count++;

						materials[*ci][0] += materials[vertex_to_vertex[*ci][j]][0];
						materials[*ci][1] += materials[vertex_to_vertex[*ci][j]][1];
						materials[*ci][2] += materials[vertex_to_vertex[*ci][j]][2];
						materials[*ci][3] += materials[vertex_to_vertex[*ci][j]][3];
					}
				}

				if(0 == valid_neighbour_count)
				{
					new_invalid_vertices.insert(*ci);
					materials[*ci][0] = invalid_data_shade;
					materials[*ci][1] = invalid_data_shade;
					materials[*ci][2] = invalid_data_shade;
					materials[*ci][3] = 1;
				}
				else
				{
					materials[*ci][0] /= valid_neighbour_count;
					materials[*ci][1] /= valid_neighbour_count;
					materials[*ci][2] /= valid_neighbour_count;
					materials[*ci][3] /= valid_neighbour_count;
				}
			}

			invalid_vertices.swap(new_invalid_vertices);

			if(invalid_vertices.size() == last_invalid_vertices_size)
				break;
			else
				last_invalid_vertices_size = invalid_vertices.size();
		}
	}

	if(true == curved_triangles)
	{
		for(size_t i = 0; i < ctris.size(); i++)
			ctris[i].init_mats(materials[ctris[i].seed_i0], materials[ctris[i].seed_i1], materials[ctris[i].seed_i2]);
	}
}

void generate_temperature_materials(void)
{
	static const double invalid_data_shade = 0.75;

	global_min_temp = 1000;
	global_max_temp = -1000;
	vector<float> temperatures;

	set<size_t> invalid_vertices;

	for(size_t i = 0; i < tess.vertices.size(); i++)
	{
		if(sd[i].years.end() == sd[i].years.find(curr_year))
		{
			if(true == spatial_interpolation)
				invalid_vertices.insert(i);

			materials[i][0] = invalid_data_shade;
			materials[i][1] = invalid_data_shade;
			materials[i][2] = invalid_data_shade;
			materials[i][3] = 1;
		}
		else
		{
			double temp = sd[i].years[curr_year].temperatures[curr_month];

			if(temp == -99)
			{
				if(true == spatial_interpolation)
					invalid_vertices.insert(i);

				materials[i][0] = invalid_data_shade;
				materials[i][1] = invalid_data_shade;
				materials[i][2] = invalid_data_shade;
				materials[i][3] = 1;
			}
			else
			{
				if(temp < global_min_temp)
					global_min_temp = temp;

				if(temp > global_max_temp)
					global_max_temp = temp;

				temperatures.push_back(temp);



				static const double temp_cap = 30;
				static RGB rgb;

				if(temp > temp_cap)
					temp = temp_cap;

				if(temp < -temp_cap)
					temp = -temp_cap;

				temp = -temp;

				temp += temp_cap;
				temp /= temp_cap*2.0;

				rgb = HSBtoRGB(static_cast<short unsigned int>(temp*260.0), 66, 100);

				materials[i][0] = static_cast<double>(rgb.r)/255.0;
				materials[i][1] = static_cast<double>(rgb.g)/255.0;
				materials[i][2] = static_cast<double>(rgb.b)/255.0;
				materials[i][3] = 1;
			}
		}
	}

	global_mean_temp = 0;

	for(size_t i = 0; i < temperatures.size(); i++)
		global_mean_temp += temperatures[i];

	global_mean_temp /= static_cast<double>(temperatures.size());
	global_mean_temp_std_dev = standard_deviation(temperatures);



	if(true == spatial_interpolation)
	{
		size_t last_invalid_vertices_size = invalid_vertices.size();

		while(0 != invalid_vertices.size())
		{
			set<size_t> new_invalid_vertices;

			for(set<size_t>::const_iterator ci = invalid_vertices.begin(); ci != invalid_vertices.end(); ci++)
			{
				size_t valid_neighbour_count = 0;
				
				materials[*ci][0] = 0;
				materials[*ci][1] = 0;
				materials[*ci][2] = 0;
				materials[*ci][3] = 0;

				for(size_t j = 0; j < vertex_to_vertex[*ci].size(); j++)
				{
					if(invalid_vertices.end() == invalid_vertices.find(vertex_to_vertex[*ci][j]))
					{
						valid_neighbour_count++;

						materials[*ci][0] += materials[vertex_to_vertex[*ci][j]][0];
						materials[*ci][1] += materials[vertex_to_vertex[*ci][j]][1];
						materials[*ci][2] += materials[vertex_to_vertex[*ci][j]][2];
						materials[*ci][3] += materials[vertex_to_vertex[*ci][j]][3];
					}
				}

				if(0 == valid_neighbour_count)
				{
					new_invalid_vertices.insert(*ci);
					materials[*ci][0] = invalid_data_shade;
					materials[*ci][1] = invalid_data_shade;
					materials[*ci][2] = invalid_data_shade;
					materials[*ci][3] = 1;
				}
				else
				{
					materials[*ci][0] /= valid_neighbour_count;
					materials[*ci][1] /= valid_neighbour_count;
					materials[*ci][2] /= valid_neighbour_count;
					materials[*ci][3] /= valid_neighbour_count;
				}
			}

			invalid_vertices.swap(new_invalid_vertices);

			if(invalid_vertices.size() == last_invalid_vertices_size)
				break;
			else
				last_invalid_vertices_size = invalid_vertices.size();
		}
	}

	if(true == curved_triangles)
	{
		for(size_t i = 0; i < ctris.size(); i++)
			ctris[i].init_mats(materials[ctris[i].seed_i0], materials[ctris[i].seed_i1], materials[ctris[i].seed_i2]);
	}
}


void generate_materials(void)
{
	if(false == draw_trend_data)
		generate_temperature_materials();
	else
		generate_trend_materials();

}




void latlon_to_xyz(const double &lat, const double &lon, double &x, double &y, double &z)
{
	double theta = 2*pi*(lon + 180.0)/360.0;
	double phi = pi*(lat + 90.0)/180.0;

	x = -(cos(theta)*sin(phi));
	z = -(sin(theta)*sin(phi));
	y = -cos(phi);
}


void xyz_to_latlon(const double &x, const double &y, const double &z, double &lat, double &lon)
{
	double theta = pi + atan2(z, x);
	double phi = acos(-y);

	lat = phi/pi*180.0 - 90.0;
	lon = theta/(2*pi)*360.0 - 180.0;
}



void idle_func(void)
{
	glutPostRedisplay();
}

void init_opengl(const int &width, const int &height)
{
	win_x = width;
	win_y = height;

	if(win_x < 1)
		win_x = 1;

	if(win_y < 1)
		win_y = 1;

	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_ALPHA|GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(win_x, win_y);
	win_id = glutCreateWindow("HadCRUT3 viewer 1.03");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_POLYGON_SMOOTH);

	//float light0_colour[] = {1.0f, 0.5f, 0.5f, 1.0f};
	//float light1_colour[] = {0.5f, 1.0f, 1.0f, 1.0f};
	//float light2_colour[] = {0.5f, 1.0f, 0.5f, 1.0f};
	//float light3_colour[] = {1.0f, 0.5f, 1.0f, 1.0f};
	//float light4_colour[] = {0.5f, 0.5f, 1.0f, 1.0f};
	//float light5_colour[] = {1.0f, 1.0f, 0.5f, 1.0f};

	float light0_colour[] = {0.8f, 0.8f, 0.8f, 0.8f};
	float light1_colour[] = {0.8f, 0.8f, 0.8f, 0.8f};
	float light2_colour[] = {0.8f, 0.8f, 0.8f, 0.8f};
	float light3_colour[] = {0.8f, 0.8f, 0.8f, 0.8f};
	float light4_colour[] = {0.8f, 0.8f, 0.8f, 0.8f};
	float light5_colour[] = {0.8f, 0.8f, 0.8f, 0.8f};


	float light0_position[] = {1.0f, 0.0f, 0.0f, 0.0f };
	float light1_position[] = {-1.0f, 0.0f, 0.0f, 0.0f };
	float light2_position[] = {0.0f, 1.0f, 0.0f, 0.0f };
	float light3_position[] = {0.0f, -1.0f, 0.0f, 0.0f };
	float light4_position[] = {0.0f, 0.0f, 1.0f, 0.0f };
	float light5_position[] = {0.0f, 0.0f, -1.0f, 0.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_colour);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_colour);
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_colour);
	glLightfv(GL_LIGHT3, GL_POSITION, light3_position);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, light3_colour);
	glLightfv(GL_LIGHT4, GL_POSITION, light4_position);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, light4_colour);
	glLightfv(GL_LIGHT5, GL_POSITION, light5_position);
	glLightfv(GL_LIGHT5, GL_DIFFUSE, light5_colour);

	glClearColor(background_colour.x, background_colour.y, background_colour.z, 1);
	glClearDepth(1.0f);

	main_camera.Set(0, 0, camera_w, camera_fov, win_x, win_y, camera_near, camera_far);

	float highest_dotsq = -1;

	vector_3 cam;
	cam = main_camera.eye;
	cam.normalize();

	for(size_t i = 0; i < tess.vertices.size(); i++)
	{
		float dotsq = cam.x*tess.vertices[i].x + cam.y*tess.vertices[i].y + cam.z*tess.vertices[i].z;

		if(dotsq > highest_dotsq)
		{
			highest_dotsq = dotsq;
			selected_vertex = i;
		}
	}


}

void reshape_func(int width, int height)
{
	win_x = width;
	win_y = height;

	if(win_x < 1)
		win_x = 1;

	if(win_y < 1)
		win_y = 1;

	glutSetWindow(win_id);
	glutReshapeWindow(win_x, win_y);
	glViewport(0, 0, win_x, win_y);

	main_camera.Set(main_camera.u, main_camera.v, main_camera.w, main_camera.fov, win_x, win_y, camera_near, camera_far);
}

// Text drawing code originally from "GLUT Tutorial -- Bitmap Fonts and Orthogonal Projections" by A R Fernandes
void render_string(int x, const int y, void *font, const string &text)
{
	for(size_t i = 0; i < text.length(); i++)
	{
		glRasterPos2i(x, y);
		glutBitmapCharacter(font, text[i]);
		x += glutBitmapWidth(font, text[i]) + 1;
	}
}
// End text drawing code.

void display_func(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	draw_objects();


	glDisable(GL_LIGHTING);

	// Text drawing code originally from "GLUT Tutorial -- Bitmap Fonts and Orthogonal Projections" by A R Fernandes
	// http://www.lighthouse3d.com/opengl/glut/index.php?bmpfontortho
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, win_x, 0, win_y);
	glScalef(1, -1, 1); // Neat. :)
	glTranslatef(0, -win_y, 0); // Neat. :)
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(control_list_colour.x, control_list_colour.y, control_list_colour.z);

	size_t break_size = 20;
	size_t start = 20;

	ostringstream oss;
	oss.precision(4);

	if(false == draw_trend_data)
	{
		render_string(10, start, GLUT_BITMAP_HELVETICA_10, string("Drawing temperature data."));

		if(true == draw_control_list)
		{
			render_string(10, start + 2*break_size, GLUT_BITMAP_HELVETICA_10, string("Keyboard controls:"));
			render_string(10, start + 3*break_size, GLUT_BITMAP_HELVETICA_10, string("Q/W: Decade -/+"));
			render_string(10, start + 4*break_size, GLUT_BITMAP_HELVETICA_10, string("A/S: Year -/+"));
			render_string(10, start + 5*break_size, GLUT_BITMAP_HELVETICA_10, string("Z/X: Month -/+"));

			render_string(10, start + 7*break_size, GLUT_BITMAP_HELVETICA_10, string("D: Draw linear trend data"));

			render_string(10, start + 9*break_size, GLUT_BITMAP_HELVETICA_10, string("F: Curved triangles"));
			render_string(10, start + 10*break_size, GLUT_BITMAP_HELVETICA_10, string("G: Spatial interpolation"));
			render_string(10, start + 11*break_size, GLUT_BITMAP_HELVETICA_10, string("H: Draw vertices"));
			render_string(10, start + 12*break_size, GLUT_BITMAP_HELVETICA_10, string("J: Draw triangle outlines"));
			render_string(10, start + 13*break_size, GLUT_BITMAP_HELVETICA_10, string("K: Draw axis"));
			render_string(10, start + 14*break_size, GLUT_BITMAP_HELVETICA_10, string("L: Shrink control list"));
			render_string(10, start + 15*break_size, GLUT_BITMAP_HELVETICA_10, string(";: Calculate lighting"));
		}
		else
		{
			render_string(10, start + 2*break_size, GLUT_BITMAP_HELVETICA_10, string("Keyboard controls:"));
			render_string(10, start + 3*break_size, GLUT_BITMAP_HELVETICA_10, string("L: Expand control list"));

			if(718660 == sd[selected_vertex].station_id)
				render_string(10, start + 5*break_size, GLUT_BITMAP_HELVETICA_10, string("Paris of the prairies!"));
		}


		oss << "Total stations = " << tess.vertices.size();
		render_string(10, win_y - (start + 5*break_size), GLUT_BITMAP_HELVETICA_10, oss.str());
		oss.str("");
		oss.clear();

		oss << "Date = " << month_names[curr_month] << ' ' << curr_year;
		render_string(10, win_y - (start + 4*break_size), GLUT_BITMAP_HELVETICA_10, oss.str());
		oss.str("");
		oss.clear();

		oss << "Station = " << sd[selected_vertex].station_id << ' ' << sd[selected_vertex].name << ' ' << sd[selected_vertex].country << endl;
		render_string(10, win_y - (start + 3*break_size), GLUT_BITMAP_HELVETICA_10, oss.str());
		oss.str("");
		oss.clear();

		oss << "Lat Lon = " << sd[selected_vertex].latitude << ' ' << sd[selected_vertex].longitude << endl;
		render_string(10, win_y - (start + 2*break_size), GLUT_BITMAP_HELVETICA_10, oss.str());
		oss.str("");
		oss.clear();

		if(sd[selected_vertex].years.end() == sd[selected_vertex].years.find(curr_year))
		{
			oss << "Local temp = Not available for this year" << endl;
			render_string(10, win_y - (start + 1*break_size), GLUT_BITMAP_HELVETICA_10, oss.str());
			oss.str("");
			oss.clear();
		}
		else
		{
			double temp = sd[selected_vertex].years[curr_year].temperatures[curr_month];

			if(temp == -99)
			{
				oss << "Local temp = Not available for this month" << endl;
				render_string(10, win_y - (start + 1*break_size), GLUT_BITMAP_HELVETICA_10, oss.str());
				oss.str("");
				oss.clear();
			}
			else
			{
				oss << "Local temp = " << temp << " C" << endl;
				render_string(10, win_y - (start + 1*break_size), GLUT_BITMAP_HELVETICA_10, oss.str());
				oss.str("");
				oss.clear();
			}
		}

		oss << "Global min, mean, max temp = " << global_min_temp << ", " << global_mean_temp << " +/- " << global_mean_temp_std_dev << ", " << global_max_temp << " C" << endl;
		render_string(10, win_y - (start + 0*break_size), GLUT_BITMAP_HELVETICA_10, oss.str());
		oss.str("");
		oss.clear();

	}
	else
	{
		oss << "Drawing warming/cooling linear trend data." << endl;
		render_string(10, start, GLUT_BITMAP_HELVETICA_10, oss.str());
		oss.str("");
		oss.clear();

		oss << "Trend sample minimum = " << trend_minimum_samples;
		render_string(10, start + 1*break_size, GLUT_BITMAP_HELVETICA_10, oss.str());
		oss.str("");
		oss.clear();



		if(true == draw_control_list)
		{
			render_string(10, start + 3*break_size, GLUT_BITMAP_HELVETICA_10, string("Keyboard controls:"));
			render_string(10, start + 4*break_size, GLUT_BITMAP_HELVETICA_10, string("1/2: First trend decade -/+"));
			render_string(10, start + 5*break_size, GLUT_BITMAP_HELVETICA_10, string("Q/W: First trend year -/+"));
			render_string(10, start + 6*break_size, GLUT_BITMAP_HELVETICA_10, string("A/S: Last trend decade -/+"));
			render_string(10, start + 7*break_size, GLUT_BITMAP_HELVETICA_10, string("Z/X: Last trend year -/+"));
			render_string(10, start + 8*break_size, GLUT_BITMAP_HELVETICA_10, string("3/4: Trend sample minimum -/+"));

			render_string(10, start + 10*break_size, GLUT_BITMAP_HELVETICA_10, string("D: Draw temperature data"));

			render_string(10, start + 12*break_size, GLUT_BITMAP_HELVETICA_10, string("F: Curved triangles"));
			render_string(10, start + 13*break_size, GLUT_BITMAP_HELVETICA_10, string("G: Spatial interpolation"));
			render_string(10, start + 14*break_size, GLUT_BITMAP_HELVETICA_10, string("H: Draw vertices"));
			render_string(10, start + 15*break_size, GLUT_BITMAP_HELVETICA_10, string("J: Draw triangle outlines"));
			render_string(10, start + 16*break_size, GLUT_BITMAP_HELVETICA_10, string("K: Draw axis"));
			render_string(10, start + 17*break_size, GLUT_BITMAP_HELVETICA_10, string("L: Shrink control list"));
			render_string(10, start + 18*break_size, GLUT_BITMAP_HELVETICA_10, string(";: Calculate lighting"));
		}
		else
		{
			render_string(10, start + 3*break_size, GLUT_BITMAP_HELVETICA_10, string("Keyboard controls:"));
			render_string(10, start + 4*break_size, GLUT_BITMAP_HELVETICA_10, string("L: Expand control list"));

			if(718660 == sd[selected_vertex].station_id)
				render_string(10, start + 6*break_size, GLUT_BITMAP_HELVETICA_10, string("Go Blades!"));
		}


		oss << "Total stations = " << tess.vertices.size();
		render_string(10, win_y - (start + 5*break_size), GLUT_BITMAP_HELVETICA_10, oss.str());
		oss.str("");
		oss.clear();

		oss << "Trend years = [" << trends_first_year << ", " << trends_last_year << ']';
		render_string(10, win_y - (start + 4*break_size), GLUT_BITMAP_HELVETICA_10, oss.str());
		oss.str("");
		oss.clear();

		oss << "Station = " << sd[selected_vertex].station_id << ' ' << sd[selected_vertex].name << ' ' << sd[selected_vertex].country << endl;
		render_string(10, win_y - (start + 3*break_size), GLUT_BITMAP_HELVETICA_10, oss.str());
		oss.str("");
		oss.clear();

		oss << "Lat Lon = " << sd[selected_vertex].latitude << ' ' << sd[selected_vertex].longitude << endl;
		render_string(10, win_y - (start + 2*break_size), GLUT_BITMAP_HELVETICA_10, oss.str());
		oss.str("");
		oss.clear();

		if(-99 == local_mean_trends[selected_vertex])
			oss << "Local mean trend = Not available for this station" << endl;
		else
			oss << "Local mean trend = " << local_mean_trends[selected_vertex]*100.0f << " +/- " << local_trend_std_devs[selected_vertex]*100.0f << " C/century" << endl;

		render_string(10, win_y - (start + 1*break_size), GLUT_BITMAP_HELVETICA_10, oss.str());
		oss.str("");
		oss.clear();

		oss << "Global min, mean, max trend = " << global_min_trend*100.0f << ", " << global_mean_trend*100.0f << " +/- " << global_mean_trend_std_dev*100.0f << ", " << global_max_trend*100.0f << " C/century" << endl;
		render_string(10, win_y - (start + 0*break_size), GLUT_BITMAP_HELVETICA_10, oss.str());
		oss.str("");
		oss.clear();




	}


	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	//glMatrixMode(GL_MODELVIEW);
	// End text drawing code.


	glFlush();
	glutSwapBuffers();
}

void keyboard_func(unsigned char key, int x, int y)
{
	switch(tolower(key))
	{
	case '1':
		{
			if(trends_first_year > min_year + 10)
			{
				trends_first_year -= 10;
				generate_materials();
			}
			else if(trends_first_year != min_year)
			{
				trends_first_year = min_year;
				generate_materials();
			}

			break;
		}
	case '2':
		{
			if(trends_first_year < max_year - 10)
			{
				trends_first_year += 10;

				if(trends_first_year > trends_last_year)
					trends_last_year = trends_first_year;

				generate_materials();
			}
			else if(trends_first_year != max_year)
			{
				trends_first_year = max_year;

				if(trends_first_year > trends_last_year)
					trends_last_year = trends_first_year;

				generate_materials();
			}

			break;
		}
	case '3':
		{
			if(trend_minimum_samples > 2)
			{
				trend_minimum_samples--;
				generate_materials();
			}

			break;
		}
	case '4':
		{
			trend_minimum_samples++;
			generate_materials();
			break;
		}

	case 'q':
		{
			if(false == draw_trend_data)
			{
				if(curr_year > min_year + 10)
				{
					curr_year -= 10;
					generate_materials();
				}
				else if(curr_year != min_year)
				{
					curr_year = min_year;
					generate_materials();
				}
			}
			else
			{
				if(trends_first_year > min_year)
				{
					trends_first_year--;
					generate_materials();
				}
			}

			break;
		}
	case 'w':
		{
			if(false == draw_trend_data)
			{
				if(curr_year < max_year - 10)
				{
					curr_year += 10;
					generate_materials();
				}
				else if(curr_year != max_year)
				{
					curr_year = max_year;
					generate_materials();
				}
			}
			else
			{
				if(trends_first_year < max_year)
				{
					trends_first_year++;

					if(trends_first_year > trends_last_year)
						trends_last_year = trends_first_year;

					generate_materials();
				}
			}

			break;
		}

	case 'a':
		{
			if(false == draw_trend_data)
			{
				if(curr_year > min_year)
				{
					curr_year--;
					generate_materials();
				}
			}
			else
			{
				if(trends_last_year > min_year + 10)
				{
					trends_last_year -= 10;

					if(trends_last_year < trends_first_year)
						trends_first_year = trends_last_year;

					generate_materials();
				}
				else if(trends_last_year != min_year)
				{
					trends_last_year = min_year;

					if(trends_last_year < trends_first_year)
						trends_first_year = trends_last_year;

					generate_materials();
				}
			}

			break;
		}
	case 's':
		{
			if(false == draw_trend_data)
			{
				if(curr_year < max_year)
				{
					curr_year++;
					generate_materials();
				}
			}
			else
			{
				if(trends_last_year < max_year - 10)
				{
					trends_last_year += 10;
					generate_materials();
				}
				else if(trends_last_year != max_year)
				{
					trends_last_year = max_year;
					generate_materials();
				}
			}

			break;
		}


	case 'z':
		{
			if(false == draw_trend_data)
			{
				if(curr_month > 0)
				{
					curr_month--;
					generate_materials();
				}
				else if(curr_month == 0)
				{
					if(curr_year > min_year)
					{
						curr_month = 11;
						curr_year--;
						generate_materials();
					}
				}
			}
			else
			{
				if(trends_last_year > min_year)
				{
					trends_last_year--;

					if(trends_last_year < trends_first_year)
						trends_first_year = trends_last_year;

					generate_materials();
				}
			}

			break;
		}
	
	case 'x':
		{
			if(false == draw_trend_data)
			{
				if(curr_month < 11)
				{
					curr_month++;
					generate_materials();
				}
				else if(curr_month == 11)
				{
					if(curr_year < max_year)
					{
						curr_month = 0;
						curr_year++;
						generate_materials();
					}
				}
			}
			else
			{
				if(trends_last_year < max_year)
				{
					trends_last_year++;
					generate_materials();
				}
			}

			break;
		}

	case 'd':
		{
			draw_trend_data = !draw_trend_data;
			generate_materials();
			break;
		}
	case 'f':
		{
			curved_triangles = !curved_triangles;
			generate_materials();
			break;
		}
	case 'g':
		{
			spatial_interpolation = !spatial_interpolation;
			generate_materials();
			break;
		}
	case 'h':
		{
			draw_vertices = !draw_vertices;
			generate_materials();
			break;
		}

	case 'j':
		{
			draw_tri_outlines = !draw_tri_outlines;
			break;
		}
	case 'k':
		{
			draw_axis = !draw_axis;
			break;
		}
	case 'l':
		{
			draw_control_list = !draw_control_list;
			break;
		}
	case ';':
		{
			disable_lighting = !disable_lighting;
			break;
		}

	default:
		break;
	}
}

void mouse_func(int button, int state, int x, int y)
{
	if(GLUT_LEFT_BUTTON == button)
	{
		if(GLUT_DOWN == state)
			lmb_down = true;
		else
			lmb_down = false;
	}
	else if(GLUT_MIDDLE_BUTTON == button)
	{
		if(GLUT_DOWN == state)
			mmb_down = true;
		else
			mmb_down = false;
	}
	else if(GLUT_RIGHT_BUTTON == button)
	{
		if(GLUT_DOWN == state)
			rmb_down = true;
		else
			rmb_down = false;
	}
}

void motion_func(int x, int y)
{
	int prev_mouse_x = mouse_x;
	int prev_mouse_y = mouse_y;

	mouse_x = x;
	mouse_y = y;

	int mouse_delta_x = mouse_x - prev_mouse_x;
	int mouse_delta_y = prev_mouse_y - mouse_y;

	if(true == lmb_down && (0 != mouse_delta_x || 0 != mouse_delta_y))
	{
		if(main_camera.w < 1.5f)
		{
			main_camera.u -= static_cast<float>(mouse_delta_y)*u_spacer/10.0f;
			main_camera.v += static_cast<float>(mouse_delta_x)*v_spacer/10.0f;
		}
		else
		{
			main_camera.u -= static_cast<float>(mouse_delta_y)*u_spacer;
			main_camera.v += static_cast<float>(mouse_delta_x)*v_spacer;
		}

		float highest_dotsq = -1;

		vector_3 cam;
		cam = main_camera.eye;
		cam.normalize();

		for(size_t i = 0; i < tess.vertices.size(); i++)
		{
			float dotsq = cam.x*tess.vertices[i].x + cam.y*tess.vertices[i].y + cam.z*tess.vertices[i].z;

			if(dotsq > highest_dotsq)
			{
				highest_dotsq = dotsq;
				selected_vertex = i;
			}
		}
	}
	else if(true == rmb_down && (0 != mouse_delta_y))
	{
		main_camera.w -= static_cast<float>(mouse_delta_y)*w_spacer;

		if(main_camera.w < 1.1f)
			main_camera.w = 1.1f;
		else if(main_camera.w > 8.0f)
			main_camera.w = 8.0f;
	}

	main_camera.Set(); // Calculate new camera vectors.
}

void passive_motion_func(int x, int y)
{
	mouse_x = x;
	mouse_y = y;
}

void draw_objects(void)
{
	if(false == disable_lighting)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		glEnable(GL_LIGHT3);
		glEnable(GL_LIGHT4);
		glEnable(GL_LIGHT5);
	}
	else
	{
		glDisable(GL_LIGHTING);
	}

	static const float rad_to_deg = 180.0f/pi;

	glPushMatrix();

	glTranslatef(camera_x_transform, camera_y_transform, 0);
	

	// Draw vertices
	if(true == draw_vertices)
	{
		for(size_t i = 0; i < tess.vertices.size(); i++)
		{
			if(false == disable_lighting)
			{
				float temp_mat[4];
				temp_mat[0] = 1 - materials[i][0];
				temp_mat[1] = 1 - materials[i][1];
				temp_mat[2] = 1 - materials[i][2];
				temp_mat[3] = 1;

				glMaterialfv(GL_FRONT, GL_DIFFUSE, &temp_mat[0]);
			}
			else
				glColor3f(1 - materials[i][0], 1 - materials[i][1], 1 - materials[i][2]);

			glPushMatrix();
			glTranslatef(tess.vertices[i].x, tess.vertices[i].y, tess.vertices[i].z);
			glutSolidSphere(vertex_size, vertex_slices, vertex_stacks);
			glPopMatrix();
		}
	}



	if(true == curved_triangles)
	{
		if(false == disable_lighting)
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);

		glBegin(GL_TRIANGLES);

		for(size_t i = 0; i < ctris.size(); i++)
		{
			// Preliminary backface culling.
			if(0 < main_camera.look_at.dot(ctris[i].circumcentre_normal))
				continue;

			if(false == disable_lighting)
				ctris[i].draw_mat4();
			else
				ctris[i].draw_colour3();
		}

		glEnd();

		if(true == draw_tri_outlines)
		{
			glDisable(GL_LIGHTING);

			glLineWidth(outline_width);

			glColor4f(outline_colour[0], outline_colour[1], outline_colour[2], 0.2f);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glEnable(GL_ALPHA);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glBegin(GL_LINES);

			for(size_t i = 0; i < ctris.size(); i++)
			{




				ctris[i].draw_outline();
			}

			glEnd();

			glDisable(GL_BLEND);
			glDisable(GL_ALPHA);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}



	}
	else
	{
		if(false == disable_lighting)
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);

		glBegin(GL_TRIANGLES);








		for(size_t i = 0; i < tess.dtris.size(); i++)
		{










			if(false == disable_lighting)
				glMaterialfv(GL_FRONT, GL_DIFFUSE, &materials[tess.dtris[i].i0][0]);
			else
				glColor3f(materials[tess.dtris[i].i0][0], materials[tess.dtris[i].i0][1], materials[tess.dtris[i].i0][2]);

			glNormal3f(tess.vertices[tess.dtris[i].i0].x, tess.vertices[tess.dtris[i].i0].y, tess.vertices[tess.dtris[i].i0].z);
			glVertex3f(tess.vertices[tess.dtris[i].i0].x, tess.vertices[tess.dtris[i].i0].y, tess.vertices[tess.dtris[i].i0].z);

			if(false == disable_lighting)
				glMaterialfv(GL_FRONT, GL_DIFFUSE, &materials[tess.dtris[i].i1][0]);
			else
				glColor3f(materials[tess.dtris[i].i1][0], materials[tess.dtris[i].i1][1], materials[tess.dtris[i].i1][2]);

			glNormal3f(tess.vertices[tess.dtris[i].i1].x, tess.vertices[tess.dtris[i].i1].y, tess.vertices[tess.dtris[i].i1].z);
			glVertex3f(tess.vertices[tess.dtris[i].i1].x, tess.vertices[tess.dtris[i].i1].y, tess.vertices[tess.dtris[i].i1].z);

			if(false == disable_lighting)
				glMaterialfv(GL_FRONT, GL_DIFFUSE, &materials[tess.dtris[i].i2][0]);
			else
				glColor3f(materials[tess.dtris[i].i2][0], materials[tess.dtris[i].i2][1], materials[tess.dtris[i].i2][2]);

			glNormal3f(tess.vertices[tess.dtris[i].i2].x, tess.vertices[tess.dtris[i].i2].y, tess.vertices[tess.dtris[i].i2].z);
			glVertex3f(tess.vertices[tess.dtris[i].i2].x, tess.vertices[tess.dtris[i].i2].y, tess.vertices[tess.dtris[i].i2].z);
		}
		glEnd();

		if(true == draw_tri_outlines)
		{
			glDisable(GL_LIGHTING);

			glLineWidth(outline_width);

			glColor4f(outline_colour[0], outline_colour[1], outline_colour[2], 0.2f);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glEnable(GL_ALPHA);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glBegin(GL_TRIANGLES);

			for(size_t i = 0; i < tess.dtris.size(); i++)
			{






				glVertex3f(tess.vertices[tess.dtris[i].i0].x, tess.vertices[tess.dtris[i].i0].y, tess.vertices[tess.dtris[i].i0].z);
				glVertex3f(tess.vertices[tess.dtris[i].i1].x, tess.vertices[tess.dtris[i].i1].y, tess.vertices[tess.dtris[i].i1].z);
				glVertex3f(tess.vertices[tess.dtris[i].i2].x, tess.vertices[tess.dtris[i].i2].y, tess.vertices[tess.dtris[i].i2].z);
			}
			glEnd();

			glDisable(GL_BLEND);
			glDisable(GL_ALPHA);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}


// draw selected vertex
	if(false == disable_lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);


	if(false == disable_lighting)
		glMaterialfv(GL_FRONT, GL_DIFFUSE, vertex_colour0);
	else
		glColor3f(vertex_colour0[0], vertex_colour0[1], vertex_colour0[2]);

	glPushMatrix();
	glTranslatef(tess.vertices[selected_vertex].x, tess.vertices[selected_vertex].y, tess.vertices[selected_vertex].z);
	glutSolidSphere(vertex_size*1, vertex_slices*10, vertex_stacks*10);
	glPopMatrix();



	if(false == disable_lighting)
	{
		float temp_mat[4];
		temp_mat[0] = 1 - materials[selected_vertex][0];
		temp_mat[1] = 1 - materials[selected_vertex][1];
		temp_mat[2] = 1 - materials[selected_vertex][2];
		temp_mat[3] = selected_vertex_colour[3];

		glMaterialfv(GL_FRONT, GL_DIFFUSE, &temp_mat[0]);
	}
	else
		glColor4f(1 - materials[selected_vertex][0], 1 - materials[selected_vertex][1], 1 - materials[selected_vertex][2], selected_vertex_colour[3]);


	glEnable(GL_ALPHA);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glTranslatef(tess.vertices[selected_vertex].x, tess.vertices[selected_vertex].y, tess.vertices[selected_vertex].z);
	glutSolidSphere(vertex_size*2, vertex_slices*10, vertex_stacks*10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(tess.vertices[selected_vertex].x, tess.vertices[selected_vertex].y, tess.vertices[selected_vertex].z);
	glutSolidSphere(vertex_size*3, vertex_slices*10, vertex_stacks*10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(tess.vertices[selected_vertex].x, tess.vertices[selected_vertex].y, tess.vertices[selected_vertex].z);
	glutSolidSphere(vertex_size*4, vertex_slices*10, vertex_stacks*10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(tess.vertices[selected_vertex].x, tess.vertices[selected_vertex].y, tess.vertices[selected_vertex].z);
	glutSolidSphere(vertex_size*5, vertex_slices*10, vertex_stacks*10);
	glPopMatrix();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA);






	// If we do draw the axis at all, make sure not to draw its outline.
	if(draw_axis)
	{
		glDisable(GL_LIGHTING);

		glEnable(GL_ALPHA);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glLineWidth(outline_width*1.5f);

		glBegin(GL_LINES);

//		glColor4f(0, 0, 0, 0.5);

		const float opacity = 0.5;
		const float length = 1.2;

		glColor4f(1, 0, 0, opacity);
		glVertex3f(0, 0, 0);
		glVertex3f(length, 0, 0);

		glColor4f(0, 1, 0, opacity);
		glVertex3f(0, 0, 0);
		glVertex3f(0, length, 0);

		glColor4f(0, 0, 1, opacity);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, length);

		glColor4f(0, 1, 1, opacity);
		glVertex3f(0, 0, 0);
		glVertex3f(-length, 0, 0);

		glColor4f(1, 0, 1, opacity);
		glVertex3f(0, 0, 0);
		glVertex3f(0, -length, 0);

		glColor4f(1, 1, 0, opacity);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, -length);



		//glColor4f(0, 0, 0, 0.25);
		//glVertex3f(0, 0, 0);
		//glVertex3f(-1, 0, 0);
		//glVertex3f(0, 0, 0);
		//glVertex3f(0, -1, 0);
		//glVertex3f(0, 0, 0);
		//glVertex3f(0, 0, -1);

		glEnd();

		glDisable(GL_BLEND);
		glDisable(GL_ALPHA);
	}

	glPopMatrix();
}



#endif