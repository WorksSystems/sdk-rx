#ifndef __XMPPCHAT__
#define __XMPPCHAT__

#ifdef __cplusplus
extern "C"
{
#endif

#include "strophe.h"
#include "xmpp_types.h"

typedef struct _xmppchat_t
{
    void *data;
    int size;
    char *tojid;
    char *from;
    char *subject;
    char *thread;
} xmppchat_t;

typedef int (*xmppchat_handler)(xmpp_conn_t *conn, xmppchat_t *msgdata, void *udata);

int xmppchat_send_message(xmpp_conn_t *conn, xmppchat_t *msgdata);

void xmppchat_handler_add(xmpp_conn_t *conn, xmppchat_handler handler, void *udata);

void xmppchat_handler_del(xmpp_conn_t *conn, xmppchat_handler handler);

#ifdef __cplusplus
}
#endif

#endif//__XMPPCHAT__
