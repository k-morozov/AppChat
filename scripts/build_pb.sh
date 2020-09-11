#!/bin/bash

build_pb_msgs() {
	echo "build protobuf messages"
	protoc --version
	pwd
	cd ../src/protocol/include/protocol/proto/
	echo "dir after cd"
	pwd
	rm -r build/
	mkdir build/
	ls
	protoc -I . --cpp_out ./build/ request.proto response.proto messages.proto	
}

build_pb_msgs
