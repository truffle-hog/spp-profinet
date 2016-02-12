## $Id
AUTOMAKE_OPTIONS=foreign no-dependencies

noinst_LIBRARIES = libprofinet.a

SUBDIRS = dissectors

libprofinet_a_SOURCES = \
Dissector.c Dissector.h \
DissectorRegister.c DissectorRegister.h \
Sender.c Sender.h \
Buffy.c Buffy.h \
ProtocolTree.c ProtocolTree.h \
UnixSocketSender.c UnixSocketSender.h \
Truffle.c Truffle.h

libprofinet_a_LIBADD = \
Dissector.o \
dissectors/PacketDissector.o \
Sender.o \
Buffy.o \
ProtocolTree.o \
UnixSocketSender.o

INCLUDES = @INCLUDES@
