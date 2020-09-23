#!/bin/bash

build_pb_msgs() {
	echo "Build protobuf messages"
	protoc --version
	PATH_PB=./src/protocol/include/protocol/proto
	rm -r $PATH_PB/build/
	mkdir $PATH_PB/build/
	protoc -I $PATH_PB --cpp_out=$PATH_PB/build request.proto	
	protoc -I $PATH_PB --cpp_out=$PATH_PB/build response.proto
	protoc -I $PATH_PB --cpp_out=$PATH_PB/build messages.proto

	TEST_PATH_PB=./src/py_tests
	protoc -I $PATH_PB --python_out=$TEST_PATH_PB request.proto	
	protoc -I $PATH_PB --python_out=$TEST_PATH_PB response.proto
	protoc -I $PATH_PB --python_out=$TEST_PATH_PB messages.proto
}

build_pb_msgs
