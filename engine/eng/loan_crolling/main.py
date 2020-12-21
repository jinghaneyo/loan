#!/usr/bin/python3

import os
import sys
import socket
import time
import threading
import queue
import concurrent.futures
import asyncio
import multiprocessing 
import configparser
import json
from pygtail import Pygtail
from loan_pb2 import MsgLog

g_bWindows = None

g_QPacket = queue.Queue()
g_LPacket = threading.Lock()

class MsgLog_Type:
    CROLLING = 1
    ANALYZER = 2
    MAX = 3

class MsgLog_Cmd_Crolling:
    SENDLING = 1
    STOP_REQU = 2
    STOP_RESP = 3
    START_REQU = 4
    START_RESP = 5
    MAX = 6

class Conf_ini:
    server_ip = ''
    server_port = 0

    def __init__(self):
        pass

class Service_Log:
    service_name = ''
    service_path = ''
    file_start = 0  # 0 -> 처음부터 읽기, 1 -> 마지막부터 시작 

class Log_Packet:
    service_name = ''
    service_path = ''
    log_contents = ''

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

def Check_OS():
    global g_bWindows
    if "win32" == sys.platform or "win64" == sys.platform:
        g_bWindows = True
    else :
        g_bWindows = False

    return g_bWindows

def GetCurrentPath():
    global g_bWindows
    curr_dir = os.getcwd()

    if None == g_bWindows:
        Check_OS()

    if False == os.path.isdir(curr_dir):
        if True == g_bWindows:
            pos = curr_dir.rfind('\\')
        else:
            pos = curr_dir.rfind('/')

        curr_dir = curr_dir[:pos]

    # 마지막은 항상 \\, / 로 끝난다
    if True == g_bWindows:
        if '\\' != curr_dir[len(curr_dir)-1]:
            curr_dir = curr_dir + '\\'
    else:
        if '/' != curr_dir[len(curr_dir)-1]:
            curr_dir = curr_dir + '/'

    return curr_dir

def Load_Config(path_conf):
    conf = Conf_ini()
    config = configparser.ConfigParser()
    cnt = 0

    list_encoding = ['utf-8-sig','utf-8','euc-kr']
    for enc in list_encoding:
        try:
            cnt = config.read(path_conf, encoding=enc)    
            break
        except:
            cnt = -1

    if -1 == cnt:
        return None

    try:
        # conf.ini 설정 로드 
        if 0 == len(cnt):
            print('[Load_Config]Read Fail = %s' % path_conf )
            return None
        else:
            print('[COMMON] Read SUCC = %s' % path_conf )

            conf.server_ip = config['COMMON']['SERVER_IP']
            print('[COMMON] SERVER_IP = %s' % conf.server_ip)

            conf.server_port = int(config['COMMON']['SERVER_PORT'])
            print('[COMMON] SERVER_PORT = %d' % conf.server_port)

            return conf
    except Exception as e:
        print('[Exception][Load_Config][ERR = %s]' % e )
        return None

def Load_ServiceLog(path_json):
    log_list = []
    with open(path_json, "r") as json_file:
        conf = json.load(json_file)
        for service_list in conf:
            for s_key in service_list:
                log = Service_Log()
                log.service_name = s_key
                for l_obj in service_list[s_key]:
                    for l_key in l_obj:
                        if 'log' == l_key:
                            log.service_path = l_obj[l_key]
                        elif 'file_start' == l_key:
                            if 'first' == l_obj[l_key]:
                                log.file_start = 0
                            elif 'last' == l_obj[l_key]:
                                log.file_start = 1

                log_list.append(log)

    return log_list

def tail(log):
    try:
        while True:
            for line in Pygtail(log.service_path):
                data = Log_Packet()
                data.service_name = log.service_name
                data.service_path = log.service_path
                data.log_contents = line
                Push_Packet(data)
                print("%s\n" % line)
    except Exception as e:
        print("[Exception][tail] Err = %s" % e)

def Thread_Send_Log(server_ip, port):
    while True:
        try:
            # 접속실패 시 exception 이다 
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket.connect((server_ip, port))

            while True:
                try:
                    count, data = Pop_Packet()
                    if 0 < count:
                        for d in data:
                            print(d.log_contents)
                            msg = MsgLog()
                            msg.msg_type = 1 #MsgLog_Type.CROLLING
                            msg.msg_cmd = 1 #MsgLog_Cmd_Crolling.SENDLING
                            msg.service_name = d.service_name
                            msg.LogContents = d.log_contents
                            client_socket.send( msg.SerializeToString() )
                    else:
                        time.sleep(1)
                        print("POP COUNT = %d" % count)
                except Exception as e:
                    print("[Exception][Send_Log][COUNT = %d][Err = %s]" % (count, e) )

        except Exception as e:
            print("[Exception][FAIL][connect][Err = %s]" % e )
            time.sleep(5)

def Thread_Tail_Log(log):
    while True:
        try:
            tail(log)
        except Exception as e:
            print("[Exception][Thread_work] tail Err = %s" % e)

        time.sleep(1)

if __name__ == '__main__':
    strCurpath = GetCurrentPath()
    path_config = strCurpath + "conf.ini"
    path_json = strCurpath + "log.json"

    conf = Load_Config(path_config)
    log_list = Load_ServiceLog(path_json)
    
    # 설정 정보 출력
    if None == conf:
        print("[FAIL] Read config.ini => %s" % path_config)
        exit(1)
    if None == log_list or 0 == len(log_list):
        print("[FAIL] Read service.json => %s" % path_json)
        exit(1)

    print("START INFO >>> SERVER IP   = [%s]" % conf.server_ip)
    print("START INFO >>> SERVER PORT = [%d]" % conf.server_port)
    for log in log_list:
        print("START INFO >>> LOG NAME = [%s]" % log.service_name)
        print("START INFO >>> LOG PATH = [%s]" % log.service_path)
        if 0 == log.file_start:
            print("START INFO >>> LOG FILE = [%s]" % "first")
        elif 1 == log.file_start:
            print("START INFO >>> LOG FILE = [%s]" % "last")

    for log in log_list:
        t = threading.Thread(target=Thread_Tail_Log, args=(log,))
        t.start()

    t2 = threading.Thread(target=Thread_Send_Log, args=(conf.server_ip, conf.server_port))
    t2.start()

    while True:
        time.sleep(0.01)
