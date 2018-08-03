#!/usr/bin/python
#
# Quick script to open a binary file and stream the
# bytes in it to a TCP server socket.  Meant to be used
# to send bytes to COSMOS.
#
# Python socket programming tutorial:  https://pymotw.com/2/socket/tcp.html
# Reading binary file in Python:  http://stackoverflow.com/questions/1035340/reading-binary-file-in-python-and-looping-over-each-byte
#

import socket
import sys
import threading
import time
import getopt

def main():
	try:
		opts, args = getopt.getopt(sys.argv[1:], "c:t:f:", ["cmd_port=", "tlm_port=", "file="])
		try:
			print >>sys.stderr, 'Starting stream_bytes.py'
			cmd_port = 30102
			tlm_port = 30100
			infile = "HighFifo_Example1_Cadet_EPS.bin"
			for o, a in opts:
				if o in ("-c", "--cmd_port"):
					cmd_port = int(a)
				elif o in ("-t", "--tlm_port"):
					tlm_port = int(a)
				elif o in ("-f", "--file"):
					infile = a
				else:
					assert False, "Unhandled option"
			cs = CmdSocketThread(cmd_port)
			cs.start()
			fs = TlmFileStreamerThread(tlm_port, infile)
			fs.start()
		except KeyboardInterrupt:
			print >>sys.stderr, ''
			print >>sys.stderr, 'Keyboard interrupt... stopping stream_bytes.py'
	except getopt.GetoptError as err:
		print >>sys.stderr, str(err)
		usage()
		sys.exit(2)

def usage():
	print >>sys.stderr, 'stream_bytes.py [-c cmd_port] [-t tlm_port] [-f file]'

class CmdSocketThread(threading.Thread):
	def __init__(self, cmd_port):
		threading.Thread.__init__(self)
		self._cmdport = cmd_port 
		# Create a TCP/IP socket
		self._sockcmd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self._sockcmd.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		# Bind the socket to the port
		server_address = ('localhost', self._cmdport)
		print >>sys.stderr, 'Starting up cmd server on %s port %s' % server_address
		self._sockcmd.bind(server_address)

	def run(self):
		# Listen for incoming connection
		self._sockcmd.listen(1)
		
		# Wait for a connection
		self._connection, client_address = self._sockcmd.accept()

	def __del__(self):
		print >>sys.stderr, 'Closing cmd server socket'
		self._connection.close()
		self._sockcmd.close()

class TlmFileStreamerThread(threading.Thread):
	def __init__(self, tlm_port, infile):
		threading.Thread.__init__(self)
		self._file = infile
		self._tlmport = tlm_port
		# Create a TCP/IP socket
		self._socktlm = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self._socktlm.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		# Bind the socket to the port
		server_address = ('localhost', self._tlmport)
		print >>sys.stderr, 'Starting up tlm server on %s port %s' % server_address
		self._socktlm.bind(server_address)

	def run(self):
		# Listen for incoming connection
		self._socktlm.listen(1)
		
		# Wait for a connection
		print >>sys.stderr, 'Waiting for a tlm connection'
		connection, client_address = self._socktlm.accept()
		self.stream_file_to_client(connection, client_address)
		connection.close()

	def stream_file_to_client(self, connection, client_address):
		try:
			print >>sys.stderr, 'Tlm connection from', client_address
			with open(self._file, "rb") as f:
				print >>sys.stderr, 'Opened file %s to read' % self._file
				while True:
					byte = f.read(3600)
					if not byte:
						print >>sys.stderr, 'Done reading'
						break
					# Do stuff with byte.
					#print >>sys.stderr, 'Read byte: '.format(byte, '02x')
					connection.send(byte)

		finally:
			# Clean up the connection
			connection.close()

	def __del__(self):
		print >>sys.stderr, 'Closing tlm server socket'
		self._socktlm.close()

main()


