#!/bin/sh
##!

##! download a file with username and password 
##!
##! @AUTHOR: tangwentao@baidu.com 
##! @VERSION: 1.0 update@2014-01-21
##! @DEP: curl 
##!

# set bash options
set -o pipefail

#
# download a url with auth 
#
download_url_with_auth()
{
    if [ $# -lt 4 ]; then
        echo "usage: download_url_with_auth url dstpath username password" >&2
        return 1
    fi

    local file_url=$1
    local dst_path=$2
    local username=$3
    local password=$4

    # do download
    touch $dst_path # curl -o may not create file when the file is empty
    http_code=$(curl -s -u "$username:$password" "$file_url" -o $dst_path -w %{http_code} 2>/dev/null) 
    if [ "$http_code" == "200" ]; then
        return 0
    elif [ "${http_code}" == "401" ]; then
        echo "authorized failed" >&2
        return 1
    fi
    echo "server return ${http_code} when downloading $file_url" >&2
    return 1
}

#
# check md5 then download
#
# @return 0 for download all success, 2 for current md5 is up to date, other return codes hint errors occur
#
check_md5_then_download()
{
    if [ $# -lt 7 ];then
        echo "usage: check_md5_then_download current_md5_file url url_params dstpath username password force_download" >&2
        return 1
    fi

    local current_md5_file=$1
    local file_url=$2
    local url_params=$3
    local dst_path=$4
    local username=$5
    local password=$6
    local force_download=$7

    # download md5
    download_url_with_auth ${file_url}.md5?${url_params} ${dst_path}.md5 $username $password || return 3 
    
    if [ ${force_download} -ne 1 -a -f ${current_md5_file} ]; then
        if diff ${current_md5_file} ${dst_path}.md5 ; then
            return 2
        fi  
    fi
    
    # download real content file
    download_url_with_auth ${file_url}?${url_params} ${dst_path} $username $password || return 4

    # check download file's md5
    download_md5=`md5sum ${dst_path} | cut -d" " -f1`
    deserved_md5=`cat ${dst_path}.md5`

    # - check if one or more md5-checksum is empty
    [ -n "${download_md5}" -a -n "${deserved_md5}" ] || return 5 

    # - check if md5sum is not the same
    [ "x${deserved_md5}" = "x${download_md5}" ] || return 5

    return 0
}

check_with_md5_file()
{
    if [ $# -lt 2 ]; then
        echo "usage:check_with_md5_file <file> <md5_file>" >&2
    fi
    file=$1
    md5_file=$2
    [ -f ${file} ] || return 1
    [ -f ${md5_file} ] || return 1
    md5=`cat ${md5_file}`
    file_md5=`md5sum ${file} | cut -d" " -f1`
    [ -n "${md5}" -a -n "${file_md5}" ] || return 1
    [ "${md5}" = "${file_md5}" ] || return 1
    return 0
}

