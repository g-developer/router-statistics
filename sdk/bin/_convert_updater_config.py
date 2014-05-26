#!/usr/bin/env python
# encoding:utf8
# vim: set tw=0 shiftwidth=4 tabstop=4 expandtab number:

#
# @author Tang Wentao (tangwentao@baidu.com)
# @date 2014-01-22
#

import sys
import os
import ConfigParser
import re
import _check_python_version
import _check_config 

CHECK_CONFIGS_MAP = {
    "appkey": ["string", False, ""],
    "app_token": ["string", False, ""],
    "product": ["string", False, ""],
    "email": ["email", False, ""],
    "ip_attr_encoding": ["choices", False, ("utf-8", "gbk")],
    "ip_attrs": ["list", False, ""],
    "updater_workdir": ["string", False, ""],
    "updater_outputdir": ["string", True, "$updater_workdir/output"],
    "output_file_prefix": ["string", True, "colombo"],
    "backup_days": ["int", False, (1,36500)]
}

def convert_config(input_config_file, output_config_file):
    valid, valid_configs = _check_config.check_config(input_config_file, CHECK_CONFIGS_MAP, "updater")
    if not valid:
        return False


    output_file = open(output_config_file, "w")
    if not output_file:
        print >> sys.stderr, "open %s failed" % ( output_config_file )
        return False

    for k,v in valid_configs:
        print >> output_file, "%s=\"%s\"" % (k,v)
    output_file.close()

    return True


if __name__ == "__main__":

    if len(sys.argv) < 3:
        print >> sys.stderr, "Usage:%s <input config> <output config>" %(sys.argv[0])
        sys.exit(1)

    if not convert_config(sys.argv[1], sys.argv[2]):
        sys.exit(1)

