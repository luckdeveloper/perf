#!/usr/bin/python

import socket

HOST = ''  
PORT = 65432        # Port to listen on (non-privileged ports are > 1023)

def main(argv=None):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(socket.gethostname(), PORT)
    s.listen()

    conn, addr = s.accept()
    print('Connected by', addr)
    while True:
        data = conn.recv(1024)
        if not data:
            break
        conn.sendall(data)

if __name__ == "__main__":
    main()
