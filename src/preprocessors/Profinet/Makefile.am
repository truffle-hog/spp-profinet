## $Id
AUTOMAKE_OPTIONS=foreign no-dependencies

noinst_LIBRARIES = libprofinet.a

SUBDIRS = dissect send

libprofinet_a_SOURCES =

libprofinet_a_LIBADD = \
dissect/Dissector.o \
dissect/DissectorRegister.o \
dissect/dissectors/PacketDissector.o \
dissect/dissectors/EthernetDissector.o \
dissect/dissectors/PNRTDissector.o \
dissect/dissectors/PNRTADissector.o \
dissect/buffer/Buffy.o \
dissect/tree/ProtocolTree.o \
send/Sender.o \
send/UnixSocketSender.o \
send/Truffle.o

INCLUDES = @INCLUDES@
