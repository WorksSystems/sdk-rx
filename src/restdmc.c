#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>
#include <curl/curl.h>

#include "restdmc.h"

#define AGCONF "/v1/sbi/agent/config/"
#define AGDATA "/v1/sbi/agent/data/"
#define DEVICE "/v1/sbi/device/"
#define DVDATA "/v1/sbi/device/data/"
#define DEVTKN "/v1/sbi/device/token/"
#define UPLINK "/v1/sbi/uplink/"

#define DBG(x...) { char _ST_[22] = ""; time_t tt; time(&tt); strftime( _ST_, sizeof( _ST_ ), "%Y-%m-%dT%H:%M:%SZ", gmtime(&tt)); fprintf(stderr, "%s %16s:%04d |%12s(): ", _ST_, __FILE__, __LINE__, __FUNCTION__);} fprintf(stderr, x); fprintf(stderr, "\n")

#define DMC_URL_LENGTH 256
#define DMC_ID_LENGTH 64
#define DMC_STRING_LENGTH 64
typedef struct _restdmc_t {
    char url[DMC_URL_LENGTH];
    char upurl[DMC_URL_LENGTH];
    char upath[DMC_STRING_LENGTH];
    char gwid[DMC_ID_LENGTH];
    char key[DMC_ID_LENGTH];
    char token[DMC_STRING_LENGTH];
    struct _xmppconfig_t xmppconfig;
    int  active;
    int  debug;
} restdmc_t;

struct MemoryStruct {
    char *memory;
    size_t size;
};

static pthread_mutex_t g_mux = PTHREAD_MUTEX_INITIALIZER;

#define GLOBAL_CURL
#ifdef GLOBAL_CURL

#include <unistd.h>
#include <time.h>

#define MY_CURL_EASY_SETOPT( x ) \
    curl_easy_setopt( x, CURLOPT_SSL_VERIFYPEER, 0L ); \
    curl_easy_setopt( x, CURLOPT_SSL_VERIFYHOST, 0L ); \
    curl_easy_setopt( x, CURLOPT_TIMEOUT, 10L ); \

//    curl_easy_setopt( x, CURLOPT_FAILONERROR, 1L );
//    curl_easy_setopt( x, CURLOPT_CAINFO, "conf/ca.pem");
//    curl_easy_setopt( x, CURLOPT_VERBOSE, 0L );

static CURL *g_curl = NULL;
static pthread_t g_pth;
static time_t g_tout;

static void update_timeout()
{
    g_tout = time(NULL);
}

static void _curl_cleanup()
{
    if (g_curl != NULL) {
        DBG("curl_easy_cleanup()");
        curl_easy_cleanup(g_curl);
        g_curl = NULL;
    }
}

static void *curl_timeout(void *ptr)
{
    bool loop = true;
    update_timeout();
    while (loop) {
        usleep(1000000);
        pthread_mutex_lock(&g_mux);
        if (time(NULL) - g_tout >= 15) {
            DBG("_easy_cleanup()");
            _curl_cleanup();
            loop = false;
        }
        pthread_mutex_unlock(&g_mux);
    }
    pthread_detach(pthread_self());
    DBG("pthread_exit(NULL)");
    pthread_exit(NULL);
    return NULL;
}

static CURL * _curl_init()
{
    update_timeout();
    if (g_curl == NULL) {
        DBG("curl_easy_init()");
        pthread_create(&g_pth, NULL, curl_timeout, NULL);
        g_curl = curl_easy_init();
        MY_CURL_EASY_SETOPT(g_curl);
    }
    return g_curl;
}
#endif//GLOBAL_CURL

static void printdmc(void *ins)
{
    restdmc_t *rdmc = (restdmc_t *) ins;
    DBG("url %s", rdmc->url);
    DBG("gwid %s", rdmc->gwid);
    DBG("token %s", rdmc->token);
    DBG("active %d", rdmc->active);
    DBG("upurl %s", rdmc->upurl);
    DBG("xmppconfig server %s", rdmc->xmppconfig.server);
    DBG("xmppconfig host %s", rdmc->xmppconfig.host);
    DBG("xmppconfig port %d", rdmc->xmppconfig.port);
    DBG("xmppconfig username %s", rdmc->xmppconfig.username);
    DBG("xmppconfig password %s", rdmc->xmppconfig.password);
    DBG("xmppconfig presence %s", rdmc->xmppconfig.presence);
}

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *) userp;
    mem->memory = realloc(mem->memory, mem->size + realsize + 1);

    if (mem->memory == NULL) {
        /* out of memory! */
        fprintf(stderr, "not enough memory (realloc returned NULL)\n");
        return 0;
    }
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

static int sslget(void *ins, char *url, struct curl_slist *hdr, struct MemoryStruct *resp)
{
    int code;
    CURL *curl;
    CURLcode res;
    restdmc_t *rdmc = (restdmc_t *) ins;
#ifdef GLOBAL_CURL
    curl = _curl_init();
#else
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
#endif
    resp->memory = malloc(1);
    resp->size = 0;

    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    if (rdmc->debug != 0) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void * ) resp);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdr);
    res = curl_easy_perform(curl);
    /* Check for errors */
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: (%d) %s\n", res, curl_easy_strerror(res));
        code = RESTDM_CODE_SERVICE_UNAVAILABLE;
#ifdef GLOBAL_CURL
        if (res != 7) {
            // (7) Couldn't connect to server
            _curl_cleanup();
        }
#endif
    } else {
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        //DBG("response_code %ld", response_code);
        code = response_code;
    }

#ifndef GLOBAL_CURL
    /* always cleanup */
    curl_easy_cleanup(curl);
    curl_global_cleanup();
#else
    update_timeout();
#endif
    return code;
}

static int sslpost(void *ins, char *url, struct curl_slist *hdr, char *data, struct MemoryStruct *resp)
{
    //struct MemoryStruct resp;
    int code;
    CURL *curl;
    CURLcode res;
    restdmc_t *rdmc = (restdmc_t *) ins;

#ifdef GLOBAL_CURL
    curl = _curl_init();
#else
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
#endif
    resp->memory = malloc(1);
    resp->size = 0;

    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    if (rdmc->debug != 0) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        DBG("data: '%s'\n", data);
    }
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void * ) resp);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdr);
    res = curl_easy_perform(curl);

    /* Check for errors */
    if (res != CURLE_OK) {
        fprintf(stderr, "##### curl_easy_perform() failed: (%d) %s\n", res, curl_easy_strerror(res));
        code = RESTDM_CODE_SERVICE_UNAVAILABLE;
#ifdef GLOBAL_CURL
        if (res != 7) {
            // (7) Couldn't connect to server
            _curl_cleanup();
        }
#endif
    } else {
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        //DBG("response_code %ld", response_code);
        code = response_code;
    }

    if (rdmc->debug != 0) {
        DBG("resp: '%s'\n", resp->memory);
    }
#ifndef GLOBAL_CURL
    /* always cleanup */
    curl_easy_cleanup(curl);
    curl_global_cleanup();
#else
    update_timeout();
#endif
    return code;
}

void *dmc_init(char *protocol, char *server, int port, char *key, char *id, int debug)
{
    restdmc_t *rdmc;
    rdmc = malloc(sizeof(restdmc_t));
    memset(rdmc, 0, sizeof(restdmc_t));
    snprintf(rdmc->url, sizeof(rdmc->url), "%s://%s:%d", protocol, server, port);
    snprintf(rdmc->gwid, sizeof(rdmc->gwid), "%s", id);
    snprintf(rdmc->key, sizeof(rdmc->key), "%s", key);
    rdmc->debug = debug;
    //DBG("URL '%s' ID %s\n", rdmc->url, rdmc->gwid);

    return (void *) rdmc;
}

void dmc_destroy(void *ins)
{
    restdmc_t *rdmc = (restdmc_t *) ins;
    free(rdmc);
}

char *jsonlinestring(char *l)
{
    int i;
    char c;
    for (;;) {
        i = strlen(l);
        c = l[i - 1];
        if (c == ',' || c == ' ' || c == '"' || c == '\t' || c == '\n' || c == '\r') {
            l[i - 1] = '\0';
        } else {
            break;
        }
    }
    for (;;) {
        if (l[0] == ':' || l[0] == ' ' || l[0] == '"') {
            l++;
        } else {
            break;
        }
    }
    return l;
}

int jsonlinebool(char *l)
{
    char *b = NULL;
    b = jsonlinestring(l);
    if (strcmp(b, "true") == 0) {
        return 1;
    } else {
        return 0;
    }
}

void parseconfig(restdmc_t *rdmc, char *cfg)
{
    //DBG("cfg '%s'\n", cfg);
    char delim[] = ",\n";
    char *line = strtok(cfg, delim);
    char *c = NULL;
    int   b = 0;
    while (line) {
        if ((c = strstr(line, "\"xmpp_server\"")) != 0) {
            char *r;
            c = jsonlinestring(c + strlen("\"xmpp_server\""));
            strncpy(rdmc->xmppconfig.server, c, XMPP_STRING_LENGTH);
            r = strchr(c, ':');
            if (r == NULL) {
                strncpy(rdmc->xmppconfig.host, c, XMPP_STRING_LENGTH);
                rdmc->xmppconfig.port = 5222;
            } else {
                strncpy(rdmc->xmppconfig.host, c, r - c);
                rdmc->xmppconfig.port = atoi(r + 1);
            }
        } else if ((c = strstr(line, "\"xmpp_username\"")) != 0 ) {
            c = jsonlinestring(c + strlen("\"xmpp_username\""));
            strncpy(rdmc->xmppconfig.username, c, XMPP_STRING_LENGTH);
        } else if ((c = strstr(line, "\"xmpp_password\"")) != 0 ) {
            c = jsonlinestring(c + strlen("\"xmpp_password\""));
            strncpy(rdmc->xmppconfig.password, c, XMPP_STRING_LENGTH);
        } else if ((c = strstr(line, "\"xmpp_presence\"")) != 0 ) {
            c = jsonlinestring(c + strlen("\"xmpp_presence\""));
            strncpy(rdmc->xmppconfig.presence, c, XMPP_STRING_LENGTH);
        } else if ((c = strstr(line, "\"token\"")) != 0 ) {
            c = jsonlinestring(c + strlen("\"token\""));
            strncpy(rdmc->token, c, DMC_STRING_LENGTH);
        } else if ((c = strstr(line, "\"dm_server\"")) != 0 ) {
            c = jsonlinestring(c + strlen("\"dm_server\""));
            strncpy(rdmc->upurl, c, DMC_URL_LENGTH);
        } else if ((c = strstr(line, "\"dm_datapath\"")) != 0 ) {
            c = jsonlinestring(c + strlen("\"dm_datapath\""));
            strncpy(rdmc->upath, c, DMC_STRING_LENGTH);
        } else if ((c = strstr(line, "\"active\"")) != 0 ) {
            b = jsonlinebool(c + strlen("\"active\""));
            rdmc->active = b;
        } else {
            //DBG("%s", line);
        }
        //DBG("c %s", c);
        line = strtok(NULL, delim);
    }
}

restdm_code dmc_getconfig(void *ins)
{
    restdm_code code;
    char url[256];
    char str[256] = "";
    //char data[64];
    struct MemoryStruct resp;
    restdmc_t *rdmc = (restdmc_t *) ins;

    snprintf(url, sizeof(url), "%s%s", rdmc->url, AGCONF);
    struct curl_slist *hdr = NULL;
    if (strlen(rdmc->key) > 0) {

    }
    snprintf(str, sizeof(str), "x-agent-id: %s", rdmc->gwid);
    hdr = curl_slist_append(hdr, str);
    snprintf(str, sizeof(str), "Authorization: %s", rdmc->key);
    hdr = curl_slist_append(hdr, str);
    hdr = curl_slist_append(hdr, "Accept: application/json");
    hdr = curl_slist_append(hdr, "Content-Type: application/json");

    pthread_mutex_lock(&g_mux);
    code = sslget(ins, url, hdr, &resp);

    parseconfig(rdmc, resp.memory);
    free(resp.memory);
    pthread_mutex_unlock(&g_mux);
    curl_slist_free_all(hdr);

	if (rdmc->debug) {
		printdmc(ins);
	}
    return code;
}

dmc_xmppconfig_t *dmc_getxmppconfig(void *ins)
{
    restdmc_t *rdmc = (restdmc_t *) ins;
    return &(rdmc->xmppconfig);
}

char *getCurrISOFormat()
{
    struct timeval tv;
    char dt[20];
    static char out[32];
    time_t t;
    struct tm *tmp;

    gettimeofday(&tv, NULL);
    t = (time_t) tv.tv_sec;
    tmp = gmtime(&t);
    strftime(dt, sizeof(dt), "%Y-%m-%dT%H:%M:%S", tmp);
    dt[19] = '\0';
    snprintf(out, sizeof(out), "%s.%03dZ", dt, (int) (tv.tv_usec / 1000));
    return out;
}

static char *data2jsonobj(char *data)
{
    static char jsondata[4096];
    snprintf(jsondata, 4096, "{\"etime\":\"%s\",\"data\":%s}", getCurrISOFormat(), data);
    return jsondata;
}

restdm_code dmc_uplink(void *ins, int action, char *data)
{
    restdm_code code;
    restdmc_t *rdmc = (restdmc_t *) ins;

    char url[256];
    char auth[256];
    char act[64];
    struct curl_slist *hdr = NULL;
    struct MemoryStruct resp;

    snprintf(url, sizeof(url), "%s%s", rdmc->upurl, UPLINK);
    hdr = curl_slist_append(hdr, "Accept: application/json");
    hdr = curl_slist_append(hdr, "Content-Type: application/json");
    snprintf(act, sizeof(act), "x-action: %d", action);
    hdr = curl_slist_append(hdr, act);
    snprintf(auth, sizeof(auth), "Authorization: %s", rdmc->token);
    hdr = curl_slist_append(hdr, auth);
    pthread_mutex_lock(&g_mux);
    code = sslpost(ins, url, hdr, data2jsonobj(data), &resp);
    //DBG("resp '%s'\n", resp.memory);
    free(resp.memory);
    pthread_mutex_unlock(&g_mux);
    curl_slist_free_all(hdr);

    return code;
}
