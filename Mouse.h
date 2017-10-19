#ifndef MOUSE_H_INCLUDED
#define MOUSE_H_INCLUDED

#include "MathLib.h"

class Mouse {
public:
	Point2i position; 
	Point2i lastPosition; 

	int keys[6] = {0};
	bool wasPressed[6] = {false};

	Mouse (Point2i pos = Point2i(0, 0)) : position(pos), lastPosition(pos) {}

	void handleEvent (XEvent &event) {
		if (event.type == ButtonPress) {
			if (event.xbutton.button < 6)
				keys[event.xbutton.button] = true;
		}
		else if (event.type == ButtonRelease) {
			if (event.xbutton.button < 6)
				keys[event.xbutton.button] = false;
		}
		else if (event.type == MotionNotify) {
			lastPosition = position;
			
			position = Point2i(event.xmotion.x, event.xmotion.y);
		}
	}

	bool wasPressedOnce (int key) {
		if (wasPressed[key] == false && keys[key] == true) {
			wasPressed[key] = true; 
			return true; 
		}
		if (keys[key] == false) {
			wasPressed[key] = false; 
		}
		return false; 
	}

	bool isPresed (int key) {
		return keys[key] == true; 
	}

	static Point2f transformeMousePos(Point2f mousePos, int width, int height) {
		Point3f pos = (
			Matrix4f::returnScaleMatrix(1, -1, 1) * 
			Point3f(
				(
					Matrix4f::returnScaleMatrix(1.0 / width, 1.0 / height, 1) * 
					Point3f(mousePos) - 
					Point3f(0.5, 0.5, 0)
				) * 
				2.0f
			)
		);
		return Point2f(pos.x, pos.y);
	}

	Point2f getLastPosition() {
		return Point2f(lastPosition.x, lastPosition.y);
	}

	Point2f getPosition() {
		return Point2f(position.x, position.y);
	}

	bool mouseLeftOnce() {
		return wasPressedOnce(1);
	}

	bool mouseMiddleOnce() {
		return wasPressedOnce(2);
	}

	bool mouseRightOnce() {
		return wasPressedOnce(3);
	}

	bool mouseLeft() {
		return isPresed(1); 
	}

	bool mouseMiddle() {
		return isPresed(2);
	}

	bool mouseRight() {
		return isPresed(3);
	}
};

#endif