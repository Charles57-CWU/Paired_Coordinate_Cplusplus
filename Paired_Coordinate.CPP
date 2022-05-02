#include <windows.h>
// openGl
#include <gl/Gl.h>
#include <gl/Glu.h>
#include "glut.h"
//io
#include <iostream> 
#include <iomanip>
//reading file
#include <fstream>
#include <sstream>
#include <string>
//sorting matrix
#include <vector>
#include <algorithm>

//std::string filename = "wb_cancer_sorted.txt";
std::string filename = "iris_data.csv";

using namespace std;
// dataframe
using vecf = vector<float>;
using matrix = vector<vecf>;
// samples per class array
using veci = vector<int>;
veci samples_per_class;


int number_attributes = 0;
int number_classes = 0;
int number_samples = 0;

float class_colors[7][3] = { {1.0, 0.0, 0.0},
			     {0.0, 1.0, 0.0},
			     {0.0, 0.0, 1.0},
			     {1.0, 1.0, 0.0},
			     {1.0, 0.0, 1.0},
			     {0.0, 1.0, 1.0},
			     {0.5, 0.5, 0.5} };

//==============================assistant functions==============================
//read data from file
matrix readCSV()
{
	char separator = ',';
	matrix result;
	string row, ele;

	// check file can open
	ifstream f;
	f.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		f.open(filename);
	}
	catch (std::system_error& e) {
		cout << "No file or directory.\n";
		exit(0);
	}
	f.close();

	ifstream in(filename);
	// get dataset info
	getline(in, row);
	number_attributes = stoi(row);
	if (number_attributes == 2)
	{
		cout << "Warnining!\n";
		cout << "Dataset must have even number of attributes.\n";
		cout << "Please add or remove one and try again.\n";
		exit(0);
	}

	getline(in, row);
	number_classes = stoi(row);
	if (number_classes > 7)
	{
		cout << "Warnining!\n";
		cout << "Dataset must have 7 or fewer classes.\n";
		cout << "Please remove classes (or hardcode in more colors).\n";
		exit(0);
	}

	getline(in, row);
	number_samples = stoi(row);
	getline(in, row);

	stringstream tt(row);
	while (getline(tt, ele, separator))
	{
		samples_per_class.push_back(stoi(ele));
	}

	// put sample info into matrix
	// read line loop
	vecf R;
	while (getline(in, row))
	{
		stringstream ss(row);
		// read elements in row loop
		while (getline(ss, ele, separator))
		{
			R.push_back(stof(ele));
		}
		result.push_back(R);
		R.clear();

	}
	in.close();

	return result;
}

// print matrix (optional)
void printMatrix(const matrix& M)
{
	cout << "Dataset title: " << filename << "\n";
	cout << "Number of attributes: " << number_attributes << "\n";
	cout << "Number of classes: " << number_classes << "\n";
	cout << "Number of samples: " << number_samples << "\n";

	for (vecf row : M)
	{
		for (float ele : row)
		{
			cout << setw(12) << left << ele << " ";
		}
		cout << "\n";
	}
}
//==============================data processing==============================
// find max column value (for min-max scaling)
float findColMax(const matrix& M, int col)
{
	float max = M[0][col];
	for (int i = 1; i < number_samples; i++) 
	{
		if (M[i][col] > max)
		{
			max = M[i][col];
		}
	}
	//cout << max << endl;
	return max;
}

// find min column value (for min-max scaling)
float findColMin(const matrix& M, int col) 
{
	float min = M[0][col];
	for (int i = 1; i < number_samples; i++)
	{
		if (M[i][col] < min)
		{
			min = M[i][col];
		}
	}
	//cout << min << endl;
	return min;
}

// min max scaling
void minmaxscaler(matrix& M, int col, float rangeMin, float rangeMax) 
{
	float max = findColMax(M, col);
	float min = findColMin(M, col);
	for (int j = 0; j < number_samples; j++)
	{
		M[j][col ] = rangeMin + ((M[j][col] - min) * (rangeMax - rangeMin)) / (max - min);
	}

}
//scale matrix to screen
void scaleMatrix(matrix& M)
{
	// spacers for horizontal axis
	float spacer = 1.0 / (number_attributes / 2.0);
	float space_beg = 0.0;
	float space_end = spacer;

	// scale attributes to plot axes
	for (int i = 0; i < number_attributes; i++)
	{
		// add vertical attributes to axis
		if (i % 2 != 0)
		{
			minmaxscaler(M, i, 0, 1);
		}
		// scale horizontal attributes to axis
		else
		{
			minmaxscaler(M, i, space_beg, space_end);
			space_beg = space_end;
			space_end += spacer;
		}
	}
}
//==============================vertex functions==============================
// position vertices
vecf getPositionVertices(const matrix& M) 
{
	vecf T;
	for (int i = 0; i < number_samples; i++)
	{
		for (int j = 0; j < number_attributes; j++)
		{
			T.push_back(M[i][j]);
		}
	}
	return T;
}
// color vertices
vecf getColorVertices() 
{
	vecf T;

	float R = 0.0;
	float G = 0.0;
	float B = 0.0;

	for (int i = 0; i < number_classes; i++)
	{
		
		for (int j = 0; j < samples_per_class[i] * int(number_attributes / 2); j++)
		{
			T.push_back(class_colors[i][0]);
			T.push_back(class_colors[i][1]);
			T.push_back(class_colors[i][2]);
		}
	}
	return T;
}

//==============================main functions==============================
void myDisplay()
{
	matrix dataset = readCSV();
	scaleMatrix(dataset);
	//printMatrix(dataset);
	
	vecf positions = getPositionVertices(dataset);
	vecf colors = getColorVertices();
	
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// draw y-axis (only needs one call) 
	glColor3ub(0, 0, 0);
	
	glBegin(GL_LINES);
	glVertex2f(0.0, 0.0);
	glVertex2f(1.0, 0.0);
	glEnd();
	
	//draw x-axes
	float spacer = 1.0 / (number_attributes / 2.0);
	float next_axis = 0.0;
	for (int i = 0; i < number_attributes / 2; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(next_axis, 0.0);
		glVertex2f(next_axis, 1);
		glEnd();
		next_axis += spacer;
	}

	// draw polylines of dataset
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, &positions[0]);
	glColorPointer(3, GL_FLOAT, 0, &colors[0]);

	for (int i = 0; i < number_samples * (number_attributes / 2); i = i + (number_attributes / 2)) {
		glDrawArrays(GL_LINE_STRIP, i, (number_attributes / 2));
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glFlush();
}

void myInit()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.125, 1.125, -0.125, 1.125, 0, 1);
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Paired Coordinates");
	// information
	cout << "Paired Coordinate Plot\n";
	cout << "Plot requires an even number of attributes!\n";
	cout << "All text to include class names must be converted to integers or floats\n";
	cout << "Data must be sorted by class\n";
	cout << "You may need to change the delimiter in code (default delimiter is comma)\n";
	cout << "First four rows: attribute count -> class count -> sample count -> samples per class\n";
	cout << "Please format data as the following example:\n";
	cout << "4\n";
	cout << "3\n";
	cout << "150\n";
	cout << "50,50,50\n";
	cout << "1.0,2.3,1.6,2.6,0\n";
	cout << "1.6,6.3,2.3,5.3,0\n";
	cout << "3.5,7.4,1.4,4.3,1\n";
	cout << "=========================\n";
	
	glutDisplayFunc(myDisplay);
	myInit();
	glutMainLoop();
}
