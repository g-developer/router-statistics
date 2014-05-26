/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file connect2RTMP.h
 * @author work(zhangyouchang@baidu.com)
 * @date 2014/05/17 18:26:26
 * @version $Revision$ 
 * @brief 
 *  
 **/


#ifndef  __CONNECT2RTMP_H_
#define  __CONNECT2RTMP_H_

#include <string>

class connect2RTMP{
	public:
		long time4connect;
		long time4close;
		std::string ip;
		std::string isp;
		std::string location;
		long connecttime;
		std::string daemonVersion;
		std::string fwVersion;
		connect2RTMP(){
			time4connect = -1;
			time4close = -1;
			ip = "";
			location = "";
			connecttime = -1;
			isp = "";
		}
		void setTime4connect(long time){
			time4connect = time;
			if(0 != this->time4close){
				this->connecttime = (time4close - time4connect);
			}
		}
		void setTime4close(long time){
			time4close = time;
			if(0 != this->time4connect){
				this->connecttime = (time4close - time4connect);
			}
		}
		void setIp(std::string ip){
			this->ip = ip;
		}
		void setLocaion(std::string loc){
			this->location = loc;
		}
		void setIsp(std::string isp){
			this->isp = isp;
		}
		void setDaemonVersion(std::string daemonVersion){
			this->daemonVersion = daemonVersion;
		}
		void setFwVersion(std::string fwVersion){
			this->fwVersion = fwVersion;
		}
		bool isIpNotReady(){
			return ("" == ip || "-" == ip ) ? true : false;
		}
		bool isIspNotReady(){
			return ("None" == isp || "" == isp || "-" == isp || "未知" == isp) ? true : false;
		}
		bool isLocationNotReady(){
			return ("None" == location || "" == location || "-" == location || "未知" == location) ? true : false;
		}
		void display(){
			std::cout<< "Router come ["<<time4connect<<"] Router leave ["<<time4close<<"] Router Used ["<< connecttime << "] ip["<<ip<<"] isp["<<isp<<"] loc[" << location<< "]" << std::endl;
		}
};














#endif  //__CONNECT2RTMP_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
