#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <json-c/json.h>
#include "rxagent.h"

#define LOOPPOST
#ifdef LOOPPOST
#define LP_INTERVAL 15
#define LP_COUNT 2000
#endif

static void *rx;

char *newdev1  = "{\"id\":\"gw0000\",\"type\":\"gateway\"}";
char *newdev2  = "{\"id\":\"li0000\",\"type\":\"device\",\"parent\":\"gw0000\"}";
char *newdev3  = "{\"id\":\"ss0000\",\"type\":\"sensor\",\"parent\":\"li0000\"}";
char *dev1data = "{\"id\":\"gw0000\",\"rt\":\"wks.r.gateway\"}";
char *dev2data = "{\"id\":\"li0000\",\"rt\":\"wks.r.light\"}";
char *dev3data = "{\"id\":\"ss0000\",\"rt\":\"wks.r.sensor\"}";
char *dev1evt  = "{\"id\":\"gw0000\",\"code\":\"1234\"}";
char *dev2evt  = "{\"id\":\"li0000\",\"code\":\"234\"}";
char *dev3evt  = "{\"id\":\"ss0000\",\"code\":\"34\"}";
char *dlid = NULL;

static char *getisotime()
{
    static char _ST_[22] = "";
    time_t tt; time(&tt);
    strftime( _ST_, sizeof( _ST_ ), "%Y-%m-%dT%H:%M:%SZ", gmtime(&tt));
    return _ST_;
}

static void reset(json_object *obj)
{
    char resp[512]= {0};
    const char *cmd = json_object_get_string(json_object_object_get(obj, "cmd"));
    const char *dlid = json_object_get_string(json_object_object_get(obj, "dl_id"));
    sleep(random() % 10);
    snprintf(resp, sizeof(resp), "{\"dl_id\":\"%s\",\"result\":\"success\",\"message\":\"'%s' OK\",\"resp\":{\"aa\":\"bb\",\"cc\":1234,\"time\":\"%s\"}}", dlid, cmd, getisotime());
    rx_uplink(rx, UP_DLRESP, resp);
    json_object_put(obj);
}

static void reboot(json_object *obj)
{
    char resp[512]= {0};
    const char *cmd = json_object_get_string(json_object_object_get(obj, "cmd"));
    const char *dlid = json_object_get_string(json_object_object_get(obj, "dl_id"));
    snprintf(resp, sizeof(resp), "{\"dl_id\":\"%s\",\"result\":\"success\",\"message\":\"'%s' OK\",\"resp\":{\"aa\":\"bb\",\"cc\":1234,\"time\":\"%s\"}}", dlid, cmd, getisotime());
    rx_uplink(rx, UP_DLRESP, resp);
    json_object_put(obj);
}

static void deffunc(json_object *obj)
{
    char resp[512]= {0};
    const char *cmd = json_object_get_string(json_object_object_get(obj, "cmd"));
    const char *dlid = json_object_get_string(json_object_object_get(obj, "dl_id"));
    snprintf(resp, sizeof(resp), "{\"dl_id\":\"%s\",\"result\":\"success\",\"message\":\"'%s' OK\",\"resp\":{\"aa\":\"bb\",\"cc\":1234,\"time\":\"%s\"}}", dlid, cmd, getisotime());
    rx_uplink(rx, UP_DLRESP, resp);
    json_object_put(obj);
}

static void *dlfunc(void *ptr)
{
    json_object *obj = (json_object *) ptr;
    const char *cmd = json_object_get_string(json_object_object_get(obj, "cmd"));
    printf("Downlink Command '%s'\n", cmd);
    if (strncasecmp(cmd, "reset", 5) == 0) {
        reset(obj);
    } else if (strncasecmp(cmd, "reboot", 6) == 0) {
        reboot(obj);
    } else {
        deffunc(obj);
    }
}

void dlcb (void *ins, unsigned char *data, int size)
{
    pthread_t pth;
    json_object *obj;
    fprintf(stderr, "%s() data '%s' size %d\n", __FUNCTION__, data, size);
    //dlid = get_id(data, "\"dl_id\"");
    obj = json_tokener_parse(data);
    fprintf(stderr, "%s() data '%s' size %d\n", __FUNCTION__, data, size);
    //printf("dlid %s, cmd %s\n", json_object_get_string(json_object_object_get(obj, "dl_id")), json_object_get_string(json_object_object_get(obj, "cmd")));
    pthread_create(&pth, NULL, dlfunc, obj);
    pthread_detach(pth);
}

int main(int argc, char *argv[])
{
    int rc;

    char    *key = "DJAUozi8MIP-KoKeKZrLtw==";
    char *protocol = "https";
    uint16_t port = 8080;
    char    *host = "cloud02.workssys.com";
    char    *agid = "iop-agent";
    int      flags = RX_FLAGS_CURL_DEBUG | RX_FLAGS_STROPHE_DEBUG;
    //int      flags = 0;
    char dev[16];

    if (argc == 2) {
        agid = argv[1];
    }

    rx = rx_init(protocol, host, port, agid, key, flags);
    if (rx == NULL) {
        return -1;
    }
    rx_init_downlink_cb(rx, dlcb);

    sleep(2);
    rx_uplink(rx, UP_NEWDEVICE, newdev1);
    rx_uplink(rx, UP_NEWDEVICE, newdev2);
    rx_uplink(rx, UP_NEWDEVICE, newdev3);

    rc = rx_uplink(rx, UP_NEWDEVICE, "{}");
    if (rc != 0) {
        fprintf(stderr, "rc %d\n", rc);
    }
#ifdef LOOPPOST
    while (true) {
        rx_uplink(rx, UP_DEVDATA, dev1data);
        rx_uplink(rx, UP_DEVDATA, dev2data);
        rx_uplink(rx, UP_DEVDATA, dev3data);
        if ((random() & 0xf) == 0) {
            rx_uplink(rx, UP_DEVEVENT, dev1evt);
            rx_uplink(rx, UP_DEVEVENT, dev2evt);
            rx_uplink(rx, UP_DEVEVENT, dev3evt);
        }
        sleep(LP_INTERVAL);
    }
#endif

    rx_destroy(rx);

    return 0;
}
