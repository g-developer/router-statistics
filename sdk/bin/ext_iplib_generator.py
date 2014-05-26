#!/usr/bin/env python
# encoding:utf8
# vim: set tw=0 shiftwidth=4 tabstop=4 expandtab number:

"""
Generate protobuf format iplib from colombo text format iplib

@author: Tang Wentao(tangwentao@baidu.com)
@date: 2013-12-17
"""

import sys
import argparse
import time

import _ext_iplib_env
from iprecords import IPRecords
import iplibrary_pb2

def parse_attributes_key_values(attrs_list_str):
    # format: 
    # country=0,isp=1
    items = attrs_list_str.split(",")
    if not items:
        return {}

    attrs_list = []

    for item in items:
        kv = item.split("=")
        if len(kv) != 2:
            print >> sys.stderr, "wrong attributes:%s" % item
            return [] 
        attrs_list.append([kv[0],int(kv[1])])

    return attrs_list 


class ExtIPLibGenerator(IPRecords):
    

    def dump_to_pb(self, dump_config):
        '''
            @param dump_config config object
        '''
        pbiplib = iplibrary_pb2.IPLibrary()
        pbiplib.encoding = "utf-8"
        pbiplib.total_ip_count = self.count_ip()
        pbiplib.generate_date = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) 
        pbiplib.generate_by = dump_config.generate_by 

        # attr name list
        pbiplib.attrnames.extend([x[0] for x in dump_config.attrs_list])

        attr_dict = {}
        for record in self._ip_records:
            for attr in record[2]:
                attr_key = attr
                if attr_key not in attr_dict:
                    attr_dict[attr_key] = 0
                attr_dict[attr_key] += 1

        attr_list = attr_dict.items()
        attr_list.sort(key=lambda x:x[1], reverse=True)

        pbiplib.string_dict.extend([x[0] for x in attr_list])

        string_dict = dict([[attr_list[i][0],i] for i in xrange(len(attr_list))])

        '''
        f = open("string_dict.txt", "w")
        for i in xrange(len(attr_list)):
            print >> f, "%s %d %d" % (attr_list[i][0], i, attr_list[i][1])
        f.close()
        '''
        
        for record in self._ip_records:
            pb_iprecord = pbiplib.iprecords.add()
            pb_iprecord.start_ip = record[0]
            pb_iprecord.end_ip = record[1]
            pb_iprecord.attr_ids.extend([string_dict[a] for a in record[2]])

        dump_config.output.write(pbiplib.SerializeToString())
        
if __name__ == "__main__":
    epilog = '''Convert colombo text format IP library to extensible binary format

Examples:
  $ attr_mapping='country=0,isp=1,prov=2,city=3,county=4,country-confidence=5,isp-confidence=6,prov-confidence=7,city-confidence=8,county-confidence=9'
  $ python %(prog)s -i colombo_iplib.txt -a $attr_mapping -o extensible_iplib.iplib 
    '''
    parser = argparse.ArgumentParser(description="IP Library Converter", epilog=epilog,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    
    parser.add_argument("-i", "--input", dest="input", default=sys.stdin, type=argparse.FileType("r"),
                        help="colombo formated iplib file [default:stdin]")
    parser.add_argument("-o", "--ouput", dest="output", default=sys.stdout, type=argparse.FileType("wb"),
                        help="output file[default:stdout]")
    parser.add_argument("-a", "--attrs-list", dest="attrs_list", required=True,
                        help="attributes name-index list")
    parser.add_argument("-b", "--generate-by", dest="generate_by", default="colombo@baidu.com",
                        help="genenrate by")

    args = parser.parse_args()

    attrs_list = parse_attributes_key_values(args.attrs_list)
    if not attrs_list:
        print >> sys.stderr, "parse attrs list wrong:%s" % args.attrs_list 
        sys.exit(1)

    class Config(object):
        pass

    dump_config = Config() 
    dump_config.output = args.output
    dump_config.attrs_list = attrs_list 
    dump_config.generate_by = args.generate_by

    converter = ExtIPLibGenerator(args.input, [a[1] for a in attrs_list])
    converter.dump_to_pb(dump_config)

