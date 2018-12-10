## SDK developer guide

### API

#### rx_init()
- arguments:
  1. protocol: 'http' or 'https', depend on DM server
  2. host: server hostname or IP address
  3. port: server service port
  4. agid: agent ID, the same with the ID user add in server
  5. flags: enable debug message flags
- returns:
	- instance, when successed
	- Null, when error


#### rx_destroy()
- arguments:
  1. ins: rx instance return from rx_init()


#### rx_uplink()
- arguments:
  1. ins: rx instance return from rx_init()
  2. action:
    - UP_NEWDEVICE
    - UP_DEVDATA
    - UP_DEVEVENT
    - UP_DLRESP
  3. data: data for uplink
- returns:
	- 0, when successed
	- others, when error


#### rx_init_downlink_cb()
- arguments:
  1. ins: rx instance return from rx_init()
  2. cb: function pointer for callback


### Example

#### Downlink Callback function, downlink(data/command) from Server

```c
void dlcb (void *ins, unsigned char *data, int size) {
    printf("%s() data '%s' size %d\n", __FUNCTION__, data, size);
}
```

- ins<br>
rx_init() instance
- data<br>
downlink data
- size<br>
downlink data size

#### RX Agent Create

```c
int main(int argc, char *argv[])
{
	int ins;
	int rc;

	ins = rx_init("https", "cloud02.workssys.com", 8080, "agent_shared_key", "agent_id", 0);
	if (ins == 0) {
		exit(0);
	}
....
```

- "https"<br>
DM Server protocol
- "cloud02.workssys.com"<br>
Hostname or IP of DM server
- 8080<br>
DM Service Port
- "agent_shared_key"<br>
Shared Key for Agent
- "agent_id"<br>
Agent ID
- 0<br>
debug flags

#### Register downlink callback function

```c
....
	rx_init_downlink_cb(ins, dlcb);
....
```

- ins<br>
rx_init() instance
- dlcb<br>
function pointer to callback function

#### New Device(uplink)

```c
....
	char *newdevdata = "{\"id\":\"device_id\",\"type\":\"sensor\"}";
	rc = rx_uplink(ins, UP_NEWDEVICE, newdevdata);
	if (rc != 0) {
		printf("uplink failed\n");
	}
....
```

- ins<br>
rx_init() instance
- UP_NEWDEVICE<br>
POST New Device, Create Device if not exists, or Update Device information if necessary; we recommend call this function for each device when agent startup.
- newdevdata
	- **Mandatory Keys**<br>
**id**: Device ID<br>
**type**: Device Type defined by Project
	- *Optional Keys*<br>
*parent*: Device ID of device's parent

#### Device Data(uplink)

```c
....
	char *devdata = "{\"id\":\"device_id\",\"rt\":\"wks.device.sensor\"}";
	rc = rx_uplink(ins, UP_DEVDATA, devdata);
	if (rc != 0) {
		printf("uplink failed\n");
	}
....
```

- ins<br>
rx_init() instance
- UP_DEVDATA<br>
POST Device Data<br>
- devdata
	- **Mandatory Keys**<br>
**id**: Device ID
	- *Optional Keys*<br>
*rt*: Resource Type defined by Project

#### Device Event(uplink)

```c
....
	chat *devevent = "{\"id\":\"device_id\",\"code\":\"E000123\"}";
	rc = rx_uplink(ins, UP_DEVEVENT, devevent);
	if (rc != 0) {
		printf("uplink failed\n");
	}
....
```

- ins<br>
rx_init() instance
- UP_DEVEVENT<br>
POST Device Event / Alarm / Error
- devevent
	- **Mandatory Keys**<br>
**id**: Device ID<br>
**code**: Code defined by Project

#### Downlink Response(uplink)

```c
....
	char resp = "{\"dl_id\":\"downlink_id\",\"result\":\"success\",\"message\":\"OK\",\"resp\":{\"aa\":\"bb\",\"cc\":1234}}";
	rc = rx_uplink(ins, UP_DLRESP, resp);
	if (rc != 0) {
		printf("uplink failed\n");
	}
....
```

- ins<br>
rx_init() instance
- UP_DLRESP<br>
POST Downlink Response
- resp<br>
	- **Mandatory Keys**<br>
**dl_id**: Downlink ID of downlink data
	- *Optional Keys*<br>
*result*: result of downlink<br>
*message*: message of downlink result<br>
*resp*: response of downlink

#### RX Agent Destroy

```c
....
	rx_destroy(ins);

	return 0;
}
```

- ins<br>
rx_init() instance


