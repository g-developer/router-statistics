/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/



/**
 * @file t.cpp
 * @author work(zhangyouchang@baidu.com)
 * @date 2014/05/18 13:10:09
 * @version $Revision$ 
 * @brief 
 *  
 **/
#include <hash_map.h>
#include <fstream>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include "location.h"
#include "connect2RTMP.h"
#include "routerConnect.h"
#include "statisticInfo.h"


//注意：当字符串为空时，也会返回一个空字符串
void split(std::string& s, std::string& delim,std::vector< std::string >* ret)
{
	size_t last = 0;
	size_t index=s.find_first_of(delim,last);
	while (index!=std::string::npos)
	{
		ret->push_back(s.substr(last,index-last));
		last=index+1;
		index=s.find_first_of(delim,last);
	}
	if (index-last>0)
	{
		ret->push_back(s.substr(last,index-last));
	}
}

int main(int argc, char** argv){

	int result;
	opterr = 0;

	//test -s day/hour -i /home/work/zhangyouchang/tongji/rtmp/alive/ -o /home/work/zhangyouchang/tongji/rtmp/crm_data -t 20140416 -d /home/work/zhangyouchang/tongji/rtmp/data/

	std::string style;
	std::string input;
	std::string output;
	std::string day;
	std::string dict;
	while( (result = getopt(argc, argv, "s:i:o:t:d:")) != -1 )
	{
		switch(result)
		{
			case 's':
				style = optarg;
				break;
			case 'i':
				input = optarg;
				break;
			case 'o':
				output = optarg;
				break;
			case 't':
				day = optarg;
				break;
			case 'd':
				dict = optarg;
				break;
			default:
				printf("参数错误，请重新输入:");
				exit(0);
				break;
		}
	}

	time_t t_start;
	time(&t_start);
	location l = location(dict);
	std::map<std::string, std::string> loc;


	/*
	   connect2RTMP c2r = connect2RTMP();
	   c2r.setTime4connect(1400166000017);
	   c2r.setTime4close(1400175973092);
	   c2r.setIp(ip);
	   loc = l.query(ip);
	   for(std::map<std::string, std::string>::iterator it = loc.begin(); it !=loc.end(); it++){
	   if("prov" == it->first){
	   c2r.setLocaion(it->second);
	   }else{
	   c2r.setIsp(it->second);
	   }
	   }
	   c2r.display();
	   */

	/*
	   struct tm *ptr; 
	   char str[32];
	   time_t lt=time(NULL);
	   lt -= 86400;
	   ptr=localtime(&lt);
	   strftime(str,32,"%Y%m%d",ptr); 
	   std::string yesterday = str;
	   std::string logfile = "/home/work/zhangyouchang/tongji/rtmp/alive/" + yesterday +"/alive_list";
	   */
	std::string logfile = input + "/" + day +"/alive_list";
	std::ifstream file;
	file.open(logfile.c_str(), std::ios::in);
	int i = 0;
	char line[1024];
	memset(line, 0, 1024);
	if (!file.is_open ()){
		std::cout << __LINE__ << ":" <<"打开文件失败[" << logfile << "]" << std::endl;
		return 1;
	}else{
		std::cout << __LINE__ << ":" <<"打开文件成功[" << logfile << "]" << std::endl;
	}
	std::map<std::string, routerConnect*> connectInfo;
	std::vector<std::string> tmp;
	connect2RTMP *c2r_tmp;
	routerConnect *rc_tmp;
	statisticsInfo si = statisticsInfo();
	while(!file.eof()){
		//if(0 == i % 10000){
		cout << "Run  : " << i << endl;
		//}
		file.getline(line,1024,'\n');

		if(0 == strcmp("", line)){
			continue;
		}else{
			tmp.clear();
			std::string lineTmp = line;
			std::string sp = "\t";
			split(lineTmp, sp, &tmp);
			int tmpSize = tmp.size();
			if(10 != tmpSize){
				continue;
			}
			std::string time = tmp[0];
			std::string clientId = tmp[3];
			std::string devId = tmp[5];
			std::string fwVersion = tmp[6];
			std::string daemonVersion = tmp[7];
			std::string ip = tmp[9];

			long time_x = atol(time.c_str()) / 1000;

			cout << "time["<< time << "] clientId[" << clientId << "] devId[" << devId << "] fwVersion[" << fwVersion <<"] daemonVersion[" << daemonVersion << "] ip[" << ip << "]" << endl;

			std::map<std::string, routerConnect*>::iterator it;
			it = connectInfo.find(devId);
			std::map<std::string, connect2RTMP*>::iterator it2;
			if(connectInfo.end() == it){
				cout << "tmp[4] : " << tmp[4] << endl;
				c2r_tmp = new connect2RTMP();
				rc_tmp = new routerConnect();
				cout << "Fuck---c2r_tmp" << c2r_tmp << "--rc_tmp---" << rc_tmp<< endl;
			}else{
				rc_tmp = it->second;
				it2 = rc_tmp->connectIns.find(clientId);
				if(rc_tmp->connectIns.end() == it2){
					c2r_tmp = new connect2RTMP();
				}else{
					c2r_tmp = it2->second;
				}
			}

			if("OK" == tmp[4]){
				if(-1 == c2r_tmp->time4connect){
					c2r_tmp->setTime4connect(time_x);
				}
			}else{
				if(-1 == c2r_tmp->time4close){
					c2r_tmp->setTime4close(time_x);
				}
			}
			if("" == c2r_tmp->daemonVersion){
				c2r_tmp->setDaemonVersion(daemonVersion);
			}
			if("" == c2r_tmp->fwVersion){
				c2r_tmp->setFwVersion(fwVersion);
			}
			if(c2r_tmp->isIpNotReady()){
				cout << "zyc---tmp[9] : " << tmp[9] << endl;
				c2r_tmp->setIp(ip);
				loc = l.query(ip);
				for(std::map<std::string, std::string>::iterator it = loc.begin(); it !=loc.end(); it++){
					if("prov" == it->first){
						if(c2r_tmp->isLocationNotReady()){
							c2r_tmp->setLocaion(it->second);
						}
					}else{
						if(c2r_tmp->isIspNotReady()){
							c2r_tmp->setIsp(it->second);
						}
					}
				}
			}
			rc_tmp->addConnectIns(clientId, c2r_tmp);

			connectInfo.insert(std::map<std::string, routerConnect*>::value_type(devId, rc_tmp));
			c2r_tmp->display();
		}
		i++;
	}
	file.close();
	for(std::map<std::string, routerConnect*>::iterator it = connectInfo.begin(); it!= connectInfo.end(); it++){
		cout << "devId : " << it->first << endl;
		it->second->getRouterConnectInfo();
		it->second->display();
		si.doStatistics(it->second);
	}
	si.display();
	std::string outputDir = output + "/" + "/" + day;
	DIR* dirp;
	dirp = opendir(outputDir.c_str());
	if(NULL == dirp){
		mkdir(outputDir.c_str(), 0755);
	}
	si.dump2File(outputDir);
	std::ofstream of_day_alive;
	std::string file_day_alive;
	file_day_alive = outputDir + "/day_alive";
	of_day_alive.open(file_day_alive.c_str(), ios::out);
	of_day_alive << day << "\t" <<connectInfo.size() << endl;
	of_day_alive.close();
	time_t t_end;
	time(&t_end);
	//cout << "Total Spend ----- " << (t_start - t_end) << " Size : " << connectInfo.size() << endl;
	return 1;
}

















/* vim: set ts=4 sw=4 sts=4 tw=100 */
