
#ifndef main_H
#define main_H

#include "delaunay_voronoi_on_2sphere.h"
#include "uv_camera.h"


#include "custom_math.h"
using custom_math::indexed_curved_triangle;
using custom_math::d_3;
using custom_math::d_3_sq;
using custom_math::sorted_indexed_edge;

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

//vector<station_data> sd;
delaunay_voronoi_on_2sphere tess;
vector<vector<size_t>> vertex_to_vertex;

vector<indexed_curved_triangle> ctris;
vector<indexed_curved_triangle> vctris;


bool delaunay_mode = false;
bool curved_triangles = true;
size_t selected_vertex = 0;
bool doing_screenshot = false;






// OpenGL data and functions
vector_3 background_colour(0.6, 0.6, 0.6);
float vertex_colour0[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float selected_vertex_colour[] = { 1.0f, 1.0f, 1.0f, 0.2f };


float vertex_size = 0.0025f;
size_t vertex_slices = 5;
size_t vertex_stacks = 4;

float outline_width = 1;
float outline_colour[] = { 0, 0, 0 };

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
double v_spacer = 0.5 * u_spacer;
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
void init_opengl(const int& width, const int& height);
void reshape_func(int width, int height);
void display_func(void);
void keyboard_func(unsigned char key, int x, int y);
void mouse_func(int button, int state, int x, int y);
void motion_func(int x, int y);
void passive_motion_func(int x, int y);
void draw_objects(void);






// TODO: fix camera bug where portrait mode crashes.
void take_screenshot(size_t num_cams_wide, const char* filename, const bool reverse_rows = false)
{
	doing_screenshot = true;

	// Set up Targa TGA image data.
	unsigned char  idlength = 0;
	unsigned char  colourmaptype = 0;
	unsigned char  datatypecode = 2;
	unsigned short int colourmaporigin = 0;
	unsigned short int colourmaplength = 0;
	unsigned char  colourmapdepth = 0;
	unsigned short int x_origin = 0;
	unsigned short int y_origin = 0;
	unsigned short int px = win_x * num_cams_wide;
	unsigned short int py = win_y * num_cams_wide;
	unsigned char  bitsperpixel = 24;
	unsigned char  imagedescriptor = 0;
	vector<char> idstring;

	size_t num_bytes = 3 * px * py;
	vector<unsigned char> pixel_data(num_bytes);

	// Adjust some parameters for large screen format.
	bool temp_draw_control_list = draw_control_list;
	draw_control_list = false;

	float temp_outline_width = outline_width;
	outline_width = 6;

	vector<unsigned char> fbpixels(3 * win_x * win_y);

	// Loop through subcameras.
	for (size_t cam_num_x = 0; cam_num_x < num_cams_wide; cam_num_x++)
	{
		for (size_t cam_num_y = 0; cam_num_y < num_cams_wide; cam_num_y++)
		{
			// Set up camera, draw, then copy the frame buffer.
			main_camera.Set_Large_Screenshot(num_cams_wide, cam_num_x, cam_num_y);
			display_func();
			glReadPixels(0, 0, win_x, win_y, GL_RGB, GL_UNSIGNED_BYTE, &fbpixels[0]);

			// Copy pixels to large image.
			for (GLint i = 0; i < win_x; i++)
			{
				for (GLint j = 0; j < win_y; j++)
				{
					size_t fb_index = 3 * (j * win_x + i);

					size_t screenshot_x = cam_num_x * win_x + i;
					size_t screenshot_y = cam_num_y * win_y + j;
					size_t screenshot_index = 3 * (screenshot_y * (win_x * num_cams_wide) + screenshot_x);

					pixel_data[screenshot_index] = fbpixels[fb_index + 2];
					pixel_data[screenshot_index + 1] = fbpixels[fb_index + 1];
					pixel_data[screenshot_index + 2] = fbpixels[fb_index];
				}
			}
		}
	}

	// Restore the parameters.
	draw_control_list = temp_draw_control_list;
	outline_width = temp_outline_width;
	main_camera.Set();

	// Write Targa TGA file to disk.
	ofstream out(filename, ios::binary);

	if (!out.is_open())
	{
		cout << "Failed to open TGA file for writing: " << filename << endl;
		return;
	}

	out.write(reinterpret_cast<char*>(&idlength), 1);
	out.write(reinterpret_cast<char*>(&colourmaptype), 1);
	out.write(reinterpret_cast<char*>(&datatypecode), 1);
	out.write(reinterpret_cast<char*>(&colourmaporigin), 2);
	out.write(reinterpret_cast<char*>(&colourmaplength), 2);
	out.write(reinterpret_cast<char*>(&colourmapdepth), 1);
	out.write(reinterpret_cast<char*>(&x_origin), 2);
	out.write(reinterpret_cast<char*>(&y_origin), 2);
	out.write(reinterpret_cast<char*>(&px), 2);
	out.write(reinterpret_cast<char*>(&py), 2);
	out.write(reinterpret_cast<char*>(&bitsperpixel), 1);
	out.write(reinterpret_cast<char*>(&imagedescriptor), 1);

	out.write(reinterpret_cast<char*>(&pixel_data[0]), num_bytes);

	doing_screenshot = false;
}


//void generate_trend_materials(void)
//{
//
//
//	static const double invalid_data_shade = 0.2;
//
//	set<size_t> invalid_vertices;
//
//	for (size_t i = 0; i < tess.vertices.size(); i++)
//	{
//		tess_vertices_materials[i][0] = invalid_data_shade;
//		tess_vertices_materials[i][1] = invalid_data_shade;
//		tess_vertices_materials[i][2] = invalid_data_shade;
//		tess_vertices_materials[i][3] = 1;
//	}
//
//	if (1)//true == spatial_interpolation)
//	{
//		size_t last_invalid_vertices_size = invalid_vertices.size();
//
//		while (0 != invalid_vertices.size())
//		{
//			set<size_t> new_invalid_vertices;
//
//			for (set<size_t>::const_iterator ci = invalid_vertices.begin(); ci != invalid_vertices.end(); ci++)
//			{
//				size_t valid_neighbour_count = 0;
//
//				tess_vertices_materials[*ci][0] = 0;
//				tess_vertices_materials[*ci][1] = 0;
//				tess_vertices_materials[*ci][2] = 0;
//				tess_vertices_materials[*ci][3] = 0;
//
//				for (size_t j = 0; j < vertex_to_vertex[*ci].size(); j++)
//				{
//					if (invalid_vertices.end() == invalid_vertices.find(vertex_to_vertex[*ci][j]))
//					{
//						valid_neighbour_count++;
//
//						tess_vertices_materials[*ci][0] += tess_vertices_materials[vertex_to_vertex[*ci][j]][0];
//						tess_vertices_materials[*ci][1] += tess_vertices_materials[vertex_to_vertex[*ci][j]][1];
//						tess_vertices_materials[*ci][2] += tess_vertices_materials[vertex_to_vertex[*ci][j]][2];
//						tess_vertices_materials[*ci][3] += tess_vertices_materials[vertex_to_vertex[*ci][j]][3];
//					}
//				}
//
//				if (0 == valid_neighbour_count)
//				{
//					new_invalid_vertices.insert(*ci);
//					tess_vertices_materials[*ci][0] = invalid_data_shade;
//					tess_vertices_materials[*ci][1] = invalid_data_shade;
//					tess_vertices_materials[*ci][2] = invalid_data_shade;
//					tess_vertices_materials[*ci][3] = 1;
//				}
//				else
//				{
//					tess_vertices_materials[*ci][0] /= valid_neighbour_count;
//					tess_vertices_materials[*ci][1] /= valid_neighbour_count;
//					tess_vertices_materials[*ci][2] /= valid_neighbour_count;
//					tess_vertices_materials[*ci][3] /= valid_neighbour_count;
//				}
//			}
//
//			invalid_vertices.swap(new_invalid_vertices);
//
//			if (invalid_vertices.size() == last_invalid_vertices_size)
//				break;
//			else
//				last_invalid_vertices_size = invalid_vertices.size();
//		}
//	}
//
//	if (true == curved_triangles)
//	{
//		for (size_t i = 0; i < ctris.size(); i++)
//			ctris[i].init_mats(tess_vertices_materials[ctris[i].seed_i0], tess_vertices_materials[ctris[i].seed_i1], tess_vertices_materials[ctris[i].seed_i2]);
//
//		for (size_t i = 0; i < vctris.size(); i++)
//			vctris[i].init_mats(tess_vertices_materials[vctris[i].seed_i0], tess_vertices_materials[vctris[i].seed_i1], tess_vertices_materials[vctris[i].seed_i2]);
//	}
//}
//
//void generate_temperature_materials(void)
//{
//	for (size_t i = 0; i < tess.vertices.size(); i++)
//	{
//		tess_vertices_materials[i][0] = 0.66666;// rand() / static_cast<double>(RAND_MAX);// 0.6666;
//		tess_vertices_materials[i][1] = 0.66666;//rand() / static_cast<double>(RAND_MAX);
//		tess_vertices_materials[i][2] = 0.66666;//rand() / static_cast<double>(RAND_MAX);
//		tess_vertices_materials[i][3] = 1;
//	}
//
//	for (size_t i = 0; i < tess.dual_vertices.size(); i++)
//	{
//		tess_dual_vertices_materials[i][0] = 0.66666;// rand() / static_cast<double>(RAND_MAX);// 0.6666;
//		tess_dual_vertices_materials[i][1] = 0.66666;//rand() / static_cast<double>(RAND_MAX);
//		tess_dual_vertices_materials[i][2] = 0.66666;//rand() / static_cast<double>(RAND_MAX);
//		tess_dual_vertices_materials[i][3] = 1;
//	}
//
//	if (true == curved_triangles)
//	{
//		for (size_t i = 0; i < ctris.size(); i++)
//			ctris[i].init_mats(tess_vertices_materials[ctris[i].seed_i0], tess_vertices_materials[ctris[i].seed_i1], tess_vertices_materials[ctris[i].seed_i2]);
//
//		for (size_t i = 0; i < vctris.size(); i++)
//			vctris[i].init_mats(tess_dual_vertices_materials[vctris[i].seed_i0], tess_dual_vertices_materials[vctris[i].seed_i1], tess_dual_vertices_materials[vctris[i].seed_i2]);
//	}
//}
//
//
//void generate_materials(void)
//{
//	vector<float> mat(4, 0.666f);
//	tess_vertices_materials.resize(tess.vertices.size(), mat);
//	tess_dual_vertices_materials.resize(tess.dual_vertices.size(), mat);
//
//	generate_temperature_materials();
//}




void latlon_to_xyz(const double& lat, const double& lon, double& x, double& y, double& z)
{
	double theta = 2 * pi * (lon + 180.0) / 360.0;
	double phi = pi * (lat + 90.0) / 180.0;

	x = -(cos(theta) * sin(phi));
	z = -(sin(theta) * sin(phi));
	y = -cos(phi);
}


void xyz_to_latlon(const double& x, const double& y, const double& z, double& lat, double& lon)
{
	double theta = pi + atan2(z, x);
	double phi = acos(-y);

	lat = phi / pi * 180.0 - 90.0;
	lon = theta / (2 * pi) * 360.0 - 180.0;
}

#include <random>
std::mt19937 generator(0);
std::uniform_real_distribution<double> dis(0.0, 1.0);


vector_3 RandomUnitVector(void)
{
	double z = dis(generator) * 2.0 - 1.0;
	double a = dis(generator) * 2.0 * pi;

	//MyBig z = static_cast<MyBig>(rand() % RAND_MAX) / static_cast<MyBig>(RAND_MAX) * 2 - 1;
	//MyBig a = static_cast<MyBig>(rand() % RAND_MAX) / static_cast<MyBig>(RAND_MAX) * 2 * pi;
	double r = sqrt(1.0f - z * z);
	double x = r * cos(a);
	double y = r * sin(a);
	return vector_3(x, y, z).normalize();
}




void idle_func(void)
{
	glutPostRedisplay();
}

void init_opengl(const int& width, const int& height)
{
	win_x = width;
	win_y = height;

	if (win_x < 1)
		win_x = 1;

	if (win_y < 1)
		win_y = 1;

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(win_x, win_y);
	win_id = glutCreateWindow("Delaunay/Voronoi on 2-sphere");

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

	float light0_colour[] = { 0.8f, 0.8f, 0.8f, 0.8f };
	float light1_colour[] = { 0.8f, 0.8f, 0.8f, 0.8f };
	float light2_colour[] = { 0.8f, 0.8f, 0.8f, 0.8f };
	float light3_colour[] = { 0.8f, 0.8f, 0.8f, 0.8f };
	float light4_colour[] = { 0.8f, 0.8f, 0.8f, 0.8f };
	float light5_colour[] = { 0.8f, 0.8f, 0.8f, 0.8f };


	float light0_position[] = { 1.0f, 0.0f, 0.0f, 0.0f };
	float light1_position[] = { -1.0f, 0.0f, 0.0f, 0.0f };
	float light2_position[] = { 0.0f, 1.0f, 0.0f, 0.0f };
	float light3_position[] = { 0.0f, -1.0f, 0.0f, 0.0f };
	float light4_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	float light5_position[] = { 0.0f, 0.0f, -1.0f, 0.0f };

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


	for (size_t i = 0; i < tess.dual_centres.size(); i++)
	{
		float dotsq = cam.x * tess.dual_centres[i].x + cam.y * tess.dual_centres[i].y + cam.z * tess.dual_centres[i].z;

		if (dotsq > highest_dotsq)
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

	if (win_x < 1)
		win_x = 1;

	if (win_y < 1)
		win_y = 1;

	glutSetWindow(win_id);
	glutReshapeWindow(win_x, win_y);
	glViewport(0, 0, win_x, win_y);

	main_camera.Set(main_camera.u, main_camera.v, main_camera.w, main_camera.fov, win_x, win_y, camera_near, camera_far);
}

// Text drawing code originally from "GLUT Tutorial -- Bitmap Fonts and Orthogonal Projections" by A R Fernandes
void render_string(int x, const int y, void* font, const string& text)
{
	for (size_t i = 0; i < text.length(); i++)
	{
		glRasterPos2i(x, y);
		glutBitmapCharacter(font, text[i]);
		x += glutBitmapWidth(font, text[i]) + 1;
	}
}
// End text drawing code.

void display_func(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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


	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	//glMatrixMode(GL_MODELVIEW);
	// End text drawing code.


	glFlush();

	if(false == doing_screenshot)
	glutSwapBuffers();
}

void keyboard_func(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{

	case 'f':
	{
		//curved_triangles = !curved_triangles;
		//	generate_materials();
		break;
	}
	case 'g':
	{
		//delaunay_mode = !delaunay_mode;
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

	case 'm':
	{
		take_screenshot(4, "out.tga");// , const bool reverse_rows = false)

		break;
	}

	default:
		break;
	}
}

void mouse_func(int button, int state, int x, int y)
{
	if (GLUT_LEFT_BUTTON == button)
	{
		if (GLUT_DOWN == state)
			lmb_down = true;
		else
			lmb_down = false;
	}
	else if (GLUT_MIDDLE_BUTTON == button)
	{
		if (GLUT_DOWN == state)
			mmb_down = true;
		else
			mmb_down = false;
	}
	else if (GLUT_RIGHT_BUTTON == button)
	{
		if (GLUT_DOWN == state)
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

	if (true == lmb_down && (0 != mouse_delta_x || 0 != mouse_delta_y))
	{
		if (main_camera.w < 1.5f)
		{
			main_camera.u -= static_cast<float>(mouse_delta_y) * u_spacer / 10.0f;
			main_camera.v += static_cast<float>(mouse_delta_x) * v_spacer / 10.0f;
		}
		else
		{
			main_camera.u -= static_cast<float>(mouse_delta_y) * u_spacer;
			main_camera.v += static_cast<float>(mouse_delta_x) * v_spacer;
		}

		float highest_dotsq = -1;

		vector_3 cam;
		cam = main_camera.eye;
		cam.normalize();


		for (size_t i = 0; i < tess.dual_centres.size(); i++)
		{
			float dotsq = cam.x * tess.dual_centres[i].x + cam.y * tess.dual_centres[i].y + cam.z * tess.dual_centres[i].z;

			if (dotsq > highest_dotsq)
			{
				highest_dotsq = dotsq;
				selected_vertex = i;
			}
		}
	}
	else if (true == rmb_down && (0 != mouse_delta_y))
	{
		main_camera.w -= static_cast<float>(mouse_delta_y) * w_spacer;

		if (main_camera.w < 0.1f)
			main_camera.w = 0.1f;
		else if (main_camera.w > 8.0f)
			main_camera.w = 8.0f;
	}

	main_camera.Set(); // Calculate new camera vectors.
}

void passive_motion_func(int x, int y)
{
	mouse_x = x;
	mouse_y = y;
}




vector_3 slerp(vector_3 s0, vector_3 s1, const double t)
{
	vector_3 s0_norm = s0;
	s0_norm.normalize();

	vector_3 s1_norm = s1;
	s1_norm.normalize();

	const double cos_angle = s0_norm.dot(s1_norm);
	const double angle = acos(cos_angle);

	const double p0_factor = sin((1 - t) * angle) / sin(angle);
	const double p1_factor = sin(t * angle) / sin(angle);

	return s0 * p0_factor + s1 * p1_factor;
}



void draw_objects(void)
{
	glDisable(GL_LIGHTING);

	static const float rad_to_deg = 180.0f / pi;

	glPushMatrix();

	glTranslatef(camera_x_transform, camera_y_transform, 0);


	if (doing_screenshot)
		glLineWidth(6.0);
	else
		glLineWidth(1.0);


	//if (delaunay_mode == true && curved_triangles == true)
	//{
	//	glColor3f(0.5, 0.5, 0.5);



	//	glBegin(GL_TRIANGLES);

	//	for (size_t i = 0; i < ctris.size(); i++)
	//		ctris[i].draw_colour3();

	//	glEnd();


	//	for (size_t j = 0; j < ctris.size(); j += 1)
	//	{
	//		glColor3f(0, 0, 0);

	//		glBegin(GL_LINE_STRIP);
	//		{
	//			const vector_3 vj_start = tess.vertices[ctris[j].seed_i0];
	//			const vector_3 vj_end = tess.vertices[ctris[j].seed_i1];

	//			const size_t step_count = 100;

	//			const double step_size = 1.0 / step_count;

	//			double t = 0;

	//			for (size_t k = 0; k < step_count; k++, t += step_size)
	//			{
	//				vector_3 vj = slerp(vj_start, vj_end, t);
	//				glVertex3d(vj.x, vj.y, vj.z);
	//			}
	//		}

	//		glEnd();
	//		glBegin(GL_LINE_STRIP);
	//		{
	//			const vector_3 vj_start = tess.vertices[ctris[j].seed_i1];
	//			const vector_3 vj_end = tess.vertices[ctris[j].seed_i2];

	//			const size_t step_count = 100;

	//			const double step_size = 1.0 / step_count;

	//			double t = 0;

	//			for (size_t k = 0; k < step_count; k++, t += step_size)
	//			{
	//				vector_3 vj = slerp(vj_start, vj_end, t);
	//				glVertex3d(vj.x, vj.y, vj.z);
	//			}
	//		}

	//		glEnd();
	//		glBegin(GL_LINE_STRIP);
	//		{
	//			const vector_3 vj_start = tess.vertices[ctris[j].seed_i2];
	//			const vector_3 vj_end = tess.vertices[ctris[j].seed_i0];

	//			const size_t step_count = 100;

	//			const double step_size = 1.0 / step_count;

	//			double t = 0;

	//			for (size_t k = 0; k < step_count; k++, t += step_size)
	//			{
	//				vector_3 vj = slerp(vj_start, vj_end, t);
	//				glVertex3d(vj.x, vj.y, vj.z);
	//			}
	//		}

	//		glEnd();
	//	}
	//}
	//else if (delaunay_mode == true && curved_triangles == false)
	//{
	//	if (false == disable_lighting)
	//		glEnable(GL_LIGHTING);
	//	else
	//		glDisable(GL_LIGHTING);

	//	glBegin(GL_TRIANGLES);

	//	for (size_t i = 0; i < tess.dtris.size(); i++)
	//	{
	//		glColor3f(0.5, 0.5, 0.5);

	//		glNormal3d(tess.vertices[tess.dtris[i].i0].x, tess.vertices[tess.dtris[i].i0].y, tess.vertices[tess.dtris[i].i0].z);
	//		glVertex3d(tess.vertices[tess.dtris[i].i0].x, tess.vertices[tess.dtris[i].i0].y, tess.vertices[tess.dtris[i].i0].z);

	//		glNormal3d(tess.vertices[tess.dtris[i].i1].x, tess.vertices[tess.dtris[i].i1].y, tess.vertices[tess.dtris[i].i1].z);
	//		glVertex3d(tess.vertices[tess.dtris[i].i1].x, tess.vertices[tess.dtris[i].i1].y, tess.vertices[tess.dtris[i].i1].z);

	//		glNormal3d(tess.vertices[tess.dtris[i].i2].x, tess.vertices[tess.dtris[i].i2].y, tess.vertices[tess.dtris[i].i2].z);
	//		glVertex3d(tess.vertices[tess.dtris[i].i2].x, tess.vertices[tess.dtris[i].i2].y, tess.vertices[tess.dtris[i].i2].z);
	//	}

	//	glEnd();

	//	if (true == draw_tri_outlines)
	//	{
	//		glDisable(GL_LIGHTING);

	//		glLineWidth(outline_width);

	//		glColor4f(outline_colour[0], outline_colour[1], outline_colour[2], 0.2f);

	//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//		glBegin(GL_TRIANGLES);

	//		for (size_t i = 0; i < tess.dtris.size(); i++)
	//		{
	//			glVertex3d(tess.vertices[tess.dtris[i].i0].x, tess.vertices[tess.dtris[i].i0].y, tess.vertices[tess.dtris[i].i0].z);
	//			glVertex3d(tess.vertices[tess.dtris[i].i1].x, tess.vertices[tess.dtris[i].i1].y, tess.vertices[tess.dtris[i].i1].z);
	//			glVertex3d(tess.vertices[tess.dtris[i].i2].x, tess.vertices[tess.dtris[i].i2].y, tess.vertices[tess.dtris[i].i2].z);
	//		}
	//		glEnd();

	//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//	}



	//}
	//else if (delaunay_mode == false && curved_triangles == false)
	//{
	//	if (false == disable_lighting)
	//		glEnable(GL_LIGHTING);
	//	else
	//		glDisable(GL_LIGHTING);

	//	glBegin(GL_TRIANGLES);

	//	for (size_t i = 0; i < tess.vtris.size(); i++)
	//	{
	//		glColor3f(0.5, 0.5, 0.5);

	//		glNormal3d(tess.dual_vertices[tess.vtris[i].i0].x, tess.dual_vertices[tess.vtris[i].i0].y, tess.dual_vertices[tess.vtris[i].i0].z);
	//		glVertex3d(tess.dual_vertices[tess.vtris[i].i0].x, tess.dual_vertices[tess.vtris[i].i0].y, tess.dual_vertices[tess.vtris[i].i0].z);

	//		glNormal3d(tess.dual_vertices[tess.vtris[i].i1].x, tess.dual_vertices[tess.vtris[i].i1].y, tess.dual_vertices[tess.vtris[i].i1].z);
	//		glVertex3d(tess.dual_vertices[tess.vtris[i].i1].x, tess.dual_vertices[tess.vtris[i].i1].y, tess.dual_vertices[tess.vtris[i].i1].z);

	//		glNormal3d(tess.dual_vertices[tess.vtris[i].i2].x, tess.dual_vertices[tess.vtris[i].i2].y, tess.dual_vertices[tess.vtris[i].i2].z);
	//		glVertex3d(tess.dual_vertices[tess.vtris[i].i2].x, tess.dual_vertices[tess.vtris[i].i2].y, tess.dual_vertices[tess.vtris[i].i2].z);
	//	}

	//	glEnd();

	//	{
	//		glDisable(GL_LIGHTING);

	//		glLineWidth(outline_width);

	//		glColor4f(outline_colour[0], outline_colour[1], outline_colour[2], 0.2f);

	//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//		glBegin(GL_TRIANGLES);

	//		for (size_t i = 0; i < tess.vtris.size(); i++)
	//		{
	//			glVertex3d(tess.dual_vertices[tess.vtris[i].i0].x, tess.dual_vertices[tess.vtris[i].i0].y, tess.dual_vertices[tess.vtris[i].i0].z);
	//			glVertex3d(tess.dual_vertices[tess.vtris[i].i1].x, tess.dual_vertices[tess.vtris[i].i1].y, tess.dual_vertices[tess.vtris[i].i1].z);
	//			glVertex3d(tess.dual_vertices[tess.vtris[i].i2].x, tess.dual_vertices[tess.vtris[i].i2].y, tess.dual_vertices[tess.vtris[i].i2].z);
	//		}
	//		glEnd();

	//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//	}
	//}
	//else




if (delaunay_mode == false && curved_triangles == true)
{
	glColor3f(0.5, 0.5, 0.5);

	glBegin(GL_TRIANGLES);

	for (size_t i = 0; i < vctris.size(); i++)
	{
		//if (0 < main_camera.look_at.dot(vctris[i].circumcentre_normal))
		//	continue;

		if (tess.vtri_vngon_index[i] == selected_vertex)
			glColor3f(0, 1, 0.0);
		else
		{
			const vector<size_t> x = tess.vngon_adjacencies[selected_vertex];

			if (x.end() != find(x.begin(), x.end(), tess.vtri_vngon_index[i]))
				glColor3f(1.0, 1.0, 0);
			else
				glColor3f(0.5, 0.5, 0.5);
		}

		vctris[i].draw_colour3();
	}

	glEnd();





	if (true == draw_tri_outlines)
	{
		for (size_t i = 0; i < tess.vngons.size(); i++)
		{
			glColor3f(0, 0.0, 0);

			glBegin(GL_LINE_STRIP);

			for (size_t j = 0; j < tess.vngons[i].v.size() - 1; j += 1)
			{
				const vector_3 vj_start = tess.dual_vertices[tess.vngons[i].v[j]];
				const vector_3 vj_end = tess.dual_vertices[tess.vngons[i].v[j + 1]];

				const size_t step_count = 100;

				const double step_size = 1.0 / step_count;

				double t = 0;

				for (size_t k = 0; k < step_count; k++, t += step_size)
				{
					vector_3 vj = slerp(vj_start, vj_end, t);
					glVertex3d(vj.x, vj.y, vj.z);
				}
			}

			glEnd();
		}
	}
}












	//glColor3f(0, 0, 0);

	//glPushMatrix();
	//glTranslatef(tess.dual_centres[selected_vertex].x*1.1, tess.dual_centres[selected_vertex].y * 1.1, tess.dual_centres[selected_vertex].z * 1.1);
	//glutSolidSphere(0.1, vertex_slices * 10, vertex_stacks * 10);
	//glPopMatrix();







	// If we do draw the axis at all, make sure not to draw its outline.
	if (draw_axis)
	{
		glDisable(GL_LIGHTING);

		glEnable(GL_ALPHA);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glLineWidth(outline_width * 1.5f);

		glBegin(GL_LINES);

		//		glColor4f(0, 0, 0, 0.5);

		const float opacity = 0.5;
		const float length = 1.2;

		glColor4f(1, 0, 0, opacity);
		glVertex3d(0, 0, 0);
		glVertex3d(length, 0, 0);

		glColor4f(0, 1, 0, opacity);
		glVertex3d(0, 0, 0);
		glVertex3d(0, length, 0);

		glColor4f(0, 0, 1, opacity);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, length);

		glColor4f(0, 1, 1, opacity);
		glVertex3d(0, 0, 0);
		glVertex3d(-length, 0, 0);

		glColor4f(1, 0, 1, opacity);
		glVertex3d(0, 0, 0);
		glVertex3d(0, -length, 0);

		glColor4f(1, 1, 0, opacity);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, -length);



		//glColor4f(0, 0, 0, 0.25);
		//glVertex3d(0, 0, 0);
		//glVertex3d(-1, 0, 0);
		//glVertex3d(0, 0, 0);
		//glVertex3d(0, -1, 0);
		//glVertex3d(0, 0, 0);
		//glVertex3d(0, 0, -1);

		glEnd();

		glDisable(GL_BLEND);
		glDisable(GL_ALPHA);
	}

	glPopMatrix();
}



#endif