VERSION=v2.3

prefix=/usr/local

CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld

SYS=posix
#SYS=mingw

SO_posix=so.0
SO_mingw=dll
SO_EXT=$(SO_$(SYS))

SHARED=yes
SODEF_yes=-fPIC
SOLIB_yes=libbdrtmp.$(SO_EXT)
SOINST_yes=install_$(SO_EXT)
SO_DEF=$(SODEF_$(SHARED))
SO_LIB=$(SOLIB_$(SHARED))
SO_INST=$(SOINST_$(SHARED))

####CRYPTO=OPENSSL
#CRYPTO=POLARSSL
#CRYPTO=GNUTLS
LIB_GNUTLS=-lgnutls -lgcrypt
LIB_OPENSSL=-lssl -lcrypto
LIB_POLARSSL=-lpolarssl
CRYPTO_LIB=$(LIB_$(CRYPTO))
DEF_=-DNO_CRYPTO
CRYPTO_DEF=$(DEF_$(CRYPTO))

DEF=-DRTMPDUMP_VERSION=\"$(VERSION)\" $(CRYPTO_DEF) $(XDEF)
OPT= -O0 #-g
CFLAGS=-Wall $(XCFLAGS) $(INC) $(DEF) $(OPT) $(SO_DEF)
LDFLAGS=-Wall $(XLDFLAGS)

bindir=$(prefix)/bin
sbindir=$(prefix)/sbin
mandir=$(prefix)/man

BINDIR=$(DESTDIR)$(bindir)
SBINDIR=$(DESTDIR)$(sbindir)
MANDIR=$(DESTDIR)$(mandir)

LIBS_posix=
LIBS_mingw=-lws2_32 -lwinmm -lgdi32
LIBS=$(CRYPTO_LIB) -lz $(LIBS_$(SYS)) $(XLIBS)

THREADLIB_posix=-lpthread -lm
THREADLIB_mingw=
THREADLIB=$(THREADLIB_$(SYS))
SLIBS=$(THREADLIB) $(LIBS)

#LIBCOLOMBO=sdk/libs/libcolombo.a
LIBCOLOMBO=sdk/protobuf/lib \
		   sdk/libs
INCRTMP=sdk/protobuf/include \
		sdk/include/
EXT_posix=
EXT_mingw=.exe
EXT=$(EXT_$(SYS))

ALL_darwin=$(LIBCOLOMBO)
ALL_android=$(ALL_darwin)
ALL_posix=$(ALL_android) progs

all:	$(ALL_$(SYS))

progs: connect

clean:
	rm -f *.o *.a *.so test
	rm -rf core.*

core:
	rm -rf core.*

connect:
	g++ -Wall -Wno-deprecated -fPIC -g -static -Isdk/protobuf/include -Isdk/include  -I. -Lsdk/protobuf/lib -Lsdk/libs t.cpp -o test -lcolombo -lprotobuf -lpthread 
