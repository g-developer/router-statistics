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

def check_config(input_config_file, configs_map, section="updater"):
    ''' check config according to configs_map '''

    config = ConfigParser.ConfigParser()
    config.read(input_config_file)

    updater_configs = config.items(section)

    valid_configs = [] 

    for opt, val in updater_configs:
        if opt not in configs_map:
            print >> sys.stderr, "unrecognized option: %s" % (opt)
            return False, [] 

        opt_type, can_be_empty, default_vals = configs_map.pop(opt)

        if not can_be_empty and not val: 
            print >> sys.stderr, "option %s can not be empty" %(opt)
            return False, [] 

        if opt_type == "string":
            if val and len(val.split()) > 1:
                print >> sys.stderr, "error, option %s with space in:'%s'" % (opt, val)
                return False, []

            if can_be_empty and not val:
                val = default_vals

        elif opt_type == "choices":
            if val not in default_vals:
                print >> sys.stderr, "option %s choices are:%s" %(opt, ", ".join(default_vals))
                return False, [] 

        elif opt_type == "int":
            try: 
                val = int(val)
                if not default_vals[0] <= val <= default_vals[1]:
                    print >> sys.stderr, "option %s should in range %s" %(opt, default_vals) 
                    return False, [] 
            except Exception, e:
                print >> sys.stderr, "option %s invaild val. %s" %(opt, str(e)) 
                return False, [] 

        elif opt_type == "email":
            if not re.match('^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$', val):
                print >> sys.stderr, "invaild email: %s" %(val) 
                return False, [] 

        elif opt_type == "list":
            opt_list = val.strip().split(",")
            if not opt_list: 
                print >> sys.stderr, "invalid option: '%s'" %(opt) 
                return False, [] 
            for i in xrange(len(opt_list)):
                if not opt_list[i]:
                    print >> sys.stderr, "invalid option: '%s' with val: '%s'" %(opt, val) 
                    return False, [] 
        else:
            print >> sys.stderr, "invalid option type:" %(opt_type) 
            return False, [] 

        valid_configs.append([opt,val])

    required_config_keys = dict.fromkeys(configs_map, False) 
    for k,v in valid_configs:
        if k in required_config_keys:
            del required_config_keys

    if required_config_keys:
        print >> sys.stderr, "these config options are required:", ",".join(required_config_keys.keys())
        return False, [] 

    return True, valid_configs  

