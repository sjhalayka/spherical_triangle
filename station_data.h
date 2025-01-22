#ifndef STATION_DATA_H
#define STATION_DATA_H

#include <vector>
#include <deque>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ios>
#include <cfloat>
#include <algorithm>
#include <cmath>
using std::vector;
using std::deque;
using std::wstring;
using std::string;
using std::getline;
using std::wcout;
using std::cout;
using std::ostream;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::ios_base;
using std::sort;
using std::random_shuffle;

#include <map>
using std::map;

#include "custom_math.h"
using custom_math::pi;

class year_data
{
public:
//	short unsigned int year;
	float temperatures[12];

	year_data(void);
	bool operator==(const year_data &rhs) const;
	friend ostream& operator<<(ostream &out, const year_data &y);
};

class station_data
{
public:
	long unsigned int station_id;
	string name;
	string country;
	float latitude;
	float longitude;
	map<short unsigned int, year_data> years;

	station_data(void);
	bool operator==(const station_data &rhs) const;
	friend ostream& operator<<(ostream &out, const station_data &s);
};

float regline_slope(vector<float> &x, vector<float> &y);
bool get_data(vector<station_data> &sd);
void write_trend_histogram(const vector<station_data> &sd, long unsigned int num_histogram_bins, const size_t min_samples_per_slope = 20);
void get_local_trends(const vector<station_data> &sd, const size_t &station_id, const short unsigned int &first_year, const short unsigned int &last_year, vector<float> &output_trends, const size_t min_samples_per_slope = 20);
float standard_deviation(const vector<float> &src);

#endif
