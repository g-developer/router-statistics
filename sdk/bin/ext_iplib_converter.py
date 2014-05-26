#!/usr/bin/env python
# encoding:utf8
# vim: set tw=0 shiftwidth=4 tabstop=4 expandtab number:

"""
Convert encoding or strip fileds of protobuf format iplib

@author: Tang Wentao(tangwentao@baidu.com)
@date: 2013-12-19
"""

import sys
import argparse
import time
import platform

import _ext_iplib_env 
import iplibrary_pb2


def parse_attributes_list(attrs_list_str):
    # format: 
    # country,isp
    attrs_list_str = attrs_list_str.strip()
    if not attrs_list_str:
        return []

    items = attrs_list_str.split(",")
    return [ i.strip() for i in items if i.strip() ]

class ExtIPLibConverter(object):
    
    def convert(self, convert_config):
        '''
            @param convert_config config object
        '''
        pbiplib = iplibrary_pb2.IPLibrary()

        try:
            f = open(args.input, "rb")
            pbiplib.ParseFromString(f.read())
            f.close()
        except IOError:
            print >> sys.stderr, "Could not open file:%s" % args.input 
            return

        # check target attr list
        input_attrnames_dict = {}
        for i in xrange(len(pbiplib.attrnames)):
            input_attrnames_dict[pbiplib.attrnames[i]] = i

        #print input_attrnames_dict

        target_attr_id_indexs = []
        for attrname in convert_config.target_attrnames:
            attr_id_index = input_attrnames_dict.get(attrname)
            # index 0 is ok 
            if None == attr_id_index: 
                print >> sys.stderr, "target attr %s not exists in input file" % repr(attrname)
                return
            target_attr_id_indexs.append(attr_id_index)

        # convert attrs
        converted_iprecords = pbiplib.iprecords[:] 
        for rec in converted_iprecords:
            convert_attr_ids = [rec.attr_ids[i] for i in target_attr_id_indexs]
            del rec.attr_ids[:]
            rec.attr_ids.extend(convert_attr_ids)

        # merge continues iprecords
        merged_iprecords = []
        last_start_ip_int = 0
        last_end_ip_int = 0
        last_attrs = None

        target_string_ids = {}
        self.target_total_ip_count = 0

        def add_new_rec(start_ip, end_ip, attrs):
            rec = iplibrary_pb2.IPLibrary.IPRecord()
            rec.start_ip, rec.end_ip = start_ip, end_ip
            rec.attr_ids.extend(attrs)
            merged_iprecords.append(rec)
            for attr_id in rec.attr_ids:
                if attr_id not in target_string_ids:
                    target_string_ids[attr_id] = 0
                target_string_ids[attr_id] += 1

            self.target_total_ip_count += rec.end_ip - rec.start_ip + 1

        for rec in converted_iprecords:
            start_ip_int, end_ip_int, attrs = rec.start_ip, rec.end_ip, rec.attr_ids
            if last_attrs != None:
                if last_start_ip_int <= start_ip_int  <= last_end_ip_int + 1 and last_attrs == attrs:
                    last_end_ip_int = end_ip_int
                    continue

                add_new_rec(last_start_ip_int, last_end_ip_int, last_attrs)

            last_start_ip_int = start_ip_int
            last_end_ip_int = end_ip_int
            last_attrs = attrs

        if last_attrs != None:
            add_new_rec(last_start_ip_int, last_end_ip_int, last_attrs)

        target_string_ids_list = target_string_ids.items()
        target_string_ids_list.sort(key=lambda x:x[1], reverse=True) 
        
        in_encoding = pbiplib.encoding
        out_encoding = convert_config.target_encoding
        do_convert_encoding = in_encoding != out_encoding

        #print do_convert_encoding, in_encoding, out_encoding

        def convert_encoding(s):
            ''' may raise exception, when convert failed'''
            if not do_convert_encoding:
                return s

            return s.decode(in_encoding).encode(out_encoding)

        string_id_mapping = {}
        target_string_dict = []
        for i in xrange(len(target_string_ids_list)):
            old_attr_id, new_attr_count = target_string_ids_list[i]
            string_id_mapping[old_attr_id] = i
            target_string_dict.append(convert_encoding(pbiplib.string_dict[old_attr_id]))
                
        # updating attr_ids
        for rec in merged_iprecords:
            for i in xrange(len(rec.attr_ids)):
                rec.attr_ids[i] = string_id_mapping[rec.attr_ids[i]]

        # save to file
        out_iplib = iplibrary_pb2.IPLibrary()
        out_iplib.encoding = out_encoding
        out_iplib.total_ip_count = self.target_total_ip_count
        out_iplib.generate_date = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
        out_iplib.generate_by = convert_config.generate_by + ' ' + platform.node()
        out_iplib.attrnames.extend(convert_config.target_attrnames)
        out_iplib.string_dict.extend(target_string_dict)
        out_iplib.iprecords.extend(merged_iprecords)

        convert_config.output.write(out_iplib.SerializeToString())
        
if __name__ == "__main__":
    epilog = '''Convert extensible binary format

Examples:
  $ python %(prog)s -i xxx.iplib -a isp,prov -o extensible_iplib.iplib -e utf8
    '''
    parser = argparse.ArgumentParser(description="IP Library Converter", epilog=epilog,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    
    parser.add_argument("-i", "--input", dest="input", required=True, 
                        help="input extensible binary format iplib file")
    parser.add_argument("-o", "--ouput", dest="output", type=argparse.FileType("wb"),
                        help="output extensible binary format iplib file")
    parser.add_argument("-a", "--attrs-list", dest="attrs_list", required=True,
                        help="keep attributes name list")
    parser.add_argument("-e", "--target-encoding", dest="target_encoding", required=True,
                        help="target encoding")
    parser.add_argument("-b", "--generate-by", dest="generate_by", default="colombo@baidu.com",
                        help="genenrate by")

    args = parser.parse_args()

    attrnames = parse_attributes_list(args.attrs_list)
    if not attrnames:
        print >> sys.stderr, "parse attrs list wrong:%s" % args.attrs_list 
        sys.exit(1)


    class Config(object):
        pass

    convert_config = Config() 
    convert_config.input = args.input
    convert_config.output = args.output
    convert_config.target_attrnames = attrnames 
    convert_config.target_encoding = args.target_encoding
    convert_config.generate_by = args.generate_by

    s = ExtIPLibConverter()
    s.convert(convert_config)

