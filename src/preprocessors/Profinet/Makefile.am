## $Id
AUTOMAKE_OPTIONS=foreign no-dependencies

noinst_LIBRARIES = libprofinet.a

libprofinet_a_SOURCES = \
Dissector.c Dissector.h \
PNRTDissector.c PNRTDissector.h \
Sender.c Sender.h \ 
UnixSocketSender.c UnixSocketSender.h

libprofinet_a_LIBADD = \
Dissector.o \
PNRTDissector.o \
Sender.o \
UnixSocketSender.o

INCLUDES = @INCLUDES@
