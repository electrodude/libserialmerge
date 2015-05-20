#include "simpletext.h"

#define BUF_LEN 128

// Don't change this.  Each cog gets exactly one buffer (but it might not be allocated)
#define MAX_BUFFERS 8

typedef text_t serialmerge;

typedef struct
{
    char* name;
    char buf[BUF_LEN];
    int head;
    int tail;
} serialmerge_buffer;

typedef struct
{
    text_t* device;
    serialmerge_buffer* buffers[MAX_BUFFERS];
    int cogid;
    int* stack;
} serialmerge_master;

serialmerge* serialmerge_new(text_t* device);

serialmerge_buffer* serialmerge_register(volatile serialmerge* device, char* name);
