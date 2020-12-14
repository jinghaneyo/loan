#!/usr/bin/python3

import os
import socket
import time
import threading
import queue
import concurrent.futures
import asyncio
import multiprocessing 
from pygtail import Pygtail
from loan_pb2 import MsgLog

g_QPacket = queue.Queue()
g_LPacket = threading.Lock()

class Setting_Packet:
    nType = 0
    server_ip = ''
    server_port = 0

    def __init__(self):
        self.type = 0

class Setting_Log:
    nType = 1
    vecLog = []

    def __init__(self):
        self.type = 1

def Push_Packet(data):
    global g_QPacket
    global g_LPacket

    g_LPacket.acquire()
    g_QPacket.put(data)
    g_LPacket.release()

def Pop_Packet():
    global g_QPacket
    global g_LPacket

    packet_list = []
    packet = None
    g_LPacket.acquire()
    try:
        count = 0
        while True:
            packet = g_QPacket.get_nowait()
            if None != packet:
                packet_list.append(packet)
                count = count + 1
            else:
                break
    except:
        pass
        # count = 0
        # packet_list = None

    g_LPacket.release()
    return count, packet_list

def tail(some_file):
    try:
        while True:
            for line in Pygtail(some_file):
                Push_Packet(line)
    except Exception as e:
        print("[Exception][tail] Err = %s" % e)

def Send_Log(server_ip, port):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((server_ip, port))

        while True:
            try:
                count, data = Pop_Packet()
                if 0 < count:
                    for d in data:
                        print(d)
                        #client_socket.send(bytes(d, 'utf-8', 'ignore'))
                        msg = MsgLog()
                        msg.msg_type = 1
                        msg.msg_cmd = 2
                        msg.service_name = "/share/log.txt"
                        msg.LogContents = d
                        #print(msg.SerializeToString())
                        client_socket.send( msg.SerializeToString() )
                else:
                    time.sleep(1)
                    print("POP COUNT = %d" % count)
            except Exception as e:
                print("[Exception][Send_Log][COUNT = %d][Err = %s]" % (count, e) )
    except Exception as e:
        print("[Exception][Send_Log][Err = %s]" % e )

def Thread_work(ini):
    if 0 == ini.type:
        Send_Log( ini.server_ip, ini.server_port )
    elif 1 == ini.type:
        while True:
            try:
                tail(ini.vecLog[0])
            except Exception as e:
                print("[Exception][Thread_work] tail Err = %s" % e)

            time.sleep(1)

if __name__ == '__main__':
    ini = []

    packet = Setting_Packet()
    packet.server_port = 3333
    packet.server_ip = "172.17.0.2"

    log = Setting_Log()
    log.vecLog.append("/share/engine/log.txt")
    #log.vecLog.append(r"D:\10_Open_Source\loan\engine\eng\log.txt")

    ini.append(log)   # 로그 tail 스레드
    ini.append(packet)   # 로그 tcp 전송 스레드
    
    t1 = threading.Thread(target=Thread_work, args=(ini[0], ))
    t1.start()
    t2 = threading.Thread(target=Thread_work, args=(ini[1], ))
    t2.start()
    # with concurrent.futures.ThreadPoolExecutor(max_workers=2) as executor:
    #     executor.map(Thread_work, ini)

    while True:
        time.sleep(0.01)
