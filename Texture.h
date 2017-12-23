#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include <cstring>

#include "Util.h"

class Texture {
public: 
	int width; 
	int height; 

	int pixelSize;

	GLuint openglTexture = 0; 

	std::shared_ptr<unsigned char> textureData; 

	Texture (int width = 0, int height = 0, int pixelSize = 3) 
	: width(width), height(height), pixelSize(pixelSize)
	{
		if (width != 0 && height != 0) {
			allocateTexture();	
		}
	}

	unsigned char& get_r (int row, int col) {
		return textureData.get()[(row * width + col) * pixelSize + 0];
	}

	unsigned char& get_g (int row, int col) {
		return textureData.get()[(row * width + col) * pixelSize + 1];
	}

	unsigned char& get_b (int row, int col) {
		return textureData.get()[(row * width + col) * pixelSize + 2];
	}

	unsigned char& get_a (int row, int col) {
		return textureData.get()[(row * width + col) * pixelSize + 3];
	}

	Point3f get_rgb(int row, int col) {
		return Point3f(
			get_r(row, col) / 255.,
			get_g(row, col) / 255.,
			get_b(row, col) / 255.
		);
	}

	Point4f get_rgba(int row, int col) {
		return Point4f(
			get_r(row, col) / 255.,
			get_g(row, col) / 255.,
			get_b(row, col) / 255.,
			get_a(row, col) / 255.
		);
	}

	void set_rgb(int row, int col, Point3f color) {
		get_r(row, col) = color.x * 255.;
		get_g(row, col) = color.y * 255.;
		get_b(row, col) = color.z * 255.;
	}

	void set_rgba(int row, int col, Point4f color) {
		get_r(row, col) = color.x * 255.;
		get_g(row, col) = color.y * 255.;
		get_b(row, col) = color.z * 255.;
		get_a(row, col) = color.w * 255.;
	}

	void allocateTexture() {
		textureData = std::shared_ptr<unsigned char>(new unsigned char[width * height * pixelSize]);
	}

	void alocateSpace() {
		textureData = std::shared_ptr<unsigned char>(new unsigned char[width * height * pixelSize]
							, Util::array_deleter<unsigned char>()); 
	}

	int dataSize() {
		return width * height * pixelSize;
	}

	void bind() {
		glBindTexture(GL_TEXTURE_2D, openglTexture);
	}

	void generateOpenGLTexture() {
		glGenTextures(1, &openglTexture);

		glBindTexture(GL_TEXTURE_2D, openglTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// glGenerateMipmap(GL_TEXTURE_2D);

		std::cout << "pixelSize: " << pixelSize << std::endl;

		GLint format = GL_RGB;

		if (pixelSize == 0){
			std::cout << "Invalid Pixel Format" << std::endl; 
		}
		else if (pixelSize == 1 || pixelSize == 2) {
			format = GL_ALPHA;  
		}
		else if (pixelSize == 3) {
			format = GL_RGB;  
		}
		else if (pixelSize == 4) {
			format = GL_RGBA;  
		}

		glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			format, 
			width, 
			height, 
			0, 
			format, 
			GL_UNSIGNED_BYTE, 
			textureData.get()
		);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}; 

#endif