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
sdk_dir=`dirname ${my_dir}`


#--------------- global variable --------------
typeset -r VERSION=1.0.0
typeset -r DEFUALT_CONF_FILE=`readlink -f $my_dir/../conf/updater.conf`

#
# show_usage
#
show_usage()
{
    e=$(printf "\e")
    cat << EOF

    Colombo iplibrary updater 

$e[1mSYNOPSIS$e[0m
    $0 [-h|-v] [-c $e[4mconf_file$e[0m] [-u] [-f] [-r reversion]

$e[1mDESCRIPTION$e[0m

    -h show this help

    -v show version

    -u do update
    -f force update without check local md5

    -r reversion, specific the reversion of iplib to download

    -c $e[4mconf_file$e[0m
       default: $DEFUALT_CONF_FILE 

$e[1mVERSION:$e[0m
    $VERSION

EOF
}

show_version()
{
    echo $VERSION
}

_conf_file=$DEFUALT_CONF_FILE
_do_update=0
_force_update=0
_download_reversion=latest

parse_args()
{
    if [ $# -lt 1 ]; then
        show_usage;
        exit 0
    fi

    while getopts :hvufr:c: arg
    do
        case $arg in
        c)
            _conf_file=$OPTARG
            ;;
        h)
            show_usage 
            exit 0
            ;;
        u)
            _do_update=1
            ;;
        f)
            _force_update=1
            ;;
        r)
            _download_reversion=$OPTARG
            ;;
        v)
            show_version
            exit 0
            ;;

        :) 
            echo "$0: -$OPTARG need one argument." >&2
            exit 1
            ;;

        \?) 
            echo "Invalid option -$OPTARG." >&2
            exit 1
            ;;
        esac
    done
}


#
# main routine
#
# 5 steps
#

# check env
if [ ! -e ${my_dir}/ext_iplib_converter ]; then
    echo "you should run build.sh first" >&2
    exit 1
fi

# 0. load basic config
. ${my_dir}/_base_config.sh
. ${my_dir}/_report.sh

#
# 1. parse args 
#
parse_args "$@"

# check conf_file
[ -n "${_conf_file}" ] || report_error_and_exit "$URL_BASE/report?r" "app" "null" "-c requried"
[ -f "${_conf_file}" ] || report_error_and_exit "$URL_BASE/report?r" "app" "null" "${_conf_file} does not exists"

if [ $_do_update -ne 1 -a $_force_update -eq 0 ]; then
    exit 0
fi

# load cleanup_util.sh and init_cleanup_trap
. $my_dir/_cleanup_util.sh
init_cleanup_trap

# check if one is already running
lockfile=$my_dir/.run.lock
add_to_cleanup_file $lockfile
if [ -e $lockfile ]; then
    echo "[Error] Only one iplib updater instance could be run at a time" >&2
    exit 1
fi
touch $lockfile

#
# 2. convert configs
#
converted_config_file=${my_dir}/.updater.cfg
python ${my_dir}/_convert_updater_config.py ${_conf_file} ${converted_config_file} || report_error_and_exit "$URL_BASE/report?r" "app" "null" "check config failed" 

# load configs
. ${converted_config_file}


#
# 3. prepare runtime env 
#


# create runtime directories
updater_data_dir=$sdk_dir/$updater_workdir
[ -d ${updater_data_dir} -a -d ${updater_data_dir}/current ] || mkdir -p ${updater_data_dir}/current
[ -d ${updater_outputdir} ] || mkdir -p ${updater_outputdir}


# tmpdir
UPDATER_TMP_DIR=${updater_data_dir}/.tmp.$$
mkdir ${UPDATER_TMP_DIR} 
add_to_cleanup_file ${UPDATER_TMP_DIR} 

echo "sdk_dir=${sdk_dir}" >> ${converted_config_file} 
echo "UPDATER_TMP_DIR=${UPDATER_TMP_DIR}" >> ${converted_config_file} 

#
# 4. update files
#
sh ${my_dir}/_update_files.sh ${converted_config_file} || report_error_and_exit "$URL_BASE/report?r" "$appkey" "$appkey" "update sdk files failed"


#
# 5. update data 
#
sh ${my_dir}/_update_data.sh ${converted_config_file} ${_force_update} ${_download_reversion} || report_error_and_exit "$URL_BASE/report?r" "$appkey" "$appkey" "update data failed"

