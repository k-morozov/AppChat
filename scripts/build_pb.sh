#!/bin/bash

build_pb_msgs() {
	echo "build protobuf messages"
	protoc --version
	cd ../src/protocol/include/protocol/proto/
	rm -r build/
	mkdir build/
	protoc -I . --cpp_out ./build/ request.proto response.proto messages.proto	
}

build_pb_msgs
