#pragma once
#include <vector>

using std::vector;

class Image;

class HeightMap {
	vector<float> vertices;
	vector<unsigned int> elements;
public:
	HeightMap(char* file);
	HeightMap(Image& image, const int& height, const int& width, const int& length);
	HeightMap(char* file, const int& height, const int& width, const int& length);
	void PrintVertices();
	void PrintElements();
	const vector<float>& Vertices();
	const vector<unsigned int>& Elements();
};