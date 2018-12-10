#ifndef __XMPPDMC_H_
#define __XMPPDMC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "xmpp_chat.h"

typedef int (*xmppdmc_handler)(void *instance, xmppchat_t *chat, void *udata);
typedef int (*xmppdmc_disconn_handler)(void *instance, int error, void *udata);
typedef int (*xmppdmc_downlink_handler)(void *instance, unsigned char *data, size_t size);

void *xmppdmc_init(int debug);

void xmppdmc_destroy(void *instance);

void xmppdmc_set_presence(void *instance, char *presence);
void xmppdmc_set_handler(void *instance, xmppdmc_handler hdl, void *udata);
void xmppdmc_set_disconn_handler(void *instance, xmppdmc_disconn_handler hdl, void *udata);
void xmppdmc_set_downlink_handler(void *instance, xmppdmc_downlink_handler hdl, void *udata);

int xmppdmc_connect(void *instance, char *host, unsigned short port, char *jid, char *pwd);

int xmppdmc_send(void *instance, xmppchat_t *chat);

#ifdef __cplusplus
}
#endif

#endif//__XMPPDMC_H_
