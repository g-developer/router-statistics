/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file location.h
 * @author work(zhangyouchang@baidu.com)
 * @date 2014/05/18 12:52:05
 * @version $Revision$ 
 * @brief 
 *  
 **/


#ifndef  __LOCATION_H_
#define  __LOCATION_H_

#include <string>
#include <iostream>
#include <ctime>
#include <sys/time.h>
#include <cstdlib>
#include <unistd.h>
#include "ext_iplib_querier.h"
#include "ip_util.h"

class location{
	public:
		std::string dict;
		IPLibQuerier ipq;
		std::vector<std::string> required_attrs;
		std::map<std::string, std::string> attr_values;
		location(std::string d){
			dict = d;
			required_attrs.push_back("isp");
			required_attrs.push_back("prov");
			ipq.load(dict, "utf-8", required_attrs);
		}
		std::map<std::string, std::string> query(std::string ip){
			attr_values.clear();
			int ret = ipq.query(ip, required_attrs, attr_values);	
			if(0 == ret){
				for( std::map<std::string, std::string>::iterator it = attr_values.begin(); it != attr_values.end(); ++it ){
					std::cout << "zyc---IP : " << ip << it->first << "#" << it->second << "abc\n";
				}
			}
			//else{
			//	attr_values.insert(std::map<std::string, std::string>::value_type("未知", "未知"));
			//}
			return attr_values;
		}
};
















#endif  //__LOCATION_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
