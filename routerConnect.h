/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file routerConnect.h
 * @author work(zhangyouchang@baidu.com)
 * @date 2014/05/18 21:31:15
 * @version $Revision$ 
 * @brief 
 *  
 **/


#ifndef  __ROUTERCONNECT_H_
#define  __ROUTERCONNECT_H_
#include "connect2RTMP.h"

class routerConnect{
	public:
		std::map<std::string, connect2RTMP*> connectIns;
		std::size_t totalUsedTime;
		std::size_t connect2RtmpFrequency;
		std::string oldDaemonVersion;
		std::string latestDaemonVersion;
		std::string oldFwVersion;
		std::string latestFwVersion;
		std::string location;
		std::string isp;
		std::string latestFwDaemonVersion;
		routerConnect(){
			this->totalUsedTime = 0;
			this->connect2RtmpFrequency = 0;
			this->oldDaemonVersion = "";
			this->latestDaemonVersion = "";
			this->oldFwVersion = "";
			this->latestFwVersion = "";
			this->isp = "";
			this->location = "";
			latestFwDaemonVersion = "";
			this->connectIns.clear();
		}
		~routerConnect(){
			this->connectIns.clear();
		}
		bool isDaemonUpdate(){
			return (oldDaemonVersion == latestDaemonVersion) ? false : true;
		}
		bool isFwUpdate(){
			return (oldFwVersion == latestFwVersion) ? false : true;
		}
		bool addConnectIns(std::string clientId, connect2RTMP* ins){
			std::map<std::string, connect2RTMP*>::iterator it;
			it = this->connectIns.find(clientId);
			if(it == connectIns.end()){
				this->connectIns.insert(std::map<std::string, connect2RTMP*>::value_type(clientId, ins));
				return true;
			}else{
				#ifdef DEBUG
					cout << clientId << " already in Map" << endl;
				#endif
				return false;
			}
		}
		void getRouterConnectInfo(){
			std::size_t i = 0;
			long connectTime = 0;
			long closeTime = 0;
			for(std::map<std::string, connect2RTMP*>::iterator it = connectIns.begin(); it != connectIns.end(); it++){
				cout << "getRouterConnectInfo --- i[" <<i<<"] connectTime[" << connectTime <<"] closeTime[" <<closeTime<<"]" << " (it->second)->time4connect[" << (it->second)->time4connect << "] (it->second)->time4close[" << (it->second)->time4close << "]" <<endl;
				connect2RtmpFrequency += 1;
				if(0 == i){
					oldDaemonVersion = (it->second)->daemonVersion;
					latestDaemonVersion = (it->second)->daemonVersion;
					oldFwVersion = (it->second)->fwVersion;
					latestFwVersion = (it->second)->fwVersion;
					totalUsedTime += (it->second)->connecttime;
					connectTime = (it->second)->time4connect;
					closeTime = (it->second)->time4close;
				}else{
					if((it->second)->time4connect < connectTime){
						oldDaemonVersion = (it->second)->daemonVersion;
						oldFwVersion = (it->second)->fwVersion;
						connectTime = (it->second)->time4connect;
						totalUsedTime += (it->second)->connecttime;
					}
					if((it->second)->time4connect > closeTime){
						latestDaemonVersion = (it->second)->daemonVersion;
						latestFwVersion = (it->second)->fwVersion;
						closeTime = (it->second)->time4close;
						totalUsedTime += (it->second)->connecttime;
					}
				}
				if("" == isp){
					isp = (it->second)->isp;
				}
				if("" == location){
					location = (it->second)->location;
				}
				i++;
			}
			latestFwDaemonVersion = latestFwVersion + "\t" + latestDaemonVersion;
		}
		void display(){
			//for(std::map<std::string, connect2RTMP*>::iterator it = connectIns.begin(); it != connectIns.end(); it++){
			//	cout << "ClientId : " << it->first << endl;
				cout << "totalUsedTime[" << totalUsedTime << "] connect2RtmpFrequency[" << connect2RtmpFrequency << "] oldDaemonVersion[" << oldDaemonVersion << "] latestDaemonVersion[" << latestDaemonVersion << "] oldFwVersion[" << oldFwVersion << "] latestFwVersion[" << latestFwVersion << "] isp["<< isp << "] location[" << location << "]" << endl;
			//}
		}

};
















#endif  //__ROUTERCONNECT_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
