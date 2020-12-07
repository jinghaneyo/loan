#!/usr/bin/python3

import socket
import time

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#client_socket.connect(("172.17.0.2", 3333))
#client_socket.connect(("172.17.0.4", 4444))
client_socket.connect(("172.17.0.5", 5555))
#client_socket.connect(("172.17.0.5", 4444))
while 1:
    send_data = b"[{'cmd':'send'}, {'ask':'start?'}]"
    client_socket.send(send_data)

    recv_data = client_socket.recv(512).decode()
    print ("RECEIVED => " , recv_data)

    time.sleep(1)

    client_socket.send(send_data)

    recv_data = client_socket.recv(512).decode()
    print ("RECEIVED => " , recv_data)

    time.sleep(2)
    break

print ("socket colsed... END.")
