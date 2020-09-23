FROM ubuntu:latest
MAINTAINER Konstantin Morozov <morozov-kst@yandex.ru>
ENV DEBIAN_FRONTEND=noninteractive 
RUN apt-get update && \
    apt-get install -y \
	build-essential make autoconf cmake -y \
      	libboost-all-dev -f \
	libprotobuf-dev -f protobuf-compiler \
	qt5-default \
	libsqlite3-dev -y

