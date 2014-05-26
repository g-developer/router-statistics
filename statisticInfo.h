/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/



/**
 * @file statisticInfo.h
 * @author work(zhangyouchang@baidu.com)
 * @date 2014/05/22 09:17:56
 * @version $Revision$ 
 * @brief 
 *  
 **/


#ifndef  __STATISTICINFO_H_
#define  __STATISTICINFO_H_

#include <iostream>
#include <fstream>
#include "routerConnect.h"

/*
   struct range{
   public:
   size_t min;
   size_t max;
   range(){
   min = 0;
   max = 0;
   }
   range(size_t a, size_t b){
   min = a;
   max = b;
   }
   };
   */

class statisticsInfo{
	public:
		std::map<std::string, int> daemonUpdate;
		std::map<std::string, int> fwUpdate;
		std::map<std::string, int> location;
		std::map<std::string, int> isp;
		std::map<std::string, int> connectFrequency;
		std::map<std::string, int> daemonDistribute;
		std::map<std::string, int> fwDistribute;
		std::map<std::string, int> fw_DaemonDistribute;
		std::map<std::string, int> totalUseTimeDistribute;
		std::map<std::string, int> singleUseTimeDistribute;
		size_t dailyAlive;

		statisticsInfo(){
		}

		std::string degree(size_t num, int type = 0){
			std::string desc;
			int level;
			//std::map<std::string, size_t> *tmp = new std::map<std::string, size_t>();
			if(0 == type){
				if(10 > num){
					level = 1;
					desc = "连接RTMP次数[0,10)";
				}else if(10 <= num && num < 50){
					level = 2;
					desc = "连接RTMP次数[10,50)";
				}else if(50 <= num && num < 100){
					level = 3;
					desc = "连接RTMP次数[50,100)";
				}else if(100 <= num && num < 200){
					level = 4;
					desc = "连接RTMP次数[100,200)";
				}else if(200 <= num && num < 500){
					level = 5;
					desc = "连接RTMP次数[200,500)";
				}else if(500 <= num && num < 1000){
					level = 6;
					desc = "连接RTMP次数[500,1000)";
				}else{
					level = 7;
					desc = "连接RTMP次数1000以上";
				}
			}else{
				if(num >= 28800){
					level = 1;
					desc = "路由使用时间超过8小时";
				}else if(14400 <= num && num < 28800){
					level = 2;
					desc = "路由使用时间[4h,8h)";
				}else if(7200 <= num && num < 14400){
					level = 3;
					desc = "路由使用时间[2h,4h)";
				}else if(3600 <= num && num < 7200){
					level = 4;
					desc = "路由使用时间[1h,2h)";
				}else if(1800 <= num && num < 3600){
					level = 5;
					desc = "路由使用时间[0.5h,1h)";
				}else if(600 <= num && num < 1800){
					level = 6;
					desc = "路由使用时间[10m,0.5h)";
				}else{
					level = 7;
					desc = "路由使用时间10分钟以内";
				}
			}

			//map->insert(std::map<std::string, size_t>::value_type(desc, level));
			return desc;
		}

		int doStatistics(routerConnect* rc){
			if(NULL == rc){
				return -1;
			}
			location[rc->location] += 1;
			isp[rc->isp] += 1;
			daemonDistribute[rc->latestDaemonVersion] += 1;
			fwDistribute[rc->latestFwVersion] += 1;
			if(rc->isDaemonUpdate()){
				daemonUpdate[rc->oldDaemonVersion + "-" + rc->latestDaemonVersion] += 1;
			}
			if(rc->isFwUpdate()){
				fwUpdate[rc->oldFwVersion + "-" + rc->latestFwVersion] += 1;
			}
			std::string tmp;
			tmp = degree(rc->totalUsedTime, 1);
			totalUseTimeDistribute[tmp] += 1;
			tmp = degree(rc->connect2RtmpFrequency);
			connectFrequency[tmp] += 1;
			fw_DaemonDistribute[rc->latestFwDaemonVersion] += 1;
			for(std::map<std::string, connect2RTMP*>::iterator it = rc->connectIns.begin(); it != rc->connectIns.end(); it++){
				tmp = degree(it->second->connecttime, 1);
				singleUseTimeDistribute[tmp] += 1;
			}

			return 0;
		}

		void dump2File(std::string outputDir){
			std::string file_isp = outputDir + "/isp";
			std::ofstream of_isp;
			of_isp.open(file_isp.c_str(), ios::out);
			size_t None = 0;
			for(std::map<std::string, int>::iterator it = isp.begin(); it != isp.end(); it++){
				if("None" == it->first || (it->first).empty() || "" == it->first || " " == it->first){
					None += 1;
				}else{
					of_isp << it->first << "\t" << it->second << endl;
				}
			}
			if(0 != None){
					of_isp << "未知" << "\t" << None << endl;
			}
			of_isp.close();


			std::ofstream of_location;
			None = 0;
			std::string file_location = outputDir + "/location";
			of_location.open(file_location.c_str(), ios::out);
			cout << "Location Distribute:" << endl;
			for(std::map<std::string, int>::iterator it = location.begin(); it != location.end(); it++){
				if("None" == it->first || (it->first).empty() || "" == it->first || " " == it->first){
					None += 1;
				}else{
					of_location << it->first << "\t" << it->second << endl;
				}
			}
			if(0 != None){
					of_location << "未知" << "\t" << None << endl;
			}
			of_location.close();

			std::ofstream of_fwDistribute;
			std::string file_fwDistribute = outputDir + "/fw_version_distribution";
			of_fwDistribute.open(file_fwDistribute.c_str(), ios::out);
			for(std::map<std::string, int>::iterator it = fwDistribute.begin(); it != fwDistribute.end(); it++){
				of_fwDistribute << it->first<<"\t"<<it->second<<endl;
			}
			of_fwDistribute.close();

			std::ofstream of_daemonDistribute;
			std::string file_daemonDistribute = outputDir + "/daemon_version_distribution";
			of_daemonDistribute.open(file_daemonDistribute.c_str(), ios::out);
			for(std::map<std::string, int>::iterator it = daemonDistribute.begin(); it != daemonDistribute.end(); it++){
				of_daemonDistribute << it->first<< "\t" <<it->second <<endl;
			}
			of_daemonDistribute.close();

			std::ofstream of_daemonUpdateDistribute;
			std::string file_daemonUpdateDistribute = outputDir + "/daemon_version_upate_distribution";
			of_daemonUpdateDistribute.open(file_daemonUpdateDistribute.c_str(), ios::out);
			for(std::map<std::string, int>::iterator it = daemonUpdate.begin(); it != daemonUpdate.end(); it++){
				of_daemonUpdateDistribute << it->first<<"\t"<<it->second<<endl;
			}
			of_daemonUpdateDistribute.close();

			std::ofstream of_fwUpdateDistribute;
			std::string file_fwUpdateDistribute = outputDir + "/fw_version_upate_distribution";
			of_fwUpdateDistribute.open(file_fwUpdateDistribute.c_str(), ios::out);
			for(std::map<std::string, int>::iterator it = fwUpdate.begin(); it != fwUpdate.end(); it++){
				of_fwUpdateDistribute << it->first<<"\t"<<it->second <<endl;
			}
			of_fwUpdateDistribute.close();

			std::ofstream of_online_frequency_distribution;
			std::string file_online_frequency_distribution = outputDir + "/online_frequency_distribution";
			of_online_frequency_distribution.open(file_online_frequency_distribution.c_str(), ios::out);
			for(std::map<std::string, int>::iterator it = connectFrequency.begin(); it != connectFrequency.end(); it++){
				of_online_frequency_distribution << it->first<<"\t"<<it->second<<endl;
			}
			of_online_frequency_distribution.close();

			std::ofstream of_total_time_used;
			std::string file_total_time_used = outputDir + "/total_time_used";
			of_total_time_used.open(file_total_time_used.c_str(), ios::out);
			for(std::map<std::string, int>::iterator it = totalUseTimeDistribute.begin(); it != totalUseTimeDistribute.end(); it++){
				of_total_time_used << it->first<<"\t"<<it->second <<endl;
			}
			of_total_time_used.close();
			
			std::ofstream of_one_time_used;
			std::string file_one_time_used = outputDir + "/one_time_used";
			of_one_time_used.open(file_one_time_used.c_str(), ios::out);
			for(std::map<std::string, int>::iterator it = singleUseTimeDistribute.begin(); it != singleUseTimeDistribute.end(); it++){
				of_one_time_used << it->first<<"\t"<<it->second <<endl;
			}

			std::ofstream of_fw_daemon_version_distribution;
			std::string file_fw_daemon_version_distribution = outputDir + "/fw_daemon_version_distribution";
			of_fw_daemon_version_distribution.open(file_fw_daemon_version_distribution.c_str(), ios::out);
			for(std::map<std::string, int>::iterator it = fw_DaemonDistribute.begin(); it != fw_DaemonDistribute.end(); it++){
				of_fw_daemon_version_distribution << it->first << "\t" << it->second << endl;
			}
			of_fw_daemon_version_distribution.close();
		}

		void display(){
			cout << "daily ALive : " << dailyAlive << endl;
			cout << "Isp Distribute:" << endl;
			for(std::map<std::string, int>::iterator it = isp.begin(); it != isp.end(); it++){
				cout <<"\t "<< it->first<<"["<<it->second<<"]"<<endl;
			}
			cout << "Location Distribute:" << endl;
			for(std::map<std::string, int>::iterator it = location.begin(); it != location.end(); it++){
				cout <<"\t "<< it->first<<"["<<it->second<<"]"<<endl;
			}
			cout << "Fw Distribute:" << endl;
			for(std::map<std::string, int>::iterator it = fwDistribute.begin(); it != fwDistribute.end(); it++){
				cout <<"\t "<< it->first<<"["<<it->second<<"]"<<endl;
			}
			cout << "Daemon Distribute:" << endl;
			for(std::map<std::string, int>::iterator it = daemonDistribute.begin(); it != daemonDistribute.end(); it++){
				cout <<"\t "<< it->first<<"["<<it->second<<"]"<<endl;
			}
			cout << "Daemon Update Distribute:" << endl;
			for(std::map<std::string, int>::iterator it = daemonUpdate.begin(); it != daemonUpdate.end(); it++){
				cout <<"\t "<< it->first<<"["<<it->second<<"]"<<endl;
			}
			cout << "FW Update Distribute:" << endl;
			for(std::map<std::string, int>::iterator it = fwUpdate.begin(); it != fwUpdate.end(); it++){
				cout <<"\t "<< it->first<<"["<<it->second<<"]"<<endl;
			}
			cout << "connectFrequency Distribute:" << endl;
			for(std::map<std::string, int>::iterator it = connectFrequency.begin(); it != connectFrequency.end(); it++){
				cout <<"\t "<< it->first<<"["<<it->second<<"]"<<endl;
			}
			cout << "totalUseTimeDistribute Distribute:" << endl;
			for(std::map<std::string, int>::iterator it = totalUseTimeDistribute.begin(); it != totalUseTimeDistribute.end(); it++){
				cout <<"\t "<< it->first<<"["<<it->second<<"]"<<endl;
			}
			cout << "singleUseTimeDistribute Distribute:" << endl;
			for(std::map<std::string, int>::iterator it = singleUseTimeDistribute.begin(); it != singleUseTimeDistribute.end(); it++){
				cout <<"\t "<< it->first<<"["<<it->second<<"]"<<endl;
			}
		}
};















#endif  //__STATISTICINFO_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
