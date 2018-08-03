PROC file_compare (load_file, dump_file, hdr_size)

local logging = %liv(log_procedure)
%liv(log_procedure) = FALSE

;**********************************************************************
;  Test Name:  File Compare 
;  Test Level: Build Verification 
;  Test Type:  Functional 
;  Test Description
;     The purpose of this test is to compare the headers and contents of 
;     a file that has been loaded to memory with the headers and contents
;     of a file dumped from the same memory location and report the 
;     results of the comparision.
;
;  Requirements Tested
;
;  None
;
; 
;  Prerequisite Conditions
;  Availability of loaded and dumped files in the $WORK/image directory
;
;  Change History
;
;	Date		   Name		Description
;     --------       -------------  -----------
;     04/07/06       S. Applebee    Original Procedure
;     10/26/07       S. Applebee    Change results flag to global. Delete
;                                   results capture from cleanup.
;
;  Arguments
;
;      Name           Description
;      ----           -----------
;      load_file      Filename of file loaded to memory.
;      dump_file      Filename of file dumped from memory.
;      hdr_size       Size (in bytes) of the header info contained in each file.
;
;  Procedures/Utilities Called
;
;  None
;         
;**********************************************************************
;  Define variables
;**********************************************************************

; local account = %env("ACCOUNT")
; local work = %env("WORK")
; local image = work & "/image/"

global results
local results_comp_array[0..2, 0..2] = [["** OK ** ", "", " identical. "], ["*** ALERT *** ", "", " different."],["","",""]]
local load_file_head = load_file & ".head"
local load_file_tail = load_file & ".tail"
local lf_tail_command
local lf_head_command

local dump_file_head = dump_file & ".head"
local dump_file_tail = dump_file & ".tail"
local df_tail_command
local df_head_command
local data_start_byte

local compare_head_command
global header_results

local compare_tail_command
global contents_results

; setup & execute load file tail command

data_start_byte = hdr_size + 1
;lf_tail_command = "cd $WORK/image ; tail -c +117 " 
lf_tail_command = "cd $WORK/image ; tail -c +" 
lf_tail_command = lf_tail_command & data_start_byte & " " 
lf_tail_command = lf_tail_command & load_file & " > " 
lf_tail_command = lf_tail_command & load_file_tail

;write lf_tail_command
native lf_tail_command

; setup and execute load file head command

;lf_head_command = "cd $WORK/image ; head -c 116 " 
lf_head_command = "cd $WORK/image ; head -c " 
lf_head_command = lf_head_command & hdr_size & " " 
lf_head_command = lf_head_command & load_file & " > " 
lf_head_command = lf_head_command & load_file_head

;write lf_head_command
native lf_head_command

; setup & execute dump file tail command

;df_tail_command = "cd $WORK/image ; tail -c +117 " 
df_tail_command = "cd $WORK/image ; tail -c +" 
df_tail_command = df_tail_command & data_start_byte & " " 
df_tail_command = df_tail_command & dump_file & " > " 
df_tail_command = df_tail_command & dump_file_tail

;write df_tail_command
native df_tail_command

; setup and execute dump file head command

;df_head_command = "cd $WORK/image ; head -c 116 " 
df_head_command = "cd $WORK/image ; head -c " 
df_head_command = df_head_command & hdr_size & " " 
df_head_command = df_head_command & dump_file & " > " 
df_head_command = df_head_command & dump_file_head

;write df_head_command
native df_head_command

; compare headers of loaded and dumped files (116 bytes)

compare_head_command = "cd $WORK/image ; cmp -s "
compare_head_command = compare_head_command & " " & load_file_head
compare_head_command = compare_head_command & " " & dump_file_head
; write compare_head_command
results = %native(compare_head_command)

header_results = results_comp_array[results,0] & load_file
header_results = header_results & " | " 
header_results = header_results & dump_file 
header_results = header_results & " file headers are" 
header_results = header_results & results_comp_array[results,2] 
write
write header_results
write

; compare remaining portion of loaded and dumped files 

compare_tail_command = "cd $WORK/image ; cmp -s "
compare_tail_command = compare_tail_command & " " & load_file_tail
compare_tail_command = compare_tail_command & " " & dump_file_tail
; write compare_tail_command
results = %native(compare_tail_command)

contents_results = results_comp_array[results,0] & load_file
contents_results = contents_results & " | " 
contents_results = contents_results & dump_file 
contents_results = contents_results & " file contents are" 
contents_results = contents_results & results_comp_array[results,2] 
write
write contents_results
write

CLEANUP:

local cleanup_command

cleanup_command = "cd $WORK/image; rm *.tail ; rm *.head"
; write cleanup_command
; results = %native(cleanup_command)
native cleanup_command

%liv(log_procedure) = logging

endproc
