# Build RX SDK
## dependent libraries
- XMPP Client: libstrophe<br>
  build from source
  - libstrophe.so
- Openssl<br>
  build from source, or package install
  - libssl.so
  - libcrypto.so
- XML Parser: expat<br>
  build from source, or package install
  - libexpat.so
- HTTP Client: curl<br>
  build from source, or package install
  - libcurl.so

for arm platform, Please Ref. build/README.4.arm.md

## files

### sdk source / header file
- ./src/restdmc.c
- ./src/rxagent.c
- ./src/xmpp_chat.c
- ./src/xmpp_common.c
- ./src/xmppdmc.c
- ./src/xmpp_helper.c
- ./src/xmpp_types.h
- ./src/restdmc.h
- ./src/rx_common.h
- ./src/xmpp_chat.h
- ./src/xmpp_common.h
- ./src/xmppdmc.h
- ./src/xmpp_helper.h
- ./inc/rxagent.h

### build library
- ./build/Makefile

### example code
- ./examples/main.c
- ./examples/Makefile

## build librx.a

```
cd build
make
```

## build agent

```
cd examples
make
```

