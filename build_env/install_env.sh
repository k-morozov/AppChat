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

#log4cplus https://sourceforge.net/projects/log4cplus/

#gtest
sudo apt-get install libgtest-dev
sudo apt-get install cmake # install cmake
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /usr/lib
sudo ln -s /usr/lib/libgtest.a /usr/local/lib/gtest/libgtest.a
sudo ln -s /usr/lib/libgtest_main.a /usr/local/lib/gtest/libgtest_main.a

#sqlite
sudo apt-get install sqlite3 libsqlite3-dev
