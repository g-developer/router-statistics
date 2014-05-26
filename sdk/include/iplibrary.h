/*
 * public header of colombo iplibrary
 *
 * @author Tang Wentao(tangwentao@baidu.com)
 * @date 2013-09-07
 */
#ifndef __COLOMBO_IP_LIBRARY_H__
#define __COLOMBO_IP_LIBRARY_H__

#include <stdint.h>
#include <vector>
#include <string>
#include <map>
#include <ostream>

/*
* load verified Colombo formatted ip library, and query it
* NOTICE: there is no full VERIFY of the loaded ip library
*/

class IPLibrary
{
public:
    /**
     * IP attributes indexs
     */
    typedef std::vector<int> IPAttrIndexs;

    /**
     * IP attributes list
     */
    typedef std::vector<const char *> IPAttrs;

    /**
     * one IP Record
     */
    typedef struct IPRecord
    {
        IPRecord()
            : start_ip(0)
            , end_ip(0)
        {
        }
        uint32_t start_ip;
        uint32_t end_ip;
        IPAttrs attrs;

        friend bool operator==(const IPRecord &lhs, const IPRecord &rhs)
        {
            if ( lhs.start_ip == rhs.start_ip && lhs.end_ip == rhs.end_ip && lhs.attrs == rhs.attrs )
            {
                return true;
            }
            return false;
        }
        friend bool operator!=(const IPRecord &lhs, const IPRecord &rhs)
        {
            return !(lhs == rhs);
        }
    }IPRecord;
    typedef std::vector<IPRecord> IPRecords;


    /**
     * @param const std::vector<int> attr_indexs, indexs of attrs, start from 0
     * @param merge_loaded_records, records will be merged according to select attrs, 
     *      which will improve query performance, RECOMMEND use default value
     */
    IPLibrary(const IPAttrIndexs &attr_indexs=IPAttrIndexs(), const bool merge_loaded_recods=true);
    ~IPLibrary() {};

	/**
     * load iplib file
     * @param const char * iplib, iplib file
     */
	int load(const char *iplib);

    /**
     * query failed will return a empty one
     */
    const static IPRecord EMPTY_RECORD;

    /**
     * query ip
     * @return the record with ip and its attrs in, if not found, IPLibrary::EMPTY_RECORD is returned
     */
	const IPRecord &query(const std::string &ip) const;
	const IPRecord &query(const uint32_t ip) const;

	/**
     * convert between "x.x.x.x" and host-byte order integer 
     * @param string ip_str
     * @param uint32_t ip_int: host-byte order
     * @return true for success, false for failed
     */
	bool str2ipint(const std::string &ip_str, uint32_t &ip_int) const;
    
    bool ipint2str(const uint32_t ip_int, std::string &ip_str) const;

    /**
     * dump stored iprecords to os
     */
    void dump(std::ostream &os) const;


    /**
     * get total ip count
     */
    inline uint64_t get_total_ip_count() const 
    {
        return _total_ip_count;
    }

	inline const IPRecords &get_all_records() const 
    {
        return _iprecords;
    }

    /*
     * merge continues records
     */
    bool merge_records(IPRecords &iprecords, uint64_t &ip_count);

private:
	typedef std::map<std::string, std::string> WordDict; 
    typedef enum _eParseLineStatus{ RECORD_LINE, COMMENT_LINE, EMPTY_LINE, PARSE_ERROR } ParseLineStatus;

	/*
	* @param const char *line, null character terminated 
	*/
	int parse_line(const char *line, IPRecord &record);

	/*
	* trim starting and ending spaces
	*/
	std::string trim_string(const std::string &str);

    /*
     * select target attrs, and save them to attr string dict
     * @param splited_items splited line items
     * @param target_attrs 
     */
	bool filter_attrs(std::vector<std::string> &splited_items, IPAttrs &target_attrs);

    bool is_attrs_equal(const IPAttrs &lhs, const IPAttrs &rhs);

    std::string format_iprecord(const IPRecord &record) const;


private:
	IPAttrIndexs _attr_indexes;
    bool _merge_loaded_recods;
    uint64_t _total_ip_count;
	IPRecords _iprecords;
	WordDict _words_dict;
};

#endif

