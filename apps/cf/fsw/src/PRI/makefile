CC = gcc
CFLAGS = -c -g -I. -I../PUB -Wall -Wstrict-prototypes -ansi -fno-builtin

libcfdp.a : \
       aaa.o callbacks.o cfdp_lib.o \
       default_filestore.o default_indication.o \
       event.o machine_list.o message_class.o mib.o \
       misc.o nak.o nak_mem.o pdu.o \
       pdu_as_string.o r1.o s1.o r2.o s2.o timer.o utils.o
	ar -r libcfdp.a \
            aaa.o callbacks.o cfdp_lib.o  \
            default_filestore.o default_indication.o \
            event.o machine_list.o message_class.o mib.o \
            misc.o nak.o nak_mem.o pdu.o \
            pdu_as_string.o r1.o s1.o r2.o s2.o timer.o utils.o

clean : remove_library remove_objects libcfdp.a

remove_library :
	rm -f libcfdp.a

remove_objects :
	rm -f *.o
