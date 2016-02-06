## $Id
AUTOMAKE_OPTIONS=foreign no-dependencies

noinst_LIBRARIES = libprofinet.a

SUBDIRS = dissectors

libprofinet_a_SOURCES = \
Dissector.c Dissector.h \
Sender.c Sender.h \
UnixSocketSender.c UnixSocketSender.h \
DissectorRegister.c DissectorRegister.h

libprofinet_a_LIBADD = \
Dissector.o \
dissectors/PNRTDissector.o \
Sender.o \
UnixSocketSender.o \
DissectorRegister.o

INCLUDES = @INCLUDES@
