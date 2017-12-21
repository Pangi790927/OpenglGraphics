#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include <cstring>

#include "Util.h"

class Texture {
public: 
	int width; 
	int height; 

	int pixelSize = 4; 
	int format = 0;
	int type = 0; 

	GLuint openglTexture = 0; 

	std::shared_ptr<unsigned char> textureData; 

	void alocateSpace() {
		textureData = std::shared_ptr<unsigned char>(new unsigned char[width * height * pixelSize]
							, Util::array_deleter<unsigned char>()); 
	}

	int dataSize() {
		return width * height * pixelSize;
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