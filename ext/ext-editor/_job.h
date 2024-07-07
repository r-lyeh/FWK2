API int         job_send(const char *cmd); // returns job-id
API const char* job_recv(int jobid, double timeout_ss);

static mutex_t *job_lock;
static array(char*) job_list;
int job_send(const char *cmd) { // return job-id
    int skip = strspn(cmd, " \t\r\n");
    char *buf = STRDUP(cmd + skip);
    strswap(buf, "\r\n", "");
    int jobid;
    do_threadlock(job_lock) {
        array_push(job_list, buf);
        jobid = array_count(job_list) - 1;
    }
    return jobid;
}
const char* job_recv(int jobid, double timeout_ss) {
    char *answer = 0;

    while(!answer && timeout_ss >= 0 ) {
        do_threadlock(job_lock) {
            if( job_list[jobid][0] == '\0' )
                answer = job_list[jobid];
        }
        timeout_ss -= 0.1;
        if( timeout_ss > 0 ) sleep_ms(100); // thread_yield()
    }

    return answer + 1;
}

