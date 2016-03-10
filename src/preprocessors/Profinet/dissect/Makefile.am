## $Id
AUTOMAKE_OPTIONS=foreign no-dependencies

noinst_LIBRARIES = libprofinet_dissect.a

SUBDIRS = dissectors buffer tree

libprofinet_dissect_a_SOURCES = \
Dissector.c Dissector.h \
DissectorRegister.c DissectorRegister.h \
DissectionUtils.c DissectionUtils.h

libprofinet_a_LIBADD = \
dissectors/PacketDissector.o \
dissectors/EthernetDissector.o \
dissectors/PNRTDissector.o \
dissectors/PNRTADissector.o \
dissectors/DCPDissector.o \
dissectors/BlockDissector.o \
dissectors/OptionDissector.o \
buffer/Buffy.o \
tree/ProtocolTree.o

INCLUDES = @INCLUDES@
