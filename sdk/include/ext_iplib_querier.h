/**
 *
 * Colombo IP Library query interface
 * @author: Tang Wentao(tangwentao@baidu.com)
 * @date: 2014-01-24
 *
 */
#ifndef __COLOMBO_EXT_IPLIB_QUERIER__
#define __COLOMBO_EXT_IPLIB_QUERIER__

#include <string>
#include <vector>
#include <map>
#include "iplibrary.pb.h"

class IPLibQuerier
{
public:
    IPLibQuerier() 
        : _loaded(false) 
    {
    }

    /**
     * load iplib
     *
     * @param ipfile the iplib file to load
     * @param encoding expect encoding
     * @attrs expect attrs to be queried
     * @return 0 for success
     */
    int load(const std::string &ipfile, const std::string &encoding, const std::vector<std::string> &attrs);

    /**
     * query ip
     *
     * @param ip the ip_int or doted ip to query
     * @param attrs to query
     * @param attr_values query result
     * @return 0 for success
     */
    int query(const uint32_t &ip, const std::vector<std::string> &attrs, std::map<std::string, std::string> &attr_values) const;
    int query(const std::string &ip, const std::vector<std::string> &attrs, std::map<std::string, std::string> &attr_values) const;

    typedef struct IPRecord{
        uint32_t start_ip;
        uint32_t end_ip;
        std::map<std::string,std::string> attrs;
    }IPRecord;

    /**
     * get all ip record size
     * return 0 for failure or if the records is really empty, other non-zero value for real size
     */
    int records_size() const;

    uint32_t total_ip_count() const;

    /**
     * get the record with index
     *
     * @return 0 for success
     */
    int get_record(int index, IPRecord &record);

    /**
     * @see ip_util.h
     */
    bool str2ipint(const std::string &ip_str, uint32_t &ip_int);
    bool ipint2str(const uint32_t ip_int, std::string &ip_str);

private:
    bool _loaded;
    ::colombo::IPLibrary _iplib;
    std::map<std::string /*attr_name*/, int32_t /*index*/> _attr_name_index;
};

#endif // __COLOMBO_EXT_IPLIB_QUERIER__

