#!/bin/bash

build_pb_msgs() {
	echo "Build protobuf messages"
	protoc --version
	PATH_PB=../src/protocol/include/protocol/proto
	rm -r $PATH_PB/build/
	mkdir $PATH_PB/build/
	ls
	protoc -I $PATH_PB --cpp_out=$PATH_PB/build request.proto	
	protoc -I $PATH_PB --cpp_out=$PATH_PB/build response.proto
	protoc -I $PATH_PB --cpp_out=$PATH_PB/build messages.proto
}

build_pb_msgs
