import sys
import csv
import getopt 
import subprocess 
import shlex
import re
import glob
import pickle

from HTMLParser import HTMLParser
from struct import *

class HTMLDocsParser(HTMLParser):

	#
	# Initializes allData variable
	#
	def reset(self):
		self.allData = []
		HTMLParser.reset(self)

	#
	# Appends HTML file content to allData variable
	#
	def handle_data(self, text):
		if text.strip() != '': # excludes new lines
			self.allData.append(text.strip())

	#
	# Determines UNIX data type of parameter
	#
	def findDataTypeNew(self, dataTypeOrig, paramName, paramLen, paramDesc):
		if paramLen != '': # assumes all string types have length enclosed in brackets
			return '--string'
		elif dataTypeOrig == 'uint8' or dataTypeOrig == 'boolean':
			return '--byte'
		elif dataTypeOrig == 'uint16':
			return '--half'
		elif dataTypeOrig == 'uint32':
			return '--word'
		elif dataTypeOrig == 'uint64':
			return '--double'
		else:
			return ''

	#
	# Determines character array size for string types
	#
	def findStringLen(self, keyword):
		hdr_files = glob.glob('../../../build/cpu1/inc/*.h')
		hdr_files += glob.glob('../../fsw/cfe-core/src/inc/cfe_*.h')
		hdr_files += glob.glob('../../fsw/mission_inc/cfe_mission_cfg.h')
		val = ''
		found = False
		i = 0

		while found == False and i < len(hdr_files):
			with open(hdr_files[i], 'r') as hdr_obj:
				file_lines = hdr_obj.readlines()
				j = 0
				while found == False and j < len(file_lines):
					if '#define '+keyword in file_lines[j]:
						found = True
						val = file_lines[j].split()[2]
					j += 1
			i += 1

		return val


if __name__ == '__main__':

	#
	# Initializes HTMLDocsParser class
	#
	parser = HTMLDocsParser()

	#
	# Searches for and reads in HTML files
	#
	file_list = glob.glob('../../docs/cFE UsersGuide/Doxygen/struct_c_f_e__*.html')
	
	for html_file in file_list: # loops through HTML files
		with open(html_file) as file_obj: # opens HTML file
			reader = file_obj.read() # reads HTML file
			parser.feed(reader) # feeds file contents to parser
			data = parser.allData

			dataTypesOrig = [] # uint8, uint16, uint32, char, boolean
			paramNames = [] # parameter name
			paramLen = [] # original parameter length
			paramDesc = [] # parameter description
			dataTypesNew = [] # --string, --byte, --half, --word, --double
			stringLen = [] # evaluated parameter length

			try:
				i = data.index("Data Fields") + 1
				j = data.index("Detailed Description")
				while i < j: # iterates through lines between Data Fields and Detailed Description

					# skips header parameters
					if 'Header' in data[i+1] or 'Hdr' in data[i+1]:
						i += 1
						while 'uint' not in data[i] and 'char' not in data[i] and i < j:
							i += 1

					else:
						dataTypesOrig.append(data[i]) # stores data type
						i += 1
						paramNames.append(data[i]) # stores parameter name
						i += 1
						param_len = ''
						if '[' in data[i]:
							param_len = data[i] # stores string length if provided
							i += 1
						paramLen.append(param_len)
						desc_string = ''
						while 'uint' not in data[i] and 'char' not in data[i] and i < j:
							desc_string = desc_string + ' ' + data[i]
							i += 1
						paramDesc.append(desc_string.lstrip()) # stores parameter description

						# determines new data type of parameter
						dataTypeNew = parser.findDataTypeNew(dataTypesOrig[-1], paramNames[-1], paramLen[-1], paramDesc[-1])
						dataTypesNew.append(dataTypeNew)

						# finds size of character array if type --string
						keyword = ''
						if dataTypeNew == '--string':
							keyword = re.sub('\[|\]|\(|\)','',paramLen[-1]) # removes brackets
							while keyword.isdigit() == False:
								keyword = parser.findStringLen(keyword)
								keyword = re.sub('\[|\]|\(|\)','',keyword)
						if keyword == '0':
							keyword = input(paramLen[-1]+' not found. Please enter value manually: ')
						stringLen.append(keyword)


				print "DATA TYPES:", dataTypesOrig
				print "PARAM NAMES: ", paramNames
				print "PARAM STRING LEN:", paramLen
				print "PARAM DESC: ", paramDesc
				print "UNIX DATA TYPES:", dataTypesNew
				print "STRING LENGTH:", stringLen, "\n"

			except ValueError:
				print "Data Fields not found in HTML file"

			# write data to a file
			file_split = re.split('/|\.', html_file)
			pickle_file = 'ParameterFiles/'+file_split[-2]
			with open(pickle_file,'wb') as pickle_obj:
				pickle.dump([dataTypesOrig, paramNames, paramLen, paramDesc, dataTypesNew, stringLen], pickle_obj)

			# resets data list for next HTML file
			parser.allData = [] 

