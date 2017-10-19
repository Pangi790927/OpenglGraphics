#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

// #include <keysym.h>

class Keyboard {
public:
	int keys[512] = {0};	/// 256 keys, true means pressed 
	bool wasPressed[512] = {false};

	Keyboard() {}

	int getKeyFromEvent (XEvent &event) {
		char buf[2];
        int len;
        KeySym keysym_return;
        len = XLookupString(&event.xkey, buf, 1, &keysym_return, NULL);

        if ( len != 0 ){
            return buf[0];
        }
        else {
        	return 256 + event.xkey.keycode; 
        }
	}

	void handleEvent (XEvent &event) {
		if (event.type == KeyPress) {
			keys[getKeyFromEvent(event)] = true; 
		}
		else if (event.type == KeyRelease) {
			keys[getKeyFromEvent(event)] = false;
		}
	}

	bool wasPressedOnce(int key) {
		if (wasPressed[key] == false && keys[key] == true) {
			wasPressed[key] = true; 
			return true; 
		}
		if (keys[key] == false) {
			wasPressed[key] = false; 
		}
		return false; 
	}

	bool isPresed(int key) {
		return keys[key] == true; 
	}

	const static int ESC = 27; 
};

#endif