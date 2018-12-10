#ifndef RESTDMC_H_
#define RESTDMC_H_

#include <stdint.h>

typedef enum {
    RESTDM_CODE_OK      = 200,
    RESTDM_CTEATED      = 201,

    RESTDM_CODE_BADREQUEST      = 400,
    RESTDM_CODE_UNAUTHORIZED    = 401,
    RESTDM_CODE_FORBIDDEN       = 403,
    RESTDM_CODE_NOTFOUND        = 404,

    RESTDM_CODE_INTERNALSERVERERROR     = 500,
    RESTDM_CODE_SERVICE_UNAVAILABLE     = 503
} restdm_code ;

#define XMPP_STRING_LENGTH 64
typedef struct _xmppconfig_t {
    char server[XMPP_STRING_LENGTH];
    char username[XMPP_STRING_LENGTH];
    char password[XMPP_STRING_LENGTH];
    char presence[XMPP_STRING_LENGTH];
    char host[XMPP_STRING_LENGTH];
    uint16_t port;
} dmc_xmppconfig_t;

/**
 * dmc_init()
 *
 * @param protocol ex: http, https
 * @param server restdm hostname
 * @param port restdm service port
 * @param key restdm version path
 * @param id identifier of gateway
 * @param debug enable curl verbose mode
 * @return DM client instance
 */
void *dmc_init(char *protocol, char *server, int port, char *path, char *id, int debug);

/**
 * dmc_destroy()
 *
 * @param instance DM client instance
 */
void dmc_destroy(void *instance);
#if 0
/**
 * dmc_resreg()
 *
 * @param instance DM client instance
 * @param identifier identifier of resource
 * @param resourcetype resource type
 * @return restdm_code
 */
restdm_code dmc_resreg(void *instance, char *identifier, char *resourcetype);

/**
 * dmc_respost()
 *
 * @param instance
 * @param identifier
 * @param data
 * @return restdm_code
 */
restdm_code dmc_respost(void *instance, char *identifier, char *data);
#endif
/**
 * dmc_getconfig()
 *
 * @param instance DM client instance
 * @return restdm_code
 */
restdm_code dmc_getconfig(void *instance);

/**
 * dmc_getxmppconfig()
 *   should call after dmc_getconfig()
 *
 * @param instance DM client instance
 * @return dmc_xmppconfig_t *
 */
dmc_xmppconfig_t *dmc_getxmppconfig(void *instance);
#if 0
/**
 * dmc_gwpost()
 *   POST gateway data (no device management)
 *
 * @param instance
 * @param data
 * @return restdm_code
 */
restdm_code dmc_gwpost(void *instance, unsigned char *data, size_t size);

/**
 * dmc_post()
 *   POST data (no device management)
 *
 * @param instance
 * @param type
 * @param data
 * @return restdm_code
 */
restdm_code dmc_post(void *instance, int type, char *data);

/**
 * dmc_devpost()
 *   POST device data
 *
 * @param instance
 * @param type
 * @param dev_id device ID
 * @param data
 * @return restdm_code
 */
restdm_code dmc_devpost(void *instance, int type, char *dev_id, char *data);
#endif
/**
 * dmc_uplink()
 *   POST uplink data
 *
 * @param instance
 * @param action
 * @param data
 * @return restdm_code
 */
restdm_code dmc_uplink(void *instance, int action, char *data);

#endif//RESTDMC_H_
