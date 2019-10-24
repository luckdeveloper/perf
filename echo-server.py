#!/usr/bin/python

import socket
import sys
import traceback

HOST = ''
PORT = 65432        # Port to listen on (non-privileged ports are > 1023)

def main(argv=None):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind((socket.gethostname(), PORT))
        print("echo server is listen on port ",PORT)
        s.listen(5)

        conn, addr = s.accept()
        print('Connected by ', addr)
        while True:
            data = conn.recv(1024)
            if not data:
                break
            print("recv message: ", data)
            conn.sendall(data)

    except Exception, err:
        print err.message
        traceback.print_exc()

if __name__ == "__main__":
    main()
