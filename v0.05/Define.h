
#define MAX_EVENTS 500
#define INIT_EVENTS 16
#define MAX_BUFFER 1024

#define ERR_EXIT(m) \
        do { \
            perror(m); \
            exit(EXIT_FAILURE); \
        } while(0)
