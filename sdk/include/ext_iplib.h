/*
 * common functions 
 *
 * @author Tang Wentao(tangwentao@baidu.com)
 * @date 2014-01-27
 */
#ifndef __COLOMBO_EXT_IPLIB_H__
#define __COLOMBO_EXT_IPLIB_H__

#include <stdint.h>
#include <string>
#include <map>
#include <vector>

namespace colombo_ext_iplib 
{

    /**
     * split string to items
     *
     * @note:
     * when delim is the last char, is ignored, but if is the first one, a empty item is split-out
     *
     * @examples:
     * "a,b,c" => "a","b","c"
     * "a,b,c," => "a","b","c"
     * ",a,b,c," => "","a","b","c"
     */
    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

    int parse_attr_list(const std::string &attr_list, std::vector<int> &attr_indexs, std::vector<std::string> &attr_names);

    int gen_ext_iplib(const std::string &iplib_file, const std::string &output_file, const std::string &attr_list, const std::string &generate_by);

    int strip_and_conv_ext_iplib(const std::string &iplib_file, const std::string &output_file, const std::string &encoding, const std::string &attr_list, const std::string &generate_by);

    int dump_ext_iplib(const std::string &ext_iplib_file, const std::string &dump_type);
}


#endif

