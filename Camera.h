#ifndef CAMER_H_INCLUDED
#define CAMER_H_INCLUDED

#include "MathLib.h"
#include "Mouse.h"
#include "Keyboard.h"

class Camera {
public:
	Camera() {}

	float speed = 0.1;
	float rotationSpeed = 0.3;
	float mouseMoveTreshold = 0.01;

	Matrix4f initialTransformation = Matrix4f::returnIdentityMatrix() ;
	Matrix4f transformMatrix = Matrix4f::returnIdentityMatrix(); 
	
	Matrix4f getViewTransform() {
		return transformMatrix * initialTransformation; 
	}

	void setInput (Keyboard &keyboard, Mouse &mouse, int width, int height) {
		Point2f mousePosition = Mouse::transformeMousePos(mouse.getPosition(), width, height);

		static Matrix4f rotationX = Matrix4f::returnIdentityMatrix();
		static Matrix4f rotationY = Matrix4f::returnIdentityMatrix(); 
		
		if (mousePosition.abs() > mouseMoveTreshold) {
			rotationX = Matrix4f::returnRotationMatrix(rotationSpeed * -mousePosition.x, 0, 1, 0 ) * rotationX; 
			rotationY = Matrix4f::returnRotationMatrix(rotationSpeed * mousePosition.y, 1, 0, 0 ) * rotationY;
		}

		Matrix4f rotation = rotationY * rotationX; 
		Matrix4f inverseRotationY = rotationX.returnTransposedMatrix();

		static Point3f cameraPos;

		if (keyboard.isPresed('w')) {
			cameraPos = cameraPos - inverseRotationY * Point3f(0, 0, speed);
		}

		if (keyboard.isPresed('s')) {
			cameraPos = cameraPos + inverseRotationY * Point3f(0, 0, speed);
		}

		if (keyboard.isPresed('d')) {
			cameraPos = cameraPos + inverseRotationY * Point3f(speed, 0, 0);
		}

		if (keyboard.isPresed('a')) {
			cameraPos = cameraPos - inverseRotationY * Point3f(speed, 0, 0);
		}

		if (keyboard.isPresed('q')) {
			cameraPos = cameraPos + Point3f(0, speed, 0);
		}

		if (keyboard.isPresed('e')) {
			cameraPos = cameraPos - Point3f(0, speed, 0);
		}

		Matrix4f translation = Matrix4f::returnTranslationMatrix(cameraPos * -1);

		transformMatrix = rotation * translation;
	}
};



#endif