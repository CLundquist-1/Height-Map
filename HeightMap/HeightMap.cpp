#include "HeightMap.h"
#include "Image.h"
#include <glm/glm.hpp>

using glm::vec3;

HeightMap::HeightMap(Image& image, const int& height, const int& width, const int& length) {
	if (image.Height() < 2 || image.Width() < 2) {
		cout << "Cannot create a 3D map from your 1D Drawing. Draw a Proper Map, Not A Trail. I'm Not That Kind of Terrain Generator.";
	}
	float* pixels = image.Pixels();
	float x = 0.0f, z = 0.0f;
	for (unsigned int i = 0; i < image.Height(); i++) {
		x = 0;
		for (unsigned int j = 0; j < image.Width(); j++) {
			vertices.push_back(x);
			vertices.push_back((1 - (pixels[0] + pixels[1] + pixels[2]) / 3)*height);
			vertices.push_back(z);
			//cout << (int)pixels << std::endl;
			pixels += image.Channels();//sizeof(float) * image.Channels();
			x += width;
		}
		z += length;
	}
	unsigned int r = 0;
	unsigned int w = image.Width();
	for (unsigned int i = 0; i < image.Height() - 1; i++) {
		for (unsigned int j = 0; j < image.Width() - 1; j++) {
			elements.push_back(r + j);
			elements.push_back(r + j + 1);
			elements.push_back(r + j + w);
			elements.push_back(r + j + 1);
			elements.push_back(r + j + w);
			elements.push_back(r + j + w + 1);
		}
		r += w;
	}
	//PrintVertices();
	//PrintElements();
}

HeightMap::HeightMap(char* file, const int& height, const int& width, const int& length) : HeightMap(Image(file), height, width, length) { }

HeightMap::HeightMap(char* file) {
	Image image = Image(file);
}

const vector<float>& HeightMap::Vertices() {
	return vertices;
}

const vector<unsigned int>& HeightMap::Elements() {
	return elements;
}

void HeightMap::PrintVertices() {
	for (int i = 0; i < vertices.size(); i += 3) {
		cout << vertices[i] << ", " << vertices[i + 1] << ", " << vertices[i + 2] << std::endl;
	}
	cout << std::endl << std::endl;
}

void HeightMap::PrintElements() {
	for (int i = 0; i < elements.size(); i += 3) {
		cout << elements[i] << ", " << elements[i + 1] << ", " << elements[i + 2] << std::endl;
	}
	cout << std::endl << std::endl;
}