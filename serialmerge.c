// supplies _thread_state_t
#include <sys/thread.h>

#include <stdlib.h>

#include "serialmerge.h"

/*
// private
void inline serialmerge_putc(text_t* device, char c)
{
    device->txChar(device, c);
}
// */

char n2hex(int x)
{
    static char chars[] = "0123456789ABCDEF";
    return chars[x&15];
}

// private
void serialmerge_cog(void* _this)
{
    serialmerge_master* this = (serialmerge_master*)_this;
    int cbuf;
    while (1)
    {
        for (cbuf=0; cbuf<MAX_BUFFERS; cbuf++)
        {
            //this->device->txChar(this->device, '0'+cbuf);
            //waitcnt(CNT+CLKFREQ/4);
            volatile serialmerge_buffer* buf;
            if ((buf=this->buffers[cbuf]) != NULL)
            {
                //this->device->txChar(this->device, 'e');
                //waitcnt(CNT+CLKFREQ/4);
                if (buf->tail != buf->head)
                {
                    //this->device->txChar(this->device, 'm');
                    //waitcnt(CNT+CLKFREQ/4);
                    // *
                    char* ptr = buf->name;
                    while (*ptr)
                    {
                        this->device->txChar(this->device, *ptr++);
                        //waitcnt(CNT+CLKFREQ/4);
                    }
                    // */
                    //waitcnt(CNT+CLKFREQ/4);
                    char c;
                    do
                    {
                        while (buf->tail == buf->head);
                        int nexttail = buf->tail;
                        if (++nexttail >= BUF_LEN)
                        {
                            nexttail = 0;
                        }
                        c = buf->buf[nexttail];
                        buf->tail = nexttail;
                        /*
                        this->device->txChar(this->device, '[');
                        this->device->txChar(this->device, n2hex(buf->tail>>4));
                        this->device->txChar(this->device, n2hex(buf->tail));
                        this->device->txChar(this->device, ']');
                        this->device->txChar(this->device, '=');
    
                        this->device->txChar(this->device, n2hex(c>>4));
                        this->device->txChar(this->device, n2hex(c));
                        this->device->txChar(this->device, ' ');
                        waitcnt(CNT+CLKFREQ/4);
                        // */
                        if (c >= 32 || c == 10 || c == 13)
                        {
                            this->device->txChar(this->device, c);
                        }
                        else
                        {
                            this->device->txChar(this->device, '$');
                            this->device->txChar(this->device, n2hex(c>>4));
                            this->device->txChar(this->device, n2hex(c));
                            this->device->txChar(this->device, ' ');
                        }
                    }
                    while (c != 10);
                }
            }
        }
    }
}

// private
int serialmerge_rx(struct text_struct* this);

// private
int serialmerge_tx(struct text_struct* this, int c);

serialmerge* serialmerge_new(text_t* device)
{
    serialmerge_master* master = (serialmerge_master*)malloc(sizeof(serialmerge_master));

    master->device = device;

    memset(master->buffers, 0, MAX_BUFFERS*sizeof(serialmerge_buffer*));

    int stacksize = sizeof(_thread_state_t) + 256*sizeof(int);
    master->stack = (int*)malloc(stacksize);
    master->cogid = cogstart(&serialmerge_cog, master, master->stack, stacksize);

    serialmerge* this = (serialmerge*)malloc(sizeof(serialmerge));
    this->rxChar = &serialmerge_rx;
    this->txChar = &serialmerge_tx;
    this->devst = master;

    return this;
}

serialmerge_buffer* serialmerge_register(volatile serialmerge* device, char* name)
{
    volatile serialmerge_master* master = (serialmerge_master*)device->devst;

    serialmerge_buffer* buf = (serialmerge_buffer*)malloc(sizeof(serialmerge_buffer));

    buf->name = name;

    buf->head = 0;
    buf->tail = 0;

    int i = cogid();

    if (i == MAX_BUFFERS)
    {
        return NULL;
    }

    master->buffers[i] = buf;

    return buf;
}

// private
// unimplemented: this would allow a cog to get input but we'd need a lock to arbitrate it.
int serialmerge_rx(struct text_struct* this)
{
    //volatile serialmerge_buffer* buf = ((serialmerge_master*)this->devst)->buffers[cogid()];

    return 0;
}

// private
int serialmerge_tx(struct text_struct* this, int c)
{
    volatile serialmerge_buffer* buf = ((serialmerge_master*)this->devst)->buffers[cogid()];
    
    int nexthead = (buf->head+1)%BUF_LEN;
    if (nexthead != buf->tail)
    {
        buf->buf[nexthead] = c;
        buf->head=nexthead;
    }

    return c;
}