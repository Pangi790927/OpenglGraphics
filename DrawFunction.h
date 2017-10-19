#ifndef DRAWFUNCTION_H_INCLUDED
#define DRAWFUNCTION_H_INCLUDED 

#include "OpenGLWindow.h"

class DrawFunction {
public:
	OpenGLWindow &window;
	
	void init() {
		static std::once_flag flag;
		std::call_once(flag, [this](){ initScene(); });
	}

	DrawFunction (OpenGLWindow &window) : window(window) {}

	virtual void initScene() = 0;

	virtual void mainLoop() = 0;

	virtual void onRescale() = 0;
};

#endif