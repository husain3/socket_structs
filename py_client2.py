#!/usr/bin/env python

""" client.py - Echo client for sending/receiving C-like structs via socket
References:
- Ctypes fundamental data types: https://docs.python.org/2/library/ctypes.html#ctypes-fundamental-data-types-2
- Ctypes structures: https://docs.python.org/2/library/ctypes.html#structures-and-unions
- Sockets: https://docs.python.org/2/howto/sockets.html
"""

import socket
import sys
import random
import struct
from ctypes import *


def main():
	server_addr = ('localhost', 27151)
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	if s < 0:
		print "Error creating socket"

	try:
		s.connect(server_addr)
		print "Connected to %s" % repr(server_addr)
	except:
		print "ERROR: Connection to %s refused" % repr(server_addr)
		sys.exit(1)

	long_var = struct.pack("<l", 5500)
	s.sendall(long_var)

	try:
		# for i in range(5):
		#     print ""
			
		PackingString = "<"; PackingList = []
		PackingString += "l"; PackingList.append(int(2))
		PackingString += "l"; PackingList.append(int(7))
		PackingString += "f"; PackingList.append(float(2.5))
		PackingString += "l"; PackingList.append(int(5))
		ACTI_CarInfo_test = struct.pack(PackingString, *PackingList)

		#     nsent = s.sendall(ACTI_CarInfo_test)

		#     print "----------------------"
		#     buff = s.recv(16)
		#     rec_code = struct.unpack(PackingString, buff)
		#     print (rec_code[0])
		#     print (rec_code[1])
		#     print (rec_code[2])
		#     print (rec_code[3])
		print("Done looping")

	finally:
		print "Closing socket"
		s.close()


if __name__ == "__main__":
	main()