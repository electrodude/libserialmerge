/**
 * This is the main libserialmerge program file.
 */

#include "simpletools.h"
#include "fdserial.h"

#include "serialmerge.h"

void cog2(void* term)
{
    serialmerge_register(term, "[other cog ] ");

    printi("Other cog starting! cogid = %d\n", cogid());

    while (1)
    {
        putStr("Chickens!\n");
        waitcnt(CNT+CLKFREQ/5);
    }
}

void cog3(void* term)
{
    serialmerge_register(term, "[blinky cog] ");

    printi("Blinky cog starting! cogid = %d\n", cogid());

    while (1)
    {
        high(15);
        putStr("LED on\n");
        waitcnt(CNT+CLKFREQ/5);
        low(15);
        putStr("LED off\n");
        waitcnt(CNT+CLKFREQ/5);
    }
}

int main(void)
{
    // make an fdserial and 
    serialmerge* term = serialmerge_new(fdserial_open(31,30,0,115200));

    simpleterm_close();
    simpleterm_set(term);

    serialmerge_register(term, "[main cog  ] ");

    putStr("Main cog starting!\n");

    int stacksize2 = sizeof(_thread_state_t) + 256*sizeof(int);
    int* stack2 = (int*)malloc(stacksize2);
    int id2 = cogstart(&cog2, term, stack2, stacksize2);
    printi("Other cog got id %d\n", id2);

    int stacksize3 = sizeof(_thread_state_t) + 256*sizeof(int);
    int* stack3 = (int*)malloc(stacksize3);
    int id3 = cogstart(&cog3, term, stack3, stacksize3);
    printi("Blinky cog got id %d\n", id3);

    while (1)
    {
        putStr("Test...\n");
        waitcnt(CNT+CLKFREQ/2);
    }
}

