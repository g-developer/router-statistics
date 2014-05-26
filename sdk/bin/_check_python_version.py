#!/usr/bin/env python
# encoding:utf8
# vim: set tw=0 shiftwidth=4 tabstop=4 expandtab number:

#
# @author Tang Wentao (tangwentao@baidu.com)
# @date 2014-01-22
#

import sys
import os

# check python version
req_version = [2,7]
cur_version = sys.version_info

# when run alone like: python _check_python_version.py 2.8
if __name__ == "__main__" and len(sys.argv) == 2:
    req_version = [int(i) for i in sys.argv[1].split(".")]

if cur_version < tuple(req_version):
    err_msg = "Your Python interpreter is too old. need python " + ".".join([str(i) for i in req_version])
    print >> sys.stderr, err_msg 
    raise Exception(err_msg)

