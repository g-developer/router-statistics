#!/bin/sh -
##!
##! colombo iplib updater
##!
##! @VERSION: 1.0
##! @AUTHOR: tangwentao@baidu.com 
##! @DEP: getopts@bash 
##!


# set bash options
set -o pipefail


# get current dir
canonicalize_path=`readlink -f $0`
my_dir=`dirname ${canonicalize_path}`

if [ $# -lt 3 ]; then
    echo "usage: <converted_config> <force_download> <download_reversion>" >&2
    exit 1
fi
converted_config=$1
force_download=$2
download_reversion=$3

# load basic config
. ${my_dir}/_base_config.sh
. ${my_dir}/_report.sh

# load app config
. ${converted_config}

# load download  util
. $my_dir/_download_util.sh

# download iplib
url_params="appkey=${appkey}&product=${product}&email=${email}&ip_attr_encoding=${ip_attr_encoding}&ip_attrs=${ip_attrs}"

updater_data_dir=$sdk_dir/$updater_workdir
current_md5_file=$updater_data_dir/current/current_iplib.md5
iplib_download_path=${UPDATER_TMP_DIR}/server_iplib.tar.gz

check_md5_then_download ${current_md5_file} "${URL_BASE}/data/ext_iplib/ext_iplib.${download_reversion}.tar.gz" ${url_params} ${iplib_download_path} "${appkey}" "${app_token}" $force_download

ret=$?
if [ $ret -eq 3 -o $ret -eq 4 ]; then
    report_error_and_exit "$URL_BASE/report?r" "$appkey" "$appkey" "download ext_iplib faild:$ret"
elif [ $ret -eq 5 ]; then
    report_error_and_exit "$URL_BASE/report?r" "$appkey" "$appkey" "check ext_iplib md5 failed"
elif [ $ret -eq 2 -a ${force_download} -ne 1 ]; then
    report_info "$URL_BASE/report?r" "$appkey" "$appkey" "ext_iplib is up to date"
    exit 0
fi

# convert iplib with user requirements 
server_iplib=${UPDATER_TMP_DIR}/server.iplib
tar xzf ${iplib_download_path} -O > $server_iplib || report_error_and_exit "$URL_BASE/report?r" "$appkey" "$appkey" "uncompress iplib failed"

converted_iplib=${UPDATER_TMP_DIR}/converted.iplib
#python ${my_dir}/ext_iplib_converter.py -i $server_iplib -o ${converted_iplib} -a ${ip_attrs} -e ${ip_attr_encoding} -b "$email $product" || report_error_and_exit "$URL_BASE/report?r" "$appkey" "$appkey" "convert iplib failed"

${my_dir}/ext_iplib_converter -i $server_iplib -o ${converted_iplib} -a ${ip_attrs} -e ${ip_attr_encoding} -b "$email $product" || report_error_and_exit "$URL_BASE/report?r" "$appkey" "$appkey" "convert iplib failed"

cp ${converted_iplib} ${UPDATER_TMP_DIR}/${output_file_prefix}.iplib

sh ${my_dir}/publish.sh ${updater_outputdir} ${UPDATER_TMP_DIR}/${output_file_prefix}.iplib N Y "+%Y%m%d%H%M%S" || report_error_and_exit "$URL_BASE/report?r" "$appkey" "$appkey" "save convert iplib failed"

# update current_md5_file
cp ${iplib_download_path}.md5 ${current_md5_file} || report_error_and_exit "$URL_BASE/report?r" "$appkey" "$appkey" "update current iplib md5 failed"

report_info "$URL_BASE/report?${url_params}" "$appkey" "$appkey" "update iplib success"

# clean backup files
current_files_count=`find ${updater_outputdir} -type f -name "*.md5"|wc -l`
if [ $current_files_count -ge ${backup_days} ];then
    echo "clean old data files..."
    find ${updater_outputdir} -type f -name "*.iplib.*" -ctime +${backup_days} | xargs -i rm -rfv {} 
fi

