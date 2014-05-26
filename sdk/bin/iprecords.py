#!/usr/bin/env python
# encoding:utf-8
# vim: set tw=0 shiftwidth=4 tabstop=4 expandtab number:
import re
import socket
import struct
import sys

"""
one ip record are known as:
start_ip|end_ip|attribute[|more attributes]

@author: Tang Wentao(tangwentao@baidu.com)
@date: 2013-03-29
"""

'''
util functions
'''

def parse_attributes_list(attributes_list_string):
    
    '''
    parse attributes index list string to index list 
    @return: indexs list, empty list if parse failed
    '''
    try:
        items = attributes_list_string.split(",")

        indexs_list = []
        for num in items:
            if "-" in num:
                ranges = num.split("-")
                if len(ranges) == 2:
                    indexs_list.extend(range(int(ranges[0]),int(ranges[1])+1))
            elif num:
                indexs_list.append(int(num))

        return indexs_list
    except Exception,e:
        print >> sys.stderr, "error in parse_attributes_list:", str(e)

    return []

'''
IPRecords class
'''
class IPRecords(object):
    
    def __init__(self, col_iplib_file, attrs=[], compress_loaded_records=True, sort_records=True, sep_regex="\||:"):
        '''
        Args:
            col_iplib_file, col_formated file name or opened file object for reading 
            attrs, attributes index list of ip record, like [1,2]
            cache_in_file, whether cache in pickle dumped file
        '''
        self.stats = {}
        self.sep_regex = sep_regex
        self._attrs = attrs
        if not attrs:
            self._max_attr = -1
        else:
            self._max_attr = max(attrs)
            
        self._col_iplib_filename = col_iplib_file
        if isinstance(col_iplib_file,file):
            self._col_iplib_filename = col_iplib_file.name 

        self._ip_records = []
        self._attr_names = []
        
        # load ip records
        self._load_from_iplib(col_iplib_file)
        
        # sort ascending with start ip
        if sort_records:
            self._sort_ip_records()
        
        # check if there is overlaps of different ip records
        self._check_overlap()
        
        if compress_loaded_records:
            # compress continues ip records
            self._compress_continues_ip_records()
        
    def _get_word_from_word_dict(self, words_dict, word):
        if word not in words_dict:
            words_dict[word] = word
        return words_dict[word]

    def _process_attrs(self, attrs):
        '''
        only store attributes to query
        '''
        # the max index is large then the count of attributes
        if self._attrs and self._max_attr >= len(attrs): 
            raise Exception("Wrong construct parameter `attrs`, should not larger than the count of attributes of a record")

        if not hasattr(self, "_words_dict"):
            self._words_dict = {}
            
        filtered_attrs = attrs
        if self._attrs:
            filtered_attrs = [attrs[i] for i in self._attrs]
            
        return [self._get_word_from_word_dict(self._words_dict, word) for word in filtered_attrs]

    def _load_from_iplib(self, col_iplib_file):
        '''
        load iplib records from col_iplib formatted file
        Exceptions may raise if there is loading error
        '''

        # not file object, is a file path
        if isinstance(col_iplib_file, file):
            self._do_load_from_iplib_file(col_iplib_file)
        else:
            fp = open(col_iplib_file, "r")
            if not fp:
                raise Exception("Open %s failed")

            self._do_load_from_iplib_file(fp)

            fp.close()

    def _do_load_from_iplib_file(self, iplibf):
        '''
        Input args:
            iplibf, opened file object
        '''

        self._ip_records = []
        total_ip_num = 0
        records_num = 0
        pattern = re.compile(self.sep_regex)
        for line in iplibf:
            line = line.strip()
            
            # ignore empty line and comment lines
            if not line or line.startswith("#"):
                # get attr names
                if line and line.startswith("#ipstart|ipend|"):
                    names = line.split("|")
                    self._attr_names = self._process_attrs(names[2:])
                continue
            
            segs = pattern.split(line)
            start_ip = self.ip2int(segs[0])
            end_ip = self.ip2int(segs[1])
            attrs = self._process_attrs(segs[2:])
            self._ip_records.append([start_ip, end_ip, attrs])

            try:
                total_ip_num += (end_ip - start_ip + 1)
                records_num += 1
            except Exception,e:
                print >> sys.stderr, line, start_ip, end_ip,"<-->", repr(segs)
                print >> sys.stderr, str(e)
                raise Exception(str(e))
        
        self.stats["total_ip_num"] = total_ip_num
        self.stats["records_num"] = records_num 
        
    def _sort_ip_records(self):
        """
        sort ascendingly
        """
        self._ip_records.sort(cmp=lambda x, y: cmp(x[0], y[0]))
    
    def _check_overlap(self):
        """
        check if ip records is overlaped
        rely on the ip records area SORTED!
        """
        last_record = None
        for ip_record in self._ip_records:
            start_ip_int = ip_record[0]
            attrs = ip_record[2]
            
            # overlapped, last_record[1] == last_end_ip_int
            if last_record and start_ip_int <= last_record[1]:
                if last_record[2] != attrs:
                    raise Exception("records overlapped %s <-> %s" % (self._format_record(last_record), self._format_record(ip_record)))
            
            last_record = ip_record
    
    def _compress_continues_ip_records(self):
        '''
        merge continues ip records,
        '''
        new_ip_records = []
        last_start_ip_int = 0
        last_end_ip_int = 0
        last_attrs = None
        for ip_record in self._ip_records:
            start_ip_int, end_ip_int, attrs = ip_record
            
            if 0 != last_end_ip_int:
                if last_start_ip_int <= start_ip_int  <= last_end_ip_int + 1 and last_attrs == attrs:
                    last_end_ip_int = end_ip_int
                    continue
                
                new_ip_records.append([last_start_ip_int, last_end_ip_int, last_attrs])
            
            last_start_ip_int = start_ip_int
            last_end_ip_int = end_ip_int
            last_attrs = attrs
        
        if 0 != last_end_ip_int:
            new_ip_records.append([last_start_ip_int, last_end_ip_int, last_attrs])
        self._ip_records = new_ip_records
        
    def ip2int(self, ipstr):
        try:
            ipint = struct.unpack('!I', socket.inet_aton(ipstr))[0]
            return ipint
        except:
            pass
        
        return None 

    def int2ip(self, ipint):
        try:
            ipstr = socket.inet_ntoa(struct.pack('!I', ipint))
        except:
            return "0.0.0.0"
        
        return ipstr
    
    def _binary_search(self, ipint):
        '''
        binary search 
        return merged ip record
        '''
        iprecords = self._ip_records
        s = 0
        e = len(iprecords) - 1
        index = 0
        while  s <= e:
            index = (s + e) / 2 
            cur_record = iprecords[index]

            if cur_record[0] <= ipint <= cur_record[1]:
                return cur_record 

            if ipint < cur_record[0]: 
                e = index - 1 
            else:    
                s = index + 1 

        return ()
        
    def diff_ip_records(self, left_ip_records, right_ip_records, diff_func):
        
        left_records_len = len(left_ip_records)
        right_records_len = len(right_ip_records)
        
        left_index = 0
        right_index = 0
        next_left = True
        next_right = True
        
        while left_index < left_records_len or right_index < right_records_len:
            if next_left:
                if left_index >= left_records_len:
                    left_start_ip_int, left_end_ip_int, left_attrs = 0xFFFFFFFF, 0xFFFFFFFF, []
                else:
                    left_start_ip_int, left_end_ip_int, left_attrs = left_ip_records[left_index]
                next_left = False
                
            if next_right:
                if right_index >= right_records_len:
                    right_start_ip_int, right_end_ip_int, right_attrs = 0xFFFFFFFF, 0xFFFFFFFF, []
                else:
                    right_start_ip_int, right_end_ip_int, right_attrs = right_ip_records[right_index]
                next_right = False
            
            if left_start_ip_int < right_start_ip_int:
                if left_end_ip_int < right_start_ip_int:
                    diff_func(left_start_ip_int, left_end_ip_int, left_attrs, [])
                    left_index += 1
                    next_left = True
                else: # left_end_ip_int >= right_start_ip_int
                    diff_func(left_start_ip_int, right_start_ip_int - 1, left_attrs, [])
                    left_start_ip_int = right_start_ip_int
            elif left_start_ip_int == right_start_ip_int:
                if left_end_ip_int > right_end_ip_int:
                    diff_func(left_start_ip_int, right_end_ip_int, left_attrs, right_attrs)
                    left_start_ip_int = right_end_ip_int + 1
                    right_index += 1
                    next_right = True
                elif left_end_ip_int == right_end_ip_int:
                    diff_func(left_start_ip_int, left_end_ip_int, left_attrs, right_attrs)
                    left_index += 1
                    next_left = True
                    right_index += 1
                    next_right = True
                else: # left_end_ip_int < right_end_ip_int
                    diff_func(left_start_ip_int, left_end_ip_int, left_attrs, right_attrs)
                    right_start_ip_int = left_end_ip_int + 1
                    left_index += 1
                    next_left = True
            else: # left_start_ip_int > right_start_ip_int
                if left_start_ip_int > right_end_ip_int:
                    diff_func(right_start_ip_int, right_end_ip_int, [], right_attrs)
                    right_index += 1
                    next_right = True
                else: #left_start_ip_int <= right_end_ip_int
                    diff_func(right_start_ip_int, left_start_ip_int - 1, [], right_attrs)
                    right_start_ip_int = left_start_ip_int
                    
    def merge_ip_records(self, ip_records, merge_func=lambda x, y:(True, y), compress_merged_result=True, output_not_covable=False, merge_callback=None):
        """
        Args:
            merge_func, function like merge(a,b), test a is coverable by b,
                return is a list (coverable, coverable result) like (True, a) or (True, b), or (False, None)
            compress_merged_result, whether compress merged result 
            merge_callback: merge callback function
        """
        new_ip_records = []
        
        def _merge_record(start_int, end_int, from_attrs, to_attrs):
            covable, attrs = merge_func(from_attrs, to_attrs)
            if covable:
                new_ip_records.append([start_int, end_int, attrs])
                if merge_callback:
                    merge_callback(start_int, end_int, from_attrs, attrs)
            else:
                if output_not_covable:
                    print >> sys.stderr, "%s|%s [%s] <-> [%s]" % (self.int2ip(start_int), self.int2ip(end_int),
                                                             "|".join(from_attrs), "|".join(to_attrs))

        diff_func = _merge_record
        self.diff_ip_records(self._ip_records, ip_records, diff_func)

        self._ip_records = new_ip_records
        self._check_overlap()

        if compress_merged_result:
            self._compress_continues_ip_records()

        return new_ip_records
        
    def diff_with_ip_records(self, right_ip_records, outputf=sys.stdout, output_same=False):
        """Diff with another ip records file
        
        Args:
            right_ip_records, the ip records to diff with
            outputf, the file instance to output diff result
        Returns:
            diff_records, a list of diff record, a diff record is like:
            [start_ip_int, end_ip_int, left_attrs, right_attrs]
        Output format:
            startip    endip    ip-numbers    left-attributes    right-attributes
        """
        
        NOT_EXIST_RECORD = "not_include"
        diff_records = []
        
        def _diff(start_ip_int, end_ip_int, left_attrs, right_attrs):
            
            ip_nums = end_ip_int - start_ip_int + 1
            if left_attrs == right_attrs:
                if output_same:
                    attrs_str = "|".join(right_attrs)
                    if outputf:
                        print >> outputf, "\t".join([self.int2ip(start_ip_int), self.int2ip(end_ip_int), str(ip_nums), attrs_str, attrs_str])
                return 
            
            left = "|".join(left_attrs)
            if not left_attrs:
                left = NOT_EXIST_RECORD
            right = "|".join(right_attrs)
            if not right_attrs:
                right = NOT_EXIST_RECORD
                
            diff_record = [start_ip_int, end_ip_int, left_attrs, right_attrs]
            diff_records.append(diff_record)
            if outputf:
                print >> outputf, "\t".join([self.int2ip(start_ip_int), self.int2ip(end_ip_int), str(ip_nums), left, right])
        
        self.diff_ip_records(self._ip_records, right_ip_records, _diff)
        return diff_records
                
    def remove_ip_records(self, to_remove_ip_records):
        """
        Args:
            to_remove_ip_records, ip records to remove, which only care startip and endip in it
        Returns:
            ip records with ranges in `to_remove_ip_records` removed
        """
        filtered_records = []
        def _remove(start_ip_int, end_ip_int, left_attrs, right_attrs):
            if right_attrs:
                return
            
            filtered_records.append([start_ip_int, end_ip_int, left_attrs])
            
        self.diff_ip_records(self._ip_records, to_remove_ip_records, _remove)
        self._ip_records = filtered_records
        return filtered_records

    def count_ip(self):
        return self.stats["total_ip_num"]
        '''
        ip_count = 0
        for iprecord in self._ip_records:
            ip_count += (iprecord[1] - iprecord[0] + 1)
        return ip_count
        '''
    
    def get_ip_records(self):
        return self._ip_records

    def get_attr_names(self):
        return self._attr_names

    def query_ip(self, ipstr):
        '''
        input: ipstr as 202.118.224.29(well formatted IPaddress)
        return: ip attributes or []
        '''
        ipint = self.ip2int(ipstr)
        if ipint == None:
            return []
        
        record = self._binary_search(ipint)
        if record and len(record) > 2:
            return record[2]
        
        return []

    def _format_record(self, record):
        return "|".join([self.int2ip(record[0]), self.int2ip(record[1]), "|".join(record[2])])
    
    def dump_ip_records(self, outputf=sys.stdout):
        for record in self._ip_records:
            print >> outputf, self._format_record(record)


# tests
def test_parse_attributes_list():
    print parse_attributes_list("0,1,2")
    print parse_attributes_list("0,,2")
    print parse_attributes_list("0-2,3-8,10")

if __name__ == "__main__":
    test_parse_attributes_list()
    a = IPRecords("a.txt")
    b = IPRecords("b.txt")
    a.diff_with_ip_records(b.get_ip_records())
    a.merge_ip_records(b.get_ip_records())
    
    print "--merged--"
    a.dump_ip_records()
    
    c = IPRecords("c.txt")
    print "--to remove--"
    c.dump_ip_records()
    a.remove_ip_records(c.get_ip_records())
    print "--after removed--"
    a.dump_ip_records()
    
