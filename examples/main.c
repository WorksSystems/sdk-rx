#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "rxagent.h"

#define LOOPPOST
#ifdef LOOPPOST
#define LP_INTERVAL 15
#define LP_COUNT 2000
#endif

char *newdev1  = "{\"id\":\"gw0000\",\"type\":\"gateway\"}";
char *newdev2  = "{\"id\":\"li0000\",\"type\":\"light\",\"parent\":\"gw0000\"}";
char *newdev3  = "{\"id\":\"ss0000\",\"type\":\"sensor\",\"parent\":\"li0000\"}";
char *dev1data = "{\"id\":\"gw0000\",\"rt\":\"wks.r.gateway\"}";
char *dev2data = "{\"id\":\"li0000\",\"rt\":\"wks.r.light\"}";
char *dev3data = "{\"id\":\"ss0000\",\"rt\":\"wks.r.sensor\"}";
char *dev1evt  = "{\"id\":\"gw0000\",\"code\":\"1234\"}";
char *dev2evt  = "{\"id\":\"li0000\",\"code\":\"234\"}";
char *dev3evt  = "{\"id\":\"ss0000\",\"code\":\"34\"}";
char *dlid = NULL;

char *get_id(char *s, char *k)
{
    int i = 0;
    int c = 0;
    static char id[64] = {0};
    char *tmp;
    tmp = strstr(s, k);
    if (tmp == NULL) return "";
    tmp += strlen(k);
    printf("\n\n");
    for (;strlen(tmp) > 0; tmp++) {
        printf("%c ", *tmp);
        if (c == 0) {
            if (*tmp == ':') { c++; }
        } else if (c == 1) {
            if (*tmp == '"') { c++; }
        } else if (c == 2) {
            if (*tmp == '"') {
                c++;
            } else {
                id[i++] = *tmp;
            }
        } else if (c == 3) {
            break;
        }
    }
    printf("\n%s\n", id);
    return id;
}

void dlcb (void *ins, unsigned char *data, int size) {
    fprintf(stderr, "%s() data '%s' size %d\n", __FUNCTION__, data, size);
    dlid = get_id(data, "\"dl_id\"");
}

int main(int argc, char *argv[])
{
    int rc;
    void *rx;

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
    } else if (argc == 3) {
		agid = argv[1];
		key = argv[2];
	} else {
		printf("Usage: %s AgentID [SharedKey]\n", argv[0]);
		return 0;
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
        if (dlid != NULL) {
            char resp[512]= {0};
            snprintf(resp, sizeof(resp), "{\"dl_id\":\"%s\",\"result\":\"success\",\"message\":\"OK\",\"resp\":{\"aa\":\"bb\",\"cc\":1234}}", dlid);
            rx_uplink(rx, UP_DLRESP, resp);
            dlid = NULL;
        }
        sleep(LP_INTERVAL);
    }
#endif

    rx_destroy(rx);

    return 0;
}
