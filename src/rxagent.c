#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#include "rx_common.h"
#include "restdmc.h"
#include "rxagent.h"

#include "xmpp_helper.h"
#include "xmpp_chat.h"
#include "xmppdmc.h"

typedef struct _rx {
    void *dmc;
    char jid[256];
    rx_downlink_cb_t dlcb;
    char restdmservice[128];
    dmc_xmppconfig_t *xcfg;
    xmpp_t *xmpp;
    int dbgflags;
} rx_t ;

static int xmppdmc_recv_cb(void *ins, xmppchat_t *chat, void *udata);
static int xmppdmc_mesg_cb(void *ins, unsigned char *data, size_t size);
static void * pth_conn(void *ptr);
static int xmppdmc_disconn_cb(void *ins, int err, void *udata);

static int xmppdmc_recv_cb(void *ins, xmppchat_t *chat, void *udata)
{
    DBG("<%p> sub '%s' msg '%s'", udata, chat->subject, (char *) chat->data);
    chat->tojid = chat->from;
    chat->data = "OK";
    xmppdmc_send(ins, chat);
    return 1;
}

static int xmppdmc_mesg_cb(void *ins, unsigned char *data, size_t size)
{
    DBG("data<%p> size(%ld)", data, (long int)size);

    rx_t *rx = (rx_t *) ins;
    DBG("rx<%p>->dlcb<%p>", rx, rx->dlcb);
    rx->dlcb(ins, data, size);
    return 1;
}

static void * pth_conn(void *ptr)
{
    int rc;
    rx_t *rx = ptr;
    usleep(500000);
    if (rx->xmpp != NULL) {
        DBG("rx(%p) xmppdmc_destroy(%p)", rx, rx->xmpp);
        xmppdmc_destroy(rx->xmpp);
        rx->xmpp = NULL;
    }
    usleep(1000000);
    rx->xmpp = xmppdmc_init(rx->dbgflags & RX_FLAGS_STROPHE_DEBUG);
    xmppdmc_set_handler(rx->xmpp, xmppdmc_recv_cb, (void *) rx);
    xmppdmc_set_disconn_handler(rx->xmpp, xmppdmc_disconn_cb, (void *) rx);
    xmppdmc_set_downlink_handler(rx->xmpp, xmppdmc_mesg_cb, (void *) rx);
    xmppdmc_set_presence(rx->xmpp, rx->xcfg->presence);

    DBG("rx(%p) xmppdmc_connect(%p)", rx, rx->xmpp);
    rc = xmppdmc_connect(rx->xmpp, rx->xcfg->host, rx->xcfg->port, rx->jid, rx->xcfg->password);
    DBG("rx(%p) xmppdmc_connect() rc %d return", rx, rc);
    if (rc != 0) {
        pthread_t pth;
        DBG("rc(%d) xmppdmc_connect() failed.\n", rc);
        pthread_create(&pth, NULL, pth_conn, ptr);
    }
    return NULL;
}

static int xmppdmc_disconn_cb(void *ins, int err, void *udata)
{
    pthread_t pth;
    DBG("int<%p> u<%p> error '%d'", ins, udata, err);
    pthread_create(&pth, NULL, pth_conn, udata);
    return 1;
}

void *rx_init(char *protocol, char *host, unsigned short port, char *gw, char *key, int flags)
{
    rx_t *rx;
    rx = malloc(sizeof(rx_t));
    rx->dbgflags = flags;
    DBG("prot %s host %s port %u GW %s flags %x", protocol, host, port, gw, flags);
    rx->dmc = dmc_init(protocol, host, port, key, gw, rx->dbgflags & RX_FLAGS_CURL_DEBUG);
    int rc = dmc_getconfig(rx->dmc);
    if (rc != 200) {
        DBG("rc(%d) dmc_getconfig()", rc);
        rx_destroy(rx);
        return NULL;
    }
    rx->xcfg = dmc_getxmppconfig(rx->dmc);
    DBG("xmpp s %s h %s p %u ", rx->xcfg->server, rx->xcfg->host, rx->xcfg->port);
    DBG("     u %s w %s p %s", rx->xcfg->username, rx->xcfg->password, rx->xcfg->presence);
    strcpy(rx->jid, rx->xcfg->username);
    strcat(rx->jid, "/");
    strcat(rx->jid, gw);
    DBG("JID: %s", rx->jid);
    pthread_t pth;
    pthread_create(&pth, NULL, pth_conn, rx);
    signal(SIGPIPE, SIG_IGN);

    return rx;
}

void rx_destroy(void *ins)
{
    rx_t *rx = (rx_t *) ins;
    DBG("rx<%p>", rx);
    if (rx != NULL) {
        if (rx->dmc != NULL) {
            dmc_destroy(rx->dmc);
        }
        if (rx->xmpp != NULL) {
            xmppdmc_destroy(rx->xmpp);
        }
        free(rx);
    }
    return;
}

int rx_uplink(void *ins, int type, char *data)
{
    DBG("type %d data '%s'", type, data);
    rx_t *rx = (rx_t *) ins;
    int rc = dmc_uplink(rx->dmc, type, data);
    if (rc == 401) {
        DBG("ERROE rc(%d)", rc);
        rc = dmc_getconfig(rx->dmc);
        if (rc != 200) {
            DBG("rc(%d) dmc_getconfig()", rc);
        }
        rc = dmc_uplink(rx->dmc, type, data);
    }
    DBG("rc(%d)", rc);
    return rc == 200 ? 0 : rc;
}

int rx_init_downlink_cb(void *ins, rx_downlink_cb_t cb)
{
    rx_t *rx = (rx_t *) ins;
    rx->dlcb = cb;
    DBG("rx<%p>->dlcb<%p>", rx, rx->dlcb);
    return 0;
}

