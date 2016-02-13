## $Id
AUTOMAKE_OPTIONS=foreign no-dependencies

noinst_LIBRARIES = libprofinet.a

SUBDIRS = dissect

libprofinet_a_SOURCES = \
Sender.c Sender.h \
Buffy.c Buffy.h \
ProtocolTree.c ProtocolTree.h \
UnixSocketSender.c UnixSocketSender.h \
Truffle.c Truffle.h

libprofinet_a_LIBADD = \
dissect/Dissector.o \
dissect/DissectorRegister.o \
dissect/PacketDissector.o \
dissect/EthernetDissector.o \
dissect/PNRTDissector.o \
Sender.o \
Buffy.o \
ProtocolTree.o \
UnixSocketSender.o

INCLUDES = @INCLUDES@
