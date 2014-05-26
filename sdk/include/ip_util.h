/*
 * public header of colombo iplibrary
 *
 * @author Tang Wentao(tangwentao@baidu.com)
 * @date 2013-09-07
 */
#ifndef __COLOMBO_IP_UTIL_H__
#define __COLOMBO_IP_UTIL_H__

#include <stdint.h>
#include <string>

namespace colombo_iputil 
{

    /**
     * convert doted ip_str to ip_int
     *
     * @return true for succees
     */
    bool str2ipint(const std::string &ip_str, uint32_t &ip_int);

    /**
     * convert ip_int to doted ip_str
     * 
     * @return true for success
     */
    bool ipint2str(const uint32_t ip_int, std::string &ip_str);

}

#endif

