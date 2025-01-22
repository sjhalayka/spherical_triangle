#include "station_data.h"


year_data::year_data(void)
{
//	year = 0;
	temperatures[0] = temperatures[1] = temperatures[2] = temperatures[3] = 
		temperatures[4] = temperatures[5] = temperatures[6] = temperatures[7] = 
		temperatures[8] = temperatures[9] = temperatures[10] = temperatures[11] = 0;
}

bool year_data::operator==(const year_data &rhs) const
{
	if(//year == rhs.year && 
		temperatures[0] == rhs.temperatures[0] && 
		temperatures[1] == rhs.temperatures[1] && 
		temperatures[2] == rhs.temperatures[2] && 
		temperatures[3] == rhs.temperatures[3] && 
		temperatures[4] == rhs.temperatures[4] && 
		temperatures[5] == rhs.temperatures[5] && 
		temperatures[6] == rhs.temperatures[6] && 
		temperatures[7] == rhs.temperatures[7] && 
		temperatures[8] == rhs.temperatures[8] && 
		temperatures[9] == rhs.temperatures[9] && 
		temperatures[10] == rhs.temperatures[10] && 
		temperatures[11] == rhs.temperatures[11])
	{
		return true;
	}

	return false;
}

ostream& operator<<(ostream &out, const year_data &y)
{
	out //<< y.year << ' ' 
		<< y.temperatures[0] << ' ' << y.temperatures[1] << ' ' << y.temperatures[2] << ' ' 
		<< y.temperatures[3] << ' ' << y.temperatures[4] << ' ' << y.temperatures[5] << ' ' 
		<< y.temperatures[6] << ' ' << y.temperatures[7] << ' ' << y.temperatures[8] << ' ' 
		<< y.temperatures[9] << ' ' << y.temperatures[10] << ' ' << y.temperatures[11];

	return out;
}

station_data::station_data(void)
{
	station_id = 0;
	name = country = "";
	latitude = longitude = 0;
}

bool station_data::operator==(const station_data &rhs) const
{
	if(station_id == rhs.station_id && 
		years == rhs.years &&
		name == rhs.name &&
		country == rhs.country &&
		latitude == rhs.latitude &&
		longitude == rhs.longitude)
		return true;

	return false;
}

ostream& operator<<(ostream &out, const station_data &s)
{
	out << s.station_id << ' ' << s.name << ", " << s.country << ' ' << s.latitude << ' ' << s.longitude << endl;

	for(map<short unsigned int, year_data>::const_iterator ci = s.years.begin(); ci != s.years.end(); ci++)
		out << ci->first << ' ' << ci->second << endl;

	return out;
}


float regline_slope(vector<float> &x, vector<float> &y)
{
	if(x.size() != y.size())
		return 0;

	float x_mean = 0, y_mean = 0;

	for(size_t i = 0; i < x.size(); i++)
	{
		x_mean += x[i];
		y_mean += y[i];
	}

	x_mean /= static_cast<float>(x.size());
	y_mean /= static_cast<float>(y.size());

	float covariance = 0, variance = 0;

	for(size_t i = 0; i < x.size(); i++)
	{
		float z = x[i] - x_mean;
		covariance += z*(y[i] - y_mean);
		variance += z*z;
	}

	return covariance / variance; // beta
}

float standard_deviation(const vector<float> &src)
{
	float mean = 0;
	float size = static_cast<float>(src.size());

	for(size_t i = 0; i < src.size(); i++)
		mean += src[i];

	mean /= size;

	float sq_diff = 0;

	for(size_t i = 0; i < src.size(); i++)
	{
		float diff = src[i] - mean;
		sq_diff += diff*diff;
	}

	sq_diff /= size;

	return sqrt(sq_diff);
}

bool get_data(vector<station_data> &sd)
{
	sd.clear();

	ifstream infile("HadCRUT3.bin", ios_base::binary);

	if(!infile.is_open())
	{
		cout << "Error opening file: HadCRUT3.bin" << endl;
		return false;
	}

	cout << "Reading data from binary file." << endl;

	long unsigned int num_stations = 0;
	infile.read(reinterpret_cast<char *>(&num_stations), sizeof(long unsigned int));
	sd.resize(num_stations);

	for(size_t i = 0; i < sd.size(); i++)
	{
		infile.read(reinterpret_cast<char *>(&sd[i].station_id), sizeof(long unsigned int));

		short unsigned int length = 0;

		infile.read(reinterpret_cast<char *>(&length), sizeof(short unsigned int));
		sd[i].name.resize(length);
		infile.read(reinterpret_cast<char *>(&sd[i].name[0]), sizeof(char)*length);

		infile.read(reinterpret_cast<char *>(&length), sizeof(short unsigned int));
		sd[i].country.resize(length);
		infile.read(reinterpret_cast<char *>(&sd[i].country[0]), sizeof(char)*length);

		infile.read(reinterpret_cast<char *>(&sd[i].latitude), sizeof(float));
		infile.read(reinterpret_cast<char *>(&sd[i].longitude), sizeof(float));

		long unsigned int num_years = 0;
		infile.read(reinterpret_cast<char *>(&num_years), sizeof(long unsigned int));

		for(size_t j = 0; j < num_years; j++)
		{
			short unsigned int year;
			year_data y;

			infile.read(reinterpret_cast<char *>(&year), sizeof(short unsigned int));
			infile.read(reinterpret_cast<char *>(&y.temperatures[0]), sizeof(float)*12);

			sd[i].years[year] = y;
		}
	}

	cout << "Read " << sd.size() << " stations." << endl;

	return true;
}


void get_local_trends(const vector<station_data> &sd, const size_t &station_id, const short unsigned int &first_year, const short unsigned int &last_year, vector<float> &output_trends, const size_t min_samples_per_slope)
{
	output_trends.clear();

	// x is year, y is temperature
	// one vector per month
	vector<float> x[12], y[12];

	for(map<short unsigned int, year_data>::const_iterator ci = sd[station_id].years.begin(); ci != sd[station_id].years.end(); ci++)
	{
		if(ci->first < first_year || ci->first > last_year)
			continue;

		for(size_t k = 0; k < 12; k++)
		{
			if((ci->second).temperatures[k] != -99.0f)
			{
				x[k].push_back(static_cast<float>(ci->first));
				y[k].push_back((ci->second).temperatures[k]);
			}
		}
	}

	for(size_t j = 0; j < 12; j++)
		if(min_samples_per_slope <= x[j].size())
			output_trends.push_back(regline_slope(x[j], y[j]));
}







void write_trend_histogram(const vector<station_data> &sd, long unsigned int num_histogram_bins, const size_t min_samples_per_slope)
{
	vector<float> slopes;

	for(size_t i = 0; i < sd.size(); i++)
	{
		// x is year, y is temperature
		// one vector per month
		vector<float> x[12], y[12];

		for(map<short unsigned int, year_data>::const_iterator ci = sd[i].years.begin(); ci != sd[i].years.end(); ci++)
		{
			for(size_t k = 0; k < 12; k++)
			{
				if((ci->second).temperatures[k] != -99.0f)
				{
					x[k].push_back(static_cast<float>(ci->first));
					y[k].push_back((ci->second).temperatures[k]);
				}
			}
		}

		for(size_t j = 0; j < 12; j++)
			if(min_samples_per_slope <= x[j].size())
				slopes.push_back(regline_slope(x[j], y[j]));
	}

	sort(slopes.begin(), slopes.end());

	float slope_min = FLT_MAX;
	float slope_max = FLT_MIN;
	double slope_mean = 0;

	for(size_t i = 0; i < slopes.size(); i++)
	{
		if(slopes[i] < slope_min)
			slope_min = slopes[i];

		if(slopes[i] > slope_max)
			slope_max = slopes[i];

		slope_mean += slopes[i];
	}

	slope_mean /= static_cast<double>(slopes.size());

	cout << "Slope min:  " << slope_min << endl;
	cout << "Slope max:  " << slope_max << endl;
	cout << "Slope mean: " << slope_mean << endl;




	float distance = fabsf(slope_max - slope_min); // can skip fabsf, I suppose
	float bin_width = distance / static_cast<float>(num_histogram_bins);
	float half_bin_width = bin_width * 0.5f;

	vector<long unsigned int> bins(num_histogram_bins);
	size_t slope_index = 0;

	for(size_t i = 0; i < num_histogram_bins; i++)
	{
		float curr_end = slope_min + (i+1)*bin_width;

		while(slope_index < slopes.size() && slopes[slope_index] <= curr_end)
		{
			bins[i]++;
			slope_index++;
		}
	}


	float curr_mid = slope_min + half_bin_width;

	cout << "Outputting gnuplot histogram data." << endl;
	ofstream plotdata("histogram.txt");

	for(size_t i = 0; i < num_histogram_bins; i++)
	{
		plotdata << curr_mid << ' ' << bins[i] << endl;
		curr_mid += bin_width;
	}

	ofstream plotcmd("histogram.plt");

	plotcmd << "set terminal postscript eps monochrome enhanced" << endl;
	plotcmd << "set key off" << endl;
	plotcmd << "set output \"histogram.eps\"" << endl;
	plotcmd << "set grid" << endl;
	plotcmd << "set xlabel \"Slope\"" << endl;
	plotcmd << "set ylabel \"Count\"" << endl;
	plotcmd << "set title \"Num slopes = " << slopes.size() << ", slope min = " << slope_min << ", slope max = " << slope_max << ", slope mean = " << slope_mean << ", num bins = " << num_histogram_bins << "\"" << endl;
//	plotcmd << "set xrange [" << slope_min << ':' << slope_max << ']' << endl;
	plotcmd << "set xrange [" << -0.2 << ':' << 0.2 << ']' << endl;
	plotcmd << "plot \"histogram.txt\" with boxes" << endl;
	plotcmd << "set terminal wxt" << endl;
	plotcmd << "set output" << endl;
}

