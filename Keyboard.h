#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

#include <X11/keysymdef.h>

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
			int key = getKeyFromEvent(event);

			keys[tolower(key)] = true;
			keys[toupper(key)] = true;

			keys[key] = true; 
		}
		else if (event.type == KeyRelease) {
			int key = getKeyFromEvent(event);

			keys[tolower(key)] = false;
			keys[toupper(key)] = false;
		}
	}

	void setKey (int key, bool set) {
		keys[key] = set;
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

	bool isPressed(int key) {
		return keys[key] == true; 
	}

	const static int ESC = XK_Escape & 0xff; 
	const static int L_SHIFT = XK_Shift_L & 0xff; 
};

#endif