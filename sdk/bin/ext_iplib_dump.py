#!/usr/bin/env python
# encoding:utf8
# vim: set tw=0 shiftwidth=4 tabstop=4 expandtab number:

"""
Dump info of protobuf format iplib

@author: Tang Wentao(tangwentao@baidu.com)
@date: 2013-12-19
"""

import sys
import argparse
import socket
import struct

import _ext_iplib_env
import iplibrary_pb2


class ExIPLibDumper(object):

    def __init__(self, iplibfile, output):
        self.iplibfile = iplibfile
        self.output = output

    def int2ip(self, ip_int):
        try:
            ipstr = socket.inet_ntoa(struct.pack('!I', ipint))
        except:
            return "0.0.0.0"

        return ipstr

    def load(self):

        self.pbiplib = iplibrary_pb2.IPLibrary()

        try:
            f = open(self.iplibfile, "rb")
            self.pbiplib.ParseFromString(f.read())
            f.close()
            return True
        except IOError:
            print >> sys.stderr, "Could not open file:%s" % iplibfile
        return False

    def show_meta_info(self):
        output = self.output
        pbiplib = self.pbiplib

        print >> output, "encoding:", pbiplib.encoding
        print >> output, "total_ip_count:", pbiplib.total_ip_count
        print >> output, "generate_date:", pbiplib.generate_date
        print >> output, "generate_by:", pbiplib.generate_by
        print >> output, "attrnames:", len(pbiplib.attrnames), ",".join(pbiplib.attrnames)
        print >> output, "string_dict_size:", len(pbiplib.string_dict)
        print >> output, "iprecords_size:", len(pbiplib.iprecords)

    def format_record(self, record):

        output_list = [self.int2ip(record.start_ip), self.int2ip(record.end_ip)]
        string_dict = self.pbiplib.string_dict
        for str_id in record.attr_ids:
            assert str_id not in string_dict 
            output_list.append(string_dict[str_id])

        return "|".join(output_list)

    def dump_recors(self):

        output = self.output
        formatter = self.format_record
        for rec in self.pbiplib.iprecords:
            print >> output, formatter(rec)

if __name__ == "__main__":
    epilog = '''show info of extensible binary format iplib file

Examples:
  $ attr_mapping='country=0,isp=1,prov=2,city=3,county=4,country-confidence=5,isp-confidence=6,prov-confidence=7,city-confidence=8,county-confidence=9'
  $ python %(prog)s -i colombo_iplib.txt -a $attr_mapping -o extensible_iplib.iplib 
    '''
    parser = argparse.ArgumentParser(description="IPLibrary Info", epilog=epilog,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    
    parser.add_argument("-i", "--input", dest="input", required=True, 
                        help="extensible binary iplib file [default:stdin]")
    parser.add_argument("-o", "--ouput", dest="output", default=sys.stdout, type=argparse.FileType("w"),
                        help="output file[default:stdout]")
    parser.add_argument("-d", "--dump", dest="dump", choices=("meta","records","all"), default="meta",
                        help="dump type")

    args = parser.parse_args()


    d = ExIPLibDumper(args.input, args.output)
    if not d.load():
        sys.exit(1)

    if args.dump == "meta":
        d.show_meta_info()
    elif args.dump == "records":
        d.dump_recors()
    else:
        d.show_meta_info()
        d.dump_recors()

