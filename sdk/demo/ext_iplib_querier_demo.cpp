#include <iostream>
#include <ctime>
#include <sys/time.h>
#include <cstdlib>
#include <unistd.h>
#include "ext_iplib_querier.h"
#include "ip_util.h"

int do_query(IPLibQuerier &iplib, const std::string&ip, bool output=false)
{
    std::map<std::string, std::string> attr_values;
    std::vector<std::string> required_attrs;
    required_attrs.push_back("isp");
    required_attrs.push_back("prov");
    int ret = iplib.query(ip, required_attrs, attr_values); 
    if ( ret < 0 ) 
    {
        return -1;
    }

    if ( output )
    {
        for( std::map<std::string, std::string>::iterator it = attr_values.begin(); it != attr_values.end(); ++it )
        {
            std::cout << it->first << ":" << it->second << "\n";
        }
    }

    return 0;
}

int query(const std::string &iplib_file, const std::string &ip)
{
    IPLibQuerier ipq;
    std::vector<std::string> required_attrs;
    required_attrs.push_back("isp");
    required_attrs.push_back("prov");

    int ret =  ipq.load(iplib_file, "gbk", required_attrs); 

    if ( ret >= 0 ) 
    {
        std::cout << "quering " << ip << "\n";
        return do_query(ipq, ip, true);
    }
    return -1;
}

int main(int argc, char*argv[])
{
    if ( argc < 3 )
    {
        std::cerr << "Usage: " << argv[0] << " <iplib_file> <ip>\n";
        exit(EXIT_FAILURE);
    }

    exit(query(argv[1], argv[2]));
}

