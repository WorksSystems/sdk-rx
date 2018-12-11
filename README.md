
### install tools / libraries

#### CentOS 6.8

```
yum install -y git wget
yum install -y libtool pkgconfig
yum install -y expat-devel openssl-devel curl-devel
```

#### Ubuntu 16.04

```
apt-get install -y git wget
apt-get install -y libtool pkg-config
apt-get install -y libexpat1-dev libssl-dev libcurl4-openssl-dev
```

### build libstrophe

```
wget https://github.com/strophe/libstrophe/releases/download/0.9.1/libstrophe-0.9.1.tar.bz2
tar jxvf libstrophe-0.9.1.tar.bz2
cd libstrophe-0.9.1
./configure
make install
```

### build / run rxagent

Build Agent

```
git clone https://github.com/WorksSystems/sdk-rx.git
cd sdk-rx/build
make
cd sdk-rx/examples
make
```

Run Agent

```
./rxtest iop-agent
```
