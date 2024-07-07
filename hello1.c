#include "engine.h"

AUTORUN {
	puts("hello");
}

int main() {
	window_create(0.75, 0);
	while( window_swap() ) {
//		glClearColor(1,0,1,1);
//		glClear(GL_COLOR_BUFFER_BIT);
	}
}
