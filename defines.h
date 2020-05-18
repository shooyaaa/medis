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
    int rsize;
    char *writeBuf;
    int wsize;
    int port;
    struct _client *next;
};
typedef struct _client client;

typedef struct _signalList {
    client *c;
    struct _signalList *next;
} signalList;

typedef struct _command {
    char name[20];
    char **parmas;
    void (*handler)(client *c);
} command;

void pingCommand(client *c);

struct _net {
    int     fd;
    command table[10];
    client*    clientPool;
    signalList *readSignal;
    signalList *writeSignal;
};

typedef struct _net net;



int readClient(net n);
int writeClient(net n);
int createSocket(int port);
int poll(net *n);
int acceptClient(net *n);
void countClients(void *p);
int validClients(net *);
void addToLinkList(signalList **head, signalList *item);
void removeFromLinkList(signalList **head, signalList *item);

int handleSignal(net *n);
char *codec(client *c);
#endif
