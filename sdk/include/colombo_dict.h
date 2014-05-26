/*
 * public header of colombo iplibrary
 *
 * @author Tang Wentao(tangwentao@baidu.com)
 * @date 2014-01-24
 */
#ifndef __COLOMBO_DICT_H__
#define __COLOMBO_DICT_H__

#include <string>
#include <vector>
#include <map>

/**
 * a tool for mapping baidu-map citycode and GB-full-address and Colombo-short-address
 *
 * required dict format:
 *
 * ^full-prov:full-city:full-county\tshort-prov:short-city:short-county\tbaidu-map-citycode$
 *
 */

class ColomboDict
{
public:
    /**
     * load citycode colomb-addrs map dict
     * 
     * @param dict_file dict filename
     * @return 0 for success
     */
    int load_dict(const std::string &dict_file);

    typedef struct _sAddrInfo 
    {
        std::string full_addr;
        std::string short_addr;
    }AddrInfo;
    
    /**
     * get colombo addr info of specific citycode 
     *
     * @param citycode, input citycode
     * @param addr_info include: full address format by GB(GuoBiao) and short colombo address
     * @return 0 for success
     */
    int get_citycode_info(const std::string &citycode, AddrInfo &addr_info); 

private:
    std::string trim_string(const std::string &str);
    bool split(const std::string &s, char delim, std::vector<std::string> &elems);

private:
    std::map<std::string, AddrInfo> _citycode_addrs_dict;
};

#endif

