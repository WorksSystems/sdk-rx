# sdk-rx

restful xmpp agent sdk

git clone
```sh
git clone https://github.com/WorksSystems/sdk-rx.git
```

## Build with CentOS 6.8

1. install CentOS 6.8 with option ```Software Development Workstation```

1. Upgrade git version to 2.2.1 or higher

1. git clone project and build libstrophe.so  https://github.com/strophe/libstrophe

1. git clone https://github.com/WorksSystems/sdk-rx.git

1. For doxygen, need version after 1.8.11, CentOS only provide 1.6.x, so please build it from [source code ](https://sourceforge.net/projects/doxygen/files/rel-1.8.12/), download doxygen-1.8.12.src.tar.gz
	1. Need to remove old version `yum remove doxygen`
	1. For dot command not find, `yum install graphviz`
	1. need cmake `yum install cmake`
1. pack the sdk-rx library with ```./pack.sh```, you will get sdk-rx.tgz.

## Release SDK
1. Beside sdk-rx.tgz, we should provide a test server, a test user account and a test gateway. Use RESTful API to create a user and a gateway on server.
2. IoTAgent_Porting_Guide.ppt
3. A document for the context of uplink and downlink.
