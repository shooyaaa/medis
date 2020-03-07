#ifndef __defines__
#define __defines__

#define __debug 1
#ifdef __debug
#define debug(m) printf(m)
#else
#define debug(m)
#endif
#define MAX_BUFFER_SIZE 1024
struct _client {
    int fd;
    char *readBuf;
    char *writeBuf;
    struct _client *next;
};
typedef struct _client client;

struct _net {
    int     fd;
    client*    clientPool;
    client *readSignal;
    client *writeSignal;
};

typedef struct _net net;

int readClient(net n);
int writeClient(net n);
int createSocket(int port);
int poll(net *n);
int addReadSignal(client c);
int addWriteSignal(client c);
int acceptClient(net *n);
int countClients(net *n);
int validClients(net *);
#endif
