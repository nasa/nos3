import sys
import csv
import getopt 
import subprocess 
import shlex
import re
import glob
import pickle

from bs4 import BeautifulSoup 
from HTMLParser import HTMLParser
from struct import *

class CommandParser(HTMLParser):

	#
	# Initializes allData variable
	#
	def reset(self):
		self.allData = []
		self.allhref = []
		HTMLParser.reset(self)

	#
	# Appends HTML file content to allData variable
	#
	def handle_data(self, text):
		if text.strip() != '': # excludes new lines
			self.allData.append(text.strip())
			# print self.allData[-1]

	#
	# Appends href tag contents to hrefTags variable
	#
	def handle_starttag(self, tag, attrs):
		if tag == 'a':
			for name, value in attrs:
				if name == 'href':
					val = re.split('#', value)[0]
					self.allhref.append(val)
					# print self.allhref[-1]



if __name__ == '__main__':

	# 
	# Searches for and reads HTML files
	#
	file_list = glob.glob('../../docs/cFE UsersGuide/Doxygen/cfe__*msg_8h.html')
	
	for html_file in file_list:
		with open(html_file) as html_obj:

			# initializes variables
			print '\n', 'FILE:', html_file
			reader = html_obj.read()
			soup = BeautifulSoup(reader)
			cmdNames = [] # names of commands
			cmdCodes = [] # command codes
			htmlFiles = [] # HTML files with parameter information

			# gets HTML file names
			for link in soup.findAll(text="Command Structure"):
				htmlFile = link.find_next('a')['href'] # next element with 'a' tag
				htmlFile = re.split('\.', htmlFile)[0]
				htmlFiles.append(htmlFile.encode('ascii'))

			# gets command names and command codes
			for names in soup.findAll(text="Name:"): # finds all 'Name:' text
				pre_cmdCode = names.find_previous('td').get_text()
				cmdCode = pre_cmdCode.split()[-1]
				cmdCodes.append(cmdCode.encode('ascii'))
				cmdName = names.next_element.get_text() # finds next text element
				cmdNames.append(cmdName.encode('ascii'))

			# prints values after iterating through whole file
			print 'CMD NAMES:', cmdNames
			print 'CMD CODES:', cmdCodes
			print 'HTML FILES:', htmlFiles

			# writes data to pickle file
			pickle_file = 'CommandFiles/' + re.split('/|\.', html_file)[-2]
			with open(pickle_file, 'wb') as pickle_obj:
				pickle.dump([cmdNames, cmdCodes, htmlFiles], pickle_obj)


