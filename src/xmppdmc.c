#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <xmpp_helper.h>

#include "xmppdmc.h"
#include "xmpp_chat.h"
#include "xmpp_common.h"

typedef struct _xmppdmc_t {
    xmpp_t *xmpp;
    char    presence[64];
    int     connected;
    xmppdmc_handler cbhandler;
    void *cbudata;
    xmppdmc_disconn_handler disconnhandler;
    void *disconnudata;
    xmppdmc_downlink_handler dlhandler;
    void *dludata;
} xmppdmc_t;

static int chat_recv_handler(xmpp_conn_t *conn, xmppchat_t *xdata, void *udata)
{
    xmppdmc_t *xdmc = (xmppdmc_t *) udata;
    struct _xmpp_t *xmpp = xdmc->xmpp;
    if (strncmp("downlink", xdata->subject, 8) == 0) {
        if (xdmc->dlhandler != NULL) {
            xdmc->dlhandler(xdmc->dludata, xdata->data, strlen(xdata->data));
        }
    }
    if (xmpp != NULL) {
        xmppdmc_handler hdl = xdmc->cbhandler;
        if (hdl != NULL) {
            hdl((void *) xdmc, xdata, xdmc->cbudata);
        }
    }
    return 0;
}

static int conn_handler(xmpp_t *xmpp, xmppconn_info_t *conninfo, void *udata)
{
    xmppdmc_t *xdmc = (xmppdmc_t *) udata;
    if (conninfo->connevent != 0) {
        fprintf(stderr, "  status(%d) error(%d) errorType(%d) errorText '%s'\n",
                conninfo->connevent, conninfo->error, conninfo->errortype,
                conninfo->errortext);
        xmppdmc_disconn_handler hdl = xdmc->disconnhandler;
        if (hdl != NULL) {
            hdl((void *) xdmc, conninfo->error, xdmc->disconnudata);
        }
        return 0;
    }
    //printf( "\n\n       login full JID: %s\n\n\n", xmpphelper_get_bound_jid(xmpp));
    if (strlen(xdmc->presence) > 0) {
        xmpp_presence(xmpphelper_get_conn(xmpp), xdmc->presence);
    }
    return 0;
}

void *xmppdmc_init(int dbg)
{
    xmppdmc_t *xdmc;
    xmpp_log_t *log;
    xdmc = malloc(sizeof(xmppdmc_t));
    if (xdmc == NULL) {
        return NULL;
    } else {
        memset(xdmc, 0, sizeof(xmppdmc_t));
    }
    if (dbg == 0) {
        log = NULL;
    } else {
        log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG);
    }
    xdmc->xmpp = xmpphelper_new(conn_handler, NULL, log, xdmc);
    return (void *) xdmc;
}

void xmppdmc_destroy(void *ins)
{
    xmppdmc_t *xdmc= (xmppdmc_t *) ins;
    xmpp_t *xmpp = xdmc->xmpp;
    if (xdmc->connected == 1) {
        xmpphelper_stop(xmpp);
        xmpphelper_join(xmpp);
        xmppchat_handler_del(xmpphelper_get_conn(xmpp), chat_recv_handler);
    }
    xmpphelper_release(xmpp);
    free(xdmc);
}

int xmppdmc_connect(void *ins, char *host, unsigned short port, char *jid, char *pwd)
{
    int rc = 0;
    xmppdmc_t *xdmc = (xmppdmc_t *) ins;
    xmpp_t *xmpp = xdmc->xmpp;
    rc = xmpphelper_connect(xmpp, host, port, jid, pwd);
    if (rc != 0) {
        return rc;
    }
    xdmc->connected = 1;
    xmppchat_handler_add(xmpphelper_get_conn(xmpp), chat_recv_handler, xdmc);
    xmpphelper_run(xmpp);
    usleep(0);
    return 0;
}

void xmppdmc_set_presence(void *ins, char *pres)
{
    xmppdmc_t *xdmc = (xmppdmc_t *) ins;
    if (strlen(pres) > 0) {
        snprintf(xdmc->presence, sizeof(xdmc->presence), "%s", pres);
    }
}

void xmppdmc_set_handler(void *ins, xmppdmc_handler hdl, void *udata)
{
    xmppdmc_t *xdmc = (xmppdmc_t *) ins;
    xdmc->cbhandler = (void *) hdl;
    xdmc->cbudata = udata;
}

void xmppdmc_set_disconn_handler(void *ins, xmppdmc_disconn_handler hdl, void *udata)
{
    xmppdmc_t *xdmc = (xmppdmc_t *) ins;
    xdmc->disconnhandler = (void *) hdl;
    xdmc->disconnudata = udata;
}

void xmppdmc_set_downlink_handler(void *ins, xmppdmc_downlink_handler hdl, void *udata)
{
    xmppdmc_t *xdmc = (xmppdmc_t *) ins;
    xdmc->dlhandler = (void *) hdl;
    xdmc->dludata = udata;
}

int xmppdmc_send(void *ins, xmppchat_t *chat)
{
    xmppdmc_t *xdmc = (xmppdmc_t *) ins;
    xmpp_t *xmpp = xdmc->xmpp;
    xmppchat_send_message(xmpphelper_get_conn(xmpp), chat);
    return 0;
}
