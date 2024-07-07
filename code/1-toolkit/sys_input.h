#ifndef INPUT_H
#define INPUT_H
enum { KEY_ESC };
#endif

API int input(int key);

#if CODE
int input(int key) {
	static const bool *keys;
	static int numkeys;
	if(!keys) keys = SDL_GetKeyboardState(&numkeys);
	if( key == KEY_ESC ) return keys[SDL_SCANCODE_ESCAPE];

	return 0;
}
#endif

