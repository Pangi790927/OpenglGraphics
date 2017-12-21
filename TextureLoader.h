#ifndef TEXTURELOADER_H_INCLUDED
#define TEXTURELOADER_H_INCLUDED

#include "Texture.h"

#include <memory>
#include <cstring>

#include "Util.h"

/// stb: not my library, credit goes to: Sean T. Barrett 
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class TextureLoader {
public:
	Texture rawTexture; 

	TextureLoader() {}

	~TextureLoader() {}

	Texture loadTexture (std::string filename) {
		unsigned char *data = stbi_load(filename.c_str(), 
   									&rawTexture.width, 
   									&rawTexture.height, 
   									&rawTexture.pixelSize, 0);

		rawTexture.alocateSpace();

		std::copy(data, data + rawTexture.dataSize(), rawTexture.textureData.get()); 
   		
   		stbi_image_free(data);

   		std::cout << filename << " " << rawTexture.width << " " << rawTexture.height << std::endl; 
   		rawTexture.generateOpenGLTexture();

		return rawTexture; 
	}
};


#endif