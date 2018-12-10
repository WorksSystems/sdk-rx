/**
 * @file rxagent.h
 * @brief Works System IoT Agent SDK
 *
 * @author Works Systems, Inc.
 */
#ifndef _RXAGENT_H_
#define _RXAGENT_H_

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @def RX_FLAGS_CURL_DEBUG
 *
 * Enable CRUL Debug Messages
 */
#define RX_FLAGS_CURL_DEBUG (1 << 1)
/**
 * @def RX_FLAGS_STROPHE_DEBUG
 *
 * Enable STROPHE Debug Messages
 */
#define RX_FLAGS_STROPHE_DEBUG (1 << 2)

/**
 * @def UP_NEWDEVICE
 * UPLINK NEW DEVICE
 *
 * data fields required, "id", "type",<br>
 * ex: { "id" : "deviceid" , "type" :"devicetype" }
*/
#define UP_NEWDEVICE    1

/**
 * @def UP_DEVDATA
 * UPLINK DEVICE Data
 *
 * data fields required, "id", "rt",<br>
 * ex: { "id" : "deviceid" , "rt" :"wks.device" }
*/
#define UP_DEVDATA      2

/**
 * @def UP_DEVEVENT
 * UPLINK DEVICE Event
 *
 * data fields required, "id", "code",<br>
 * ex: { "id" : "deviceid" , "code" :"1234" }
*/
#define UP_DEVEVENT     3

/**
 * @def UP_DLRESP
 * UPLINK Downlink Response
 *
 * data fields required, "dl_id", "result", "message",<br>
 * ex: { "dl_id" : "downlinkid" , "result" : "success" , "message" : "downlink completed"}
*/
#define UP_DLRESP       9

#ifdef BUILD_WIN_DLL
#define WIN_DLL __declspec(dllexport)
#else
#define WIN_DLL
#endif

/**
 * @brief north bound downlink message callback prototype
 *
 * @param ins instance of rxagent
 * @param msg the message from north bound
 * @param size the size of message
 */
typedef void (*rx_downlink_cb_t)(void *ins, unsigned char *msg, int size);

/**
 * @brief rxagent initialize
 *
 * @param protocol ex: http, https
 * @param host hostname(ip) of IoT DM server
 * @param port service port of IoT DM server
 * @param gwid Gateway Identification
 * @param key  auth key
 * @param flags RX_FLAGS
 *
 * @return instance instance of rxagent, null for error
 */
WIN_DLL void *rx_init(char *protocol, char *host, unsigned short port, char *gwid, char *key, int flags);

/**
 * @brief rxagent destroy
 *
 * @param ins instance of rxagent
 */
WIN_DLL void rx_destroy(void *ins);

/**
 * @brief uplink message
 *
 * @param ins instance of rxagent
 * @param action uplink action
 * @param data uplink data
 *
 * @return code 0 success, others for error
 */
WIN_DLL int rx_uplink(void *ins, int action , char *data);

/**
 * @brief set downlink callback function
 *
 * @param ins instance of rxagent
 * @param cb callback function pointer
 *
 * @return 0 success
 */
WIN_DLL int rx_init_downlink_cb(void *ins, rx_downlink_cb_t cb);

#ifdef __cplusplus
}
#endif

#endif//_RXAGENT_H_
