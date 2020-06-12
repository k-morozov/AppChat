#! bin/bash

# install boost
BOOST_VER=73
BOOST=boost_1_${BOOST_VER}_0
#wget https://sourceforge.net/projects/boost/files/boost/1.${BOOST_VER}.0/${BOOST}.tar.gz || return 1
wget https://dl.bintray.com/boostorg/release/1.${BOOST_VER}.0/source/boost_1_${BOOST_VER}_0.tar.gz || return 1
tar xzf ${BOOST}.tar.gz && cd ${BOOST}
./bootstrap.sh || return 1
cat <<EOF > ./project-config.jam
using gcc : : ${CXX} ;
EOF
#./b2 install define=_GLIBCXX_USE_CXX11_ABI=0 variant=release link=shared,static threading=multi --layout=tagged -j 2 --prefix=${DIR_INSTALL} --toolset=gcc --without-python || return 1
./b2 install variant=release link=shared,static threading=multi --layout=system -j 2 --prefix=${DIR_INSTALL} --toolset=gcc --without-python || return 1
cd ..

#log4cxx http://www.yolinux.com/TUTORIALS/Log4cxx.html
sudo apt-get install apache2-dev libapr1-dev libaprutil1-dev
tar xzf apache-log4cxx-0.10.0.tar.gz
cd apache-log4cxx-0.10.0/
./configure --prefix=/usr
Typically I would specify /opt or /usr/local but xmlbeansxx (an XML parser I like) which has a dependency on log4cxx and requires an installation in /usr.
make
(will call Maven mvn)
make instal

