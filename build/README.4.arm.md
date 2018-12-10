Build expat
```
# cd sdk-rx
# mkdir cross-compile-arm
# cd cross-compile-arm
# curl -kO https://nchc.dl.sourceforge.net/project/expat/expat/2.2.4/expat-2.2.4.tar.bz2
# tar jxvf expat-2.2.4.tar.bz2
# cd expat-2.2.4/
# ./configure --host=arm-linux-gnueabihf --prefix=$PWD/../expat
# make install
```

Build OpenSSL
```
# cd sdk-rx
# mkdir cross-compile-arm
# cd cross-compile-arm
# curl -kO https://ftp.openssl.org/source/old/1.0.2/openssl-1.0.2l.tar.gz
# tar zxvf openssl-1.0.2l.tar.gz
# cd openssl-1.0.2l
# ./Configure linux-generic32 shared --cross-compile-prefix=arm-linux-gnueabihf- --prefix=$PWD/../openssl
# make build_libs build_apps install_sw
```

Build libz
```
# cd sdk-rx
# mkdir cross-compile-arm
# cd cross-compile-arm
# wget http://www.zlib.net/zlib-1.2.11.tar.gz
# tar zxvf zlib-1.2.11.tar.gz
# cd zlib-1.2.11/
# export CHOST=arm-linux-gnueabihf ; ./configure --prefix=$PWD/../zlib
# make install
```

Build curl
```
# cd sdk-rx
# mkdir cross-compile-arm
# cd cross-compile-arm
# curl -LkO -w %{url_effective} https://github.com/curl/curl/releases/download/curl-7_54_0/curl-7.54.0.tar.gz
# tar zxvf curl-7.54.0.tar.gz
# cd curl-7.54.0
# ./configure --host=arm-linux-gnueabihf --with-zlib=$PWD/../zlib --with-ssl=$PWD/../openssl --prefix=$PWD/../curl --disable-ldap --disable--ldaps
# make install
```

Build libstrophe
```
# cd sdk-rx
# mkdir cross-compile-arm
# cd cross-compile-arm
# curl -LkO -w %{url_effective}  https://github.com/strophe/libstrophe/archive/0.9.1.tar.gz
# tar zxvf 0.9.1.tar.gz
# cd libstrophe-0.9.1
# ./bootstrap.sh
# vim configure (skip res_query() -lresolv check)
# export PKG_CONFIG_PATH=$PWD/../openssl/lib/pkgconfig:$PWD/../expat/lib/pkgconfig; ./configure --host=arm-linux-gnueabihf --prefix=$PWD/../libstrophe
#  make install-exec install-data
```

Build json-c
```
# cd sdk-rx
# mkdir cross-compile-arm
# cd cross-compile-arm
# wget https://github.com/json-c/json-c/archive/json-c-0.13-20171207.tar.gz
# tar zxvf json-c-0.13-20171207.tar.gz
# cd json-c-json-c-0.13-20171207/
# sh autogen.sh
# ./configure  --host=arm-linux-gnueabihf --prefix=$PWD/../json-c
# make
# make install
```

Build RX librx.a for agent
```
# cd build
# export PKG_CONFIG_PATH=$PWD/../cross-compile-arm/openssl/lib/pkgconfig:$PWD/../cross-compile-arm/expat/lib/pkgconfig:$PWD/../cross-compile-arm/libstrophe/lib/pkgconfig:$PWD/../cross-compile-arm/curl/lib/pkgconfig:$PWD/../cross-compile-arm/json-c/lib/pkgconfig ;
# export CROSS_COMPILE=arm-linux-gnueabihf- ;
# make
```

