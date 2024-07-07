void (*timer_dd[16])() = {0};
void (*timer_hh[16])() = {0};
void (*timer_mm[16])() = {0};
void (*timer_ss[16])() = {0};
void (*timer_ff[16])() = {0};

void events(void (*timer[])() ) {
    int i = 0;
    while( timer[i] ) timer[i++]();
}

void quak() {
    puts("quak!");
}

main() {
    timer_mm[0] = quak;

    for( int d = 0; ; ++d ) {
        for( int h = 0; h < 24; ++h ) {
            for( int m = 0; m < 60; ++m ) {
                for( int s = 0; s < 60; ++s ) {
                    for( int f = 0; f < 60; ++f ) {
                        printf("%dd %02dh:%02dm:%02ds.%03df\n", d, h, m, s, f);
                        events(timer_ff);
                    }
                    events(timer_ss);
                }
                events(timer_mm);
            }
            events(timer_hh);
        }
        events(timer_dd);
    }
}
