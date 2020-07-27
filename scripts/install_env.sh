#!/bin/bash

sudo apt-get update
sudo apt-get install sqlite3 libsqlite3-dev
sudo apt-get install doxygen
sudo apt-get install -qq doxygen graphviz
sudo apt-get install libboost-test-dev
sudo apt install qt5-default

#boost 1.73.0
wget -O boost_1_73_0.tar.gz https://sourceforge.net/projects/boost/files/boost/1.73.0/boost_1_73_0.tar.gz/download
tar -xf boost_1_73_0.tar.gz
cd boost_1_73_0
sudo ./bootstrap.sh --prefix=/usr/local
sudo ./b2 link=static runtime-link=shared install
#end boost

#gtest
sudo apt-get install libgtest-dev -y
sudo chmod 777 /usr/src/gtest
cd /usr/src/gtest
cmake .
make
sudo cp '*.a' /usr/lib/
#cd "${TRAVIS_BUILD_DIR}"
# end gtest
