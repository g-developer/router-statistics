#
#@author Tang Wenato (tangwentao@baidu.com)
#@date 2014-01-17
#

TARGET_LIB_COLOMBO = libcolombo.la
TARGET_LIB_IPLIBRARY = libiplibrary.la
TARGET_LIB_EXTIPLIB = libextiplib.la
TARGET_EXT_IPLIB_DUMP = bin/ext_iplib_dump
TARGET_EXT_IPLIB_GENERATOR = bin/ext_iplib_generator
TARGET_EXT_IPLIB_CONVERTER = bin/ext_iplib_converter
TARGET_EXT_IPLIB_SERVER = bin/ext_iplib_server

VERSION_INFO = 1:0:0

INCLUDE_DIR = ./include
CXX_FLAGS = -Wall -g -O2 -fPIC -I$(INCLUDE_DIR) -I./protobuf/include
#CXX_FLAGS = $(CXX_FLAGS) -D_COLOMBO_DEBUG_

SOURCES := $(wildcard src/*.cpp) $(wildcard src/*.cc)
OBJS    := $(addsuffix .o, $(basename $(SOURCES))) 
OBJS_LO := $(addsuffix .lo, $(basename $(SOURCES))) 

all: $(OBJS) $(TARGET_LIB_COLOMBO) $(TARGET_LIB_IPLIBRARY) $(TARGET_LIB_EXTIPLIB) $(TARGET_EXT_IPLIB_DUMP) $(TARGET_EXT_IPLIB_GENERATOR) $(TARGET_EXT_IPLIB_CONVERTER) $(TARGET_EXT_IPLIB_SERVER)

%.o: %.cpp
	libtool --mode=compile $(CXX) -c $^ $(CXX_FLAGS) -o $@

%.o: %.cc
	libtool --mode=compile $(CXX) -c $^ $(CXX_FLAGS) -o $@

$(TARGET_LIB_COLOMBO): src/ext_iplib_querier.lo src/iplibrary.pb.lo src/colombo_dict.lo src/ip_util.lo
	libtool --mode=link g++ -fPIC -g -o $@ $^ -L./protobuf/lib -lprotobuf -rpath /usr/local/lib -version-info $(VERSION_INFO) 
	rm .libs/$@ && cp $@ .libs

$(TARGET_LIB_EXTIPLIB): src/ext_iplib.lo src/ip_util.lo src/iconv_wrapper.lo
	libtool --mode=link g++ -static -fPIC -g -o $@ $^ -L./protobuf/lib -lprotobuf -rpath /usr/local/lib -version-info $(VERSION_INFO) 
	rm .libs/$@ && cp $@ .libs

$(TARGET_LIB_IPLIBRARY): src/iplibrary.lo 
	libtool --mode=link g++ -fPIC -g -o $@ $^ -rpath /usr/local/lib -version-info $(VERSION_INFO) 
	rm .libs/$@ && cp $@ .libs

$(TARGET_EXT_IPLIB_DUMP) : src/iplibrary.pb.lo src/ext_iplib_dump.lo src/iplibrary.lo src/ip_util.lo $(TARGET_LIB_EXTIPLIB)
	libtool --mode=link g++ -fPIC -g -o $@ $^ -L./protobuf/lib -lprotobuf -lpthread

$(TARGET_EXT_IPLIB_GENERATOR): src/iplibrary.pb.lo src/iplibrary.lo src/ext_iplib_generator.lo src/ext_iplib.lo src/ip_util.lo $(TARGET_LIB_EXTIPLIB)
	libtool --mode=link g++ -fPIC -g -o $@ $^ -L./protobuf/lib -lprotobuf -lpthread

$(TARGET_EXT_IPLIB_CONVERTER): src/iplibrary.pb.lo src/iplibrary.lo src/ext_iplib_converter.lo src/ext_iplib.lo src/ip_util.lo $(TARGET_LIB_EXTIPLIB)
	libtool --mode=link g++ -fPIC -g -o $@ $^ -L./protobuf/lib -lprotobuf -lpthread

$(TARGET_EXT_IPLIB_SERVER): src/ext_iplib_server.lo $(TARGET_LIB_COLOMBO)
	libtool --mode=link g++ -fPIC -g -o $@ $^ -L./protobuf/lib -lprotobuf -lpthread

clean:
	libtool --mode=clean rm -v $(OBJS) $(TARGET_LIB_COLOMBO) $(TARGET_EXT_IPLIB_DUMP) 
	libtool --mode=clean rm -v $(TARGET_EXT_IPLIB_GENERATOR) $(TARGET_EXT_IPLIB_CONVERTER) $(TARGET_EXT_IPLIB_SERVER)

