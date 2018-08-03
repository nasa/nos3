##############################################################################
# Filename: CHeaderParser.py
# 
# Created in Winter 2014-15
# Created by Winter Intern for NASA GSFC Code 582
#
# Editted by: Keegan Moore
# Summer Intern for NASA GSFC Code 582
# Note about edits:
#	findDataTypeNew() - kept the same
#	getFileList() - completely new utility for determining which files to use
#	__main__ - significant changes made to main code
#		- kept class functionality
#		- presents data to user rather than asking user to look in hdr file
#		- preserved pickle utility so the rest of the ground system remains
#			the same
#
##############################################################################
# Description:
#	CHeaderParser.py is designed to help automate the process of adding
# core Flight System (cFS) application commands and parameters to the 
# python-based ground system that should come with this file. By prompting 
# the user for necessary information, and parsing the cFS header files, this 
# program will generate up to two different "pickle" files for the ground 
# system to use (for information about pickle files and the pickle python 
# libary, see the link here: https://docs.python.org/2/library/pickle.html).
# These files will be placed in CommandFiles/ or ParameterFiles/.
#
# Usage:
#	This program requires the directory path to the application header file
# that contains both the Command Code Definitions and the Command Structures.
# This program, by default, looks in 'CHeaderParser-hdr-paths.txt' for the
# path to the files. If the user is inputting multiple header files, the user
# can enter the path to each header file on a new line.
#
# Examples:
# ~$ python CHeaderParser.py
#	The above command would look in CHeaderParser-hdr-paths.txt for its list
#	of header paths. 
#
###############################################################################

import sys, os
import re
import string
import pickle

from struct import *

class CHeaderParser():

	#
	# Determines data type (--string, --byte, --half, --word, --double)
	#
	def findDataTypeNew(self, dataTypeOrig, paramName):
		if '[' in paramName:
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

	# getFileList()
	# Uses <filename> as a list of files to be parsed.
	# Default filename is CHeaderParser-hdr-paths.txt
	# This file is expected to be created by the user
	# Returns list of paths to those files
	# Added by Keegan Moore
	def getFileList(self, filename='CHeaderParser-hdr-paths.txt'):
		""" Gets the list of header files to be parsed from other file """
		
		paths = []
		
		# Try to open textfile list of header files
		try:
			textfile = open(filename, 'rb')
			for line in textfile:	
				# Remove whitespace before and after
				line = line.strip()
				# Don't worry about comment lines
				if line.find('#') == -1:
					# Don't worry about empty lines
					if line != '':
						# Add line to list of paths
						paths.append(line)
			print "Using header files found in " + filename
			# Send paths back to caller function
			return paths
		except IOError as e:
			print "Couldn't find default file. Checking command line arguments."
		except:
			print "Unexpected error:", sys.exc_info()[0]

		print "No header files found. Please make sure to provide the "
		print "default file for loading headers (CHeaderParser-hdr-paths.txt) "

		# If we got to here, we couldn't find any header files, return empty list
		return []


if __name__ == '__main__':

	#
	# Initializes CHeaderParser class
	#
	parser = CHeaderParser()

	# Get list of files to parse
	file_list = parser.getFileList()

	# If list is empty, exit now
	if not file_list:
		print "ERROR: Empty file list. Nothing to be done. Exiting now."
		exit()

	# initialize command codes/descriptions lists as empty lists
	cmdCodes = []
	cmdDesc = []

	# create empty list for dumping header data
	master_hdr = []

	# Concatonate all headers into one variable for referencing
	for hdr_file in file_list:
		# open header file as single header
		single_hdr = open(hdr_file, 'rb')
		# dump single header into master
		for single_line in single_hdr:
			master_hdr.append(single_line)

	#print "Master Header Length: " + str(len(master_hdr))

	# Reads and saves command and parameter information
	# Look through each line of the header file for command definitions
	for single_line in master_hdr:
		
		# Strip newline character off of single_line for easier parsing
		single_line = single_line.strip()

		if "#define" in single_line:
			# We have found a possible definition
			# What makes a valid definition is one that has the following:
			# '#define' 'COMMAND_DESCRIPTION' 'VALUE' /* Maybe a comment too! */
			# but what we might see is a conditional compilation #define
			# So the next step is to remove comments and check if the possible
			# definition has the three parts of a definition like it should

			if (single_line.find('/*') != -1):
				# There is a comment ('/*') somewhere in the definition
				# This program assumes there is nothing important after
				# So we get rid of it, before comparing it to the definition
				# structure
				single_line = single_line[:single_line.find('/*')]

			elif (single_line.find('//') != -1):			
				# There is a comment ('//') somewhere in the definition
				# This program assumes there is nothing important after
				# So we get rid of it, before comparing it to the definition
				# structure
				single_line = single_line[:single_line.find('//')]

			#else:
			#	print "Did not find any comments in definition."

			# Split single line into list for indexing
			definition = single_line.split()

			# Check if definition meets expected structure
			# Valid definitions should have following structure
			# '#define' 'COMMAND_DESCRIPTION' 'VALUE'
			# therefore a length of 3
			if len(definition) == 3:
				# Add command descriptions/codes to respective lists 
				cmdDesc.append(definition[1])
				cmdCodes.append(definition[2])

			#else:
			#	print "length not 3, see line below"
			#	print definition

	#print "List of Command Descriptions Below:"
	#print cmdDesc
	#print "--------------------------------------------------------------"
	#print "List of Command Codes Below:"
	#print cmdCodes
	#print "--------------------------------------------------------------"
	
	print "We need to save the command into to a pickle file in 'CommandFile/'. "
	print "Please do not use any spaces/quotes in your filename. Ex: my_app_cmd_file"
	cmd_file_name = raw_input("What would you like the command file to be saved as? ")
	
	# starting from last index (-1) going backward
	# (from example above) file_split[-2] = app_msg
	# therefore picklefile = CommandFiles/app_msg
	#pickle_file = 'CommandFiles/'+file_split[-2]
	pickle_file = 'CommandFiles/' + cmd_file_name

	# Open pickle file for storing command codes/descriptions
	with open(pickle_file, 'wb') as pickle_obj:
		#                  
		# FIGURE OUT WHY SHE DID THIS \\\
		#                              vvv
		pickle.dump([cmdDesc, cmdCodes, cmdDesc], pickle_obj)

	#####################################################################################################

	# Create a copy of command descriptions to preserve the original
	# unused_cmdDesc will be used to prompt user for parameters
	unused_cmdDesc = list(cmdDesc)

	# Create an empty list of used commands to be populated
	used_cmdDesc = []

	# Initialize looping variable
	cmd_index = 0
		
	# Print a list of unused commands for the user to pick from.
	print ""
	print "Unused Commands"
	print "-----------------------------------------"
	for cmd_index in range(0, len(unused_cmdDesc)):
		print "Command (" + str(cmd_index) + " of " + str(len(unused_cmdDesc)-1) + ") " + unused_cmdDesc[cmd_index]

	# Get user input to see if any commands from this file require parameters
	more_param_cmds = raw_input('Do any commands in ' + cmd_file_name + ' require parameters? (yes/no): ')

	while string.lower(more_param_cmds) not in ['yes', 'y', 'no', 'n']:
		print "Your response was not valid. Please enter (yes, y, no, or n)."
		more_param_cmds = raw_input('Do any commands in ' + cmd_file_name + ' require parameters? (yes/no): ')

	# Check for exit condition
	if string.lower(more_param_cmds) == 'no' or \
			string.lower(more_param_cmds) == 'n':
		print "You have chosen to exit. Exiting now."
		exit()

	# Continue onto creating parameter files if yes
	elif string.lower(more_param_cmds) == 'yes' or \
			string.lower(more_param_cmds) == 'y':
		
		# Continue to ask user for commands with parameters until we get -1 to exit.
		while True:			
			# Get user input
			command_choice = int(raw_input("Which command would you like to add parameters to (-1 to exit)? "))

			# Check for exit condition
			if command_choice == -1:
				print "You have chosen to exit early. Exiting now."
				break

			# Make sure the choice is within range
			while command_choice not in range(0, len(unused_cmdDesc)):
				print "You entered " + str(command_choice) + ", but that isn't an option."
				print "Choose from the list above or choose -1 to exit."

				# Get user input
				command_choice = int(raw_input("Which command would you like to add parameters to? "))

				# Check for exit condition
				if command_choice == -1:
					print "You have chosen to exit early. Exiting now."
					exit()
			
			cmdName = unused_cmdDesc[command_choice]

			# Initialize Parameter variables to empty lists
			paramNames = []
			paramDesc = []
			dataTypesOrig = []
			dataTypesNew = []
			paramLens = []
			stringLens = []

			# This empty list will hold possibly multiple lists of line numbers
			# each list representing where inside the App Header file the
			# structure can be found
			list_cmd_structs = []

			print "This program will now attempt to find the command structure for " + cmdName

			# create a copy of file_lines for parsing structures
			file_lines = list(master_hdr)

			# inside_struct will keep track of where the next for loop is while its scanning the file
			# if it is between '{' and '}', it will assume it's inside of a struct
			inside_struct = False

			# line_num will keep track of what line we are looking at in the header file
			line_num = 0

			# Look through each line of the header file for command structures
			while line_num in range(0, len(file_lines)):
				
				# check for the start of a structure
				if '{' in file_lines[line_num]:
					inside_struct = True

				# if none is found, pass by and keep searching
				else:
					line_num += 1
					pass

				# This empty list will hold the line numbers containing structure data 
				lines_of_struct = []
				
				while inside_struct:
					# Check for the end of the structure
					# We will still want to know what line this is
					if '}' in file_lines[line_num]:
						# Clear the flag, we found the end.
						inside_struct = False

						# Add line number to list, even if it has a '}'
						lines_of_struct.append(line_num)

						# Now that we know the struct is over, add list of lines
						# to the sets of command structures
						list_cmd_structs.append(lines_of_struct)
						break

					# Add line number to list
					lines_of_struct.append(line_num)
					line_num += 1

			# After exiting this while loop, cmdStruct should contain a list of lists
			# The nested lists should contain line numbers to each structure in file
			for line_list in list_cmd_structs:
				print ""
				print "Showing structure " + str(list_cmd_structs.index(line_list)) + " of " + str(len(list_cmd_structs)-1) + " below."
				print "--------------------------------------------"
				for line_num in line_list:
					# Print the line from the file using the index from the list
					print file_lines[line_num].strip()
				print "--------------------------------------------"

			# Give the user the option to exit too.
			print "Choose from the list above or choose -1 to exit."

			# Get choice of structure from user.
			struct_choice = int(raw_input("Which of the above structures would you like to use? "))

			# Check for exit condition
			if struct_choice == -1:
				print "You have chosen to exit early. Exiting now."
				exit()

			# Make sure the choice is valid
			while struct_choice not in range(0, len(list_cmd_structs)):
				print "You entered " + str(struct_choice) + ", but that isn't an option."
				print "Choose from the list above or choose -1 to exit."

				# Get user input
				struct_choice = int(raw_input("Which of the above structures would you like to use? "))

				# Check for exit condition
				if struct_choice == -1:
					print "You have chosen to exit early. Exiting now."
					exit()

			# After exiting the while loop, user's structure choice should be a valid assignment
			# Take the appropriate list of lines from the list of command structures
			cmd_struct_lines = list(list_cmd_structs[struct_choice])

			# Initialize variable to get into loop
			param_line = 0

			# The following loop will iterate each time another parameter is added to a command
			# After exiting the loop, the parameter variables should be updated with a list of
			# parameters for the user's chosen command
			while param_line in range(0, len(cmd_struct_lines)):

				# Display the command structure with indexed before each line
				print "\n\n"
				for line_num in range(0, len(cmd_struct_lines)):
					
					# Dereference the index number in cmd_struct_lines to get the actual line number
					actual_line = cmd_struct_lines[line_num]

					# print the line of the file with our "line number" next to it
					print "Line (" + str(line_num) + ") -> " + file_lines[actual_line].strip()

				# Prompt the user for line number
				param_line = int(raw_input("Enter the line of the parameter from the above print-out (-1 to stop): "))

				# Check exit condition
				if param_line == -1:
					print "You have chosen to stop adding parameters."
					break

				# Dereference the index number in cmd_struct_lines to get the actual line number
				actual_line = cmd_struct_lines[param_line]
				
				# Look at parameter line and split it into a list
				line_split = file_lines[actual_line].split()

				# Add original data type (C data type) to list
				dataTypesOrig.append(line_split[0])

				# Get rid of any occurance of ';' (at the end of the line)
				paramNames.append(re.sub(';','',line_split[1]))

				# Not sure about why we are keeping track of this yet
				# just fill it with null for now
				paramDesc.append('')

				# Determines data type for us to use
				# returns null if no type could match
				dataTypeNew = parser.findDataTypeNew(dataTypesOrig[-1], paramNames[-1])

				# If no type could match, ask user for data type
				if dataTypeNew == '':
					typeNew = parser.findKeyword(dataTypesOrig[-1])
					dataTypeNew = raw_input('Data type for '+paramNames[-1]+' not found. Please enter new data type by hand: ')

				dataTypesNew.append(dataTypeNew)

				# finds length if --string data type
				if dataTypeNew == '--string':

					# Split parameter name into list, separating by '[' or ']'
					# if paramNames[-1] == 'array[10]' then
					# array_name_size == ['array', '10']
					array_name_size = re.split('\[|\]', paramNames[-1])

					# Re-assign paramName to just the name of the array
					# (before -> 'array[10]', after -> 'array')
					paramNames[-1] = array_name_size[0]

					# set array size to the second element
					array_size = array_name_size[1]
					
					# Add array size to the parameter list
					paramLens.append(array_size)

					print "Array size: " + str(array_size)

					# Set flag initially to false in order to get into while loop				
					array_size_within_limit = False

					# Check conditions before loop to see if loop is even necessary
					if str(array_size).isdigit():
						if (0 <= int(array_size)) and (int(array_size) <= 128):
							# If we got to this point, we have valid data already
							# We won't need to go into while loop.
							array_size_within_limit = True

					# This while loop will make sure that the user input is both
					# - a valid integer
					# - between 0 and 128 (inclusively)
					while (not str(array_size).isdigit()) or (not array_size_within_limit):
						# User input is not an integer
						if not str(array_size).isdigit():
								print "Could not translate " + str(array_name_size[1]) + "."
						try:
							# Try to translate user input to an integer
							array_size = int(raw_input("Please enter the defined value for " + str(array_name_size[1]) + " (0 - 128): "))
							# If we got an integer, we continue to testing if integer is within range
							if (array_size < 0) or (128 < array_size):
								print "Array size out of bounds. It must be between 0 and 128."
								array_size_within_limit = False
								# Restart the loop, array_size not within limit
							else:
								array_size_within_limit = True
								# End of the loop.
						except:
							print "Could not translate " + str(array_name_size[1]) + " into integer. "
							# Restart the loop, array_size not integer

					# Add string length argument to parameter list
					stringLens.append(str(array_size))

				else:
					stringLens.append('')
					paramLens.append('')

				# print the last element of list to see if it worked
				print "dataTypeOrig:", dataTypesOrig[-1]
				print "dataTypeNew:", dataTypesNew[-1]
				print "paramName:", paramNames[-1]
				print "paramLen:", paramLens[-1]
				print "stringLen:", stringLens[-1]

				print "Added: " + paramNames[-1] + " with type " + dataTypesNew[-1]

				# Remove used parameter from command structure lines
				# so that the user doesn't choose the same parameter twice
				del cmd_struct_lines[param_line]

				# make sure param_line stays in bounds
				# (If the user chose the last line in this iteration,
				# param_line would be out of bounds after deleting a line)
				param_line = 0
				
				# Start the loop over to see if user has more parameters
			
			# Add command to used commands, to keep track of things
			used_cmdDesc.append(unused_cmdDesc[command_choice])

			# Take this command out of the list of unused commands before restarting the loop
			del unused_cmdDesc[command_choice]

			# If we later want the list of structs to be updated, to remove
			# previously selected structs, uncommend this line
			#del list_cmd_structs[struct_choice]


			# saves parameter information in pickle file for command
			pickle_file = 'ParameterFiles/'+cmdName
			with open(pickle_file, 'wb') as pickle_obj:
				pickle.dump([dataTypesOrig, paramNames, paramLens, paramDesc, dataTypesNew, stringLens], pickle_obj)

			# Initialize looping variable
			cmd_index = 0
				
			# Print a list of unused commands for the user to pick from.
			print ""
			print "Unused Commands"
			print "-----------------------------------------"
			for cmd_index in range(0, len(unused_cmdDesc)):
				print "Command (" + str(cmd_index) + " of " + str(len(unused_cmdDesc)-1) + ") " + unused_cmdDesc[cmd_index]

			# End of 'while True:'
			# We must have received a -1 when prompting user for the next command.

	print "Thank you for using CHeaderParser." 
	print "The following commands have been added with parameters: "
	print used_cmdDesc