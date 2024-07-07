#include "engine.h"

AUTORUN {
    puts("hello");
}

int main() {
    window_create(0.75, 0);
    while( window_swap() ) {

    }
}

const char *appname = __FILE__;
