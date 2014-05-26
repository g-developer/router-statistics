##! @VERSION: 1.0
##! @AUTHOR: tangwentao@baidu.com 

# set bash options
set -o pipefail

#
# temp file cleanup utils 
#
__to_cleanup_files=""

exit_cleanup_trap() 
{
    if [ -z "$__to_cleanup_files" ];then
        return 0
    fi  
    echo "removing $__to_cleanup_files ..." >/dev/null
    rm -rfv $__to_cleanup_files >/dev/null
}

init_cleanup_trap()
{
    __to_cleanup_files=""
    trap exit_cleanup_trap EXIT
}

add_to_cleanup_file()
{
    if [ $# -lt 1 ];then
        return 1
    fi  
    __to_cleanup_files="$__to_cleanup_files $@"
}

