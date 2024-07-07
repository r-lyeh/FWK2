//-----------------------------------------------------------------------------
// capture tests
static
uint64_t tests_captureframes() {
    static uint64_t capture_target; do_once capture_target = optioni("--capture", 0);
    return capture_target;
}

