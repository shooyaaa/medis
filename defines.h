#ifndef __defines__
#define __defines__

#define __debug 1
#ifdef __debug
#define debug(m) printf(m)
#else
#define debug(m)
#endif
#define MAX_BUFFER_SIZE 1024

#define FATAL(msg) do {printf(msg);exit(2);} while (0)
#include <stdio.h>
#include "dm_string.h"
#include "resp.h"
#include "hash.h"

typedef enum connection_state {
    STATE_OK,
    STATE_ERROR
} connState;

struct _client {
    int fd;
    char *readBuf;
    int rsize;
    char *writeBuf;
    int wsize;
    int port;
    connState state;
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
    resp *(*handler)(resp *rp);
} command;

struct _net {
    int     fd;
    command table[10];
    client*    clientPool;
    signalList *readSignal;
    signalList *writeSignal;
    signalList *closedClient;
    hash *globalHash;
};


typedef struct _net net;

resp *pingCommand(resp *rp);
resp *configCommand(resp *rp);
resp *getCommand(resp *rp);
resp *setCommand(resp *rp);
resp *delCommand(resp *rp);


int readClient(net n);
int writeClient(net n);
int createSocket(int port);
int poll(net *n);
int acceptClient(net *n);
void freeClient(client *c, net *n);
void countClients(void *p);
int validClients(net *);
void addToLinkList(signalList **head, signalList *item);
void removeFromLinkList(signalList **head, signalList *item);

int handleSignal(net *n);
resp *codec(client *c);
#endif
