#!/usr/bin/env python
# encoding:utf8
# vim: set tw=0 shiftwidth=4 tabstop=4 expandtab number:

#
# @author Tang Wentao (tangwentao@baidu.com)
# @date 2014-01-17
#

import sys
import os

# check python version
import _check_python_version

# setup env
mypath = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.normpath(mypath + "/../libs/python"))


# check protobuf python libs
try:
    import iplibrary_pb2
except Exception, e:
    print >> sys.stderr, str(e)
    raise Exception("you should build sdk first: cd sdk && sh build.sh")

