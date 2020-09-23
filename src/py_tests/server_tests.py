#!/usr/bin/python 

import sys
import socket
import messages_pb2
import unittest
import threading

def make_socket(port=7777):
    sockobj = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sockobj.connect(('localhost', port))
    return sockobj

def connect(port=7777):
    if len(sys.argv) >= 2:
        port = int(sys.argv[1])
    return make_socket(7777)

def socket_read_n(sock, n):
    buf = b''
    while n > 0:
        data = sock.recv(n)
        if data == '':
            raise RuntimeError('unexpected connection close')
        buf += data
        n -= len(data)
    return buf

def send_message(sock, message, command_id):
    msg = message.SerializeToString()
    header = messages_pb2.Header()
    header.length = len(msg)
    header.command = command_id;
    header.version = 2;

    sock.send(header.SerializeToString())
    sock.send(message.SerializeToString())

def get_response(sock):
    len_prefix = messages_pb2.Header()
    len_buf = socket_read_n(sock, 19)
    len_prefix.ParseFromString(len_buf)
    msg_buf = socket_read_n(sock, len_prefix.length)

    msg = messages_pb2.Response()
    msg.ParseFromString(msg_buf)
   # print("response command_id = {}".format(len_prefix.command))
    return msg

def register_request(sock, name, passw):
    register_req = messages_pb2.RegRequest()
    register_req.login = name
    register_req.password = passw

    request = messages_pb2.Request()
    request.register_request.MergeFrom(register_req)

    send_message(sock, request, command_id=1)
    return get_response(sock)

def input_request(sock, name, passw):
    in_req = messages_pb2.InRequest()
    in_req.login = name
    in_req.password = passw

    request = messages_pb2.Request()
    request.input_request.MergeFrom(in_req)

    send_message(sock, request, command_id=3)
    return get_response(sock)

def join_channel_request(sock, room_id):
    join_req = messages_pb2.JoinRoomRequest()
    join_req.room_id = room_id

    request = messages_pb2.Request()
    request.join_room_request.MergeFrom(join_req)

    send_message(sock, request, command_id=7)
    return get_response(sock)

class TestServerReg(unittest.TestCase):
    def setUp(self):
        self.sockobj = make_socket()

    def tearDown(self):
        self.sockobj.close()

    def test_reg(self):
        msg = register_request(sock=self.sockobj, name='AAA', passw='123')
        self.assertEqual(msg.reg_response.status, 1)


class TestServerInput(unittest.TestCase):
    def setUp(self):
        self.sockobj = make_socket()

    def tearDown(self):
        self.sockobj.close()

    def test_input(self):
        name = 'AAA'
        passw = '123'

        msg = input_request(sock=self.sockobj, name='AAA', passw='123')
        self.assertEqual(msg.input_response.status, 1)

    def test_join(self):
        name = 'AAA'
        passw = '123'

        msg = input_request(sock=self.sockobj, name='AAA', passw='123')
        self.assertEqual(msg.input_response.status, 1)

        channel_id = 123
        #msg = join_channel_request(sock=self.sockobj, room_id = channel_id)
        #self.assertEqual(msg.join_room_response.status, 1)
        #self.assertEqual(msg.join_room_response.room_id, channel_id)
if __name__ == '__main__':
    #unittest.main()
    pass


