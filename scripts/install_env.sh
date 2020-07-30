#!/bin/bash

install_boost() {
    echo "install boost library from taz.gz"
    wget -O boost_1_73_0.tar.gz https://sourceforge.net/projects/boost/files/boost/1.73.0/boost_1_73_0.tar.gz/download
    tar -xf boost_1_73_0.tar.gz
    cd boost_1_73_0
    sudo ./bootstrap.sh --prefix=/usr/local
    sudo ./b2 link=static runtime-link=shared install
}

install_boost


