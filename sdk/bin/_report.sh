#!/bin/sh -
##!
##! tools to report error 
##!
##! @AUTHOR: tangwentao@baidu.com 
##! @VERSION: 1.0.0 2014-02-20
##!


#
# report_error
#
report_error_and_exit()
{
    if [ $# -lt 4 ]; then
        echo 'usage: report_error_and_exit "url" "appkey" "app_token" "msg"' >&2
        exit 1
    fi
    msg=`echo $4| sed 's/[ \t]/_/g'`
    curl -s -u "$2:$3" "$1&msg=$msg" &>/dev/null
    echo "[Error] $4" >&2
    exit 1
}


# 
# report info
#
report_info()
{
    if [ $# -lt 4 ]; then
        echo 'usage: report_info "url" "appkey" "app_token" "msg"' >&2
        return 1
    fi
    msg=`echo $4| sed 's/[ \t]/_/g'`
    curl -s -u "$2:$3" "$1&msg=$msg" &>/dev/null
    echo "[Info] $4" >&2
    return 0
}

