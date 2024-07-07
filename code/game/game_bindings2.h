#if !CODE

API int binding(const char *expression); // parse expressions like "down(X) & input(CTRL)"

#else

int binding(const char *expression) {
    if( expression && expression[0] ) {
        return eval(expression) > 0;
    }
    return 0;
}

#endif
