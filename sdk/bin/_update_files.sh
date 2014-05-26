#!/bin/sh
##!

##! update files from server 
##!
##! @AUTHOR: tangwentao@baidu.com 
##! @VERSION: 1.0.0 update@2014-01-23
##!

# set bash options
set -o pipefail

# get current dir
canonicalize_path=`readlink -f $0`
my_dir=`dirname ${canonicalize_path}`

if [ $# -lt 1 ]; then
    echo "usage: <converted_config>" >&2
    exit 1
fi
converted_config=$1

# load basic config
. ${my_dir}/_base_config.sh
. ${my_dir}/_report.sh

# load app config
. ${converted_config}

. ${my_dir}/_download_util.sh

current_state_dir=$sdk_dir/$updater_workdir/current
#usage: check_md5_then_download current_md5_file url url_params dstpath username password force_download

# 1. get to updating file list
force_download=1
url_params="appkey=${appkey}&product=${product}&email=${email}"

check_md5_then_download ${current_state_dir}/updater_file_list.txt.md5 ${URL_BASE}/updater/updater_file_list.txt ${url_params} ${UPDATER_TMP_DIR}/updater_file_list.txt "${appkey}" "${app_token}" ${force_download} 

ret=$?

if [ $ret -eq 3 -o $ret -eq 4 ]; then
    report_error_and_exit "$URL_BASE/report?r" "$appkey" "$appkey" "download file faild:$ret"
elif [ $ret -eq 5 ]; then
    report_error_and_exit "$URL_BASE/report?r" "$appkey" "$appkey" "check md5 failed:updater_file_list.txt" 
elif [ $ret -eq 0 ]; then
    cp ${UPDATER_TMP_DIR}/updater_file_list.txt ${current_state_dir}/updater_file_list.txt 
fi

# 2. do update files
while read url dst_path
do 
    file_basename=`basename $dst_path`
    download_path=${UPDATER_TMP_DIR}/${file_basename}
    force_download=0
    [ -f ${sdk_dir}/${dst_path} ] || force_download=1

    if [ $force_download -eq 1 ] ; then
        mkdir -p `dirname ${sdk_dir}/${dst_path}`
    fi

    check_with_md5_file ${sdk_dir}/${dst_path} ${current_state_dir}/${file_basename}.md5 || force_download=1

    # do download 
    check_md5_then_download ${current_state_dir}/${file_basename}.md5 ${URL_BASE}/${url} ${url_params} ${download_path} "${appkey}" "${app_token}" ${force_download}
    ret=$?

    if [ $ret -eq 2 ]; then
        report_info "$URL_BASE/report?r" "$appkey" "$appkey" "${dst_path} is up to date"
        continue
    elif [ $ret -eq 0 ]; then
        report_info "$URL_BASE/report?r" "$appkey" "$appkey" "download ${dst_path} success"
        cp ${download_path} ${sdk_dir}/${dst_path} && cp ${download_path}.md5 ${current_state_dir}/${file_basename}.md5
        continue
    elif [ $ret -eq 5 ]; then
        report_error_and_exit "$URL_BASE/report?r" "$appkey" "$appkey" "check md5 failed:${dst_path}"
    else
        report_error_and_exit "$URL_BASE/report?r" "$appkey" "$appkey" "update ${dst_path} failed"
    fi

done < ${current_state_dir}/updater_file_list.txt

