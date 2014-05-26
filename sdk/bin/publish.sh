#!/bin/sh
# vim: set tw=0 shiftwidth=4 tabstop=4 expandtab number:

#
# @author Tang Wentao(tangwentao@baidu.com)
# @date 2013/05/07
#

#
# publish a file to dest directory
#

if [ $# -lt 3 ]; then
    echo "Usage: $0 <publish_dir> <filename> <if_tar_file(Y/N)> [generate md5(Y/N)] [date pattren]"
    exit -1
fi

publish_dir=$1
publish_file_path=$2
publish_file_dir=`dirname ${publish_file_path}`
publish_file=`basename ${publish_file_path}`
if_tar_file=$3
genenrate_md5=Y

if [ $# -ge 4 ];then
    genenrate_md5=$4
fi

datestr=`date +%Y%m%d`
if [ $# -ge 5 ]; then
    datestr=`date $5`
fi


if [ ! -d ${publish_dir} ]; then
    echo "publish_dir:${publish_dir} DO NOT EXIST!"
    exit -1
fi
if [ ! -e ${publish_file_path} ];then
    echo "publish_file:${publish_file_path} DO NOT EXIST!"
    exit -1
fi

archive_file=${publish_file}.${datestr}

# tar czf the file
if [ x${if_tar_file} = "xY" ];then
    archive_file=${publish_file}.${datestr}.tar.gz
    tar czf $archive_file -C ${publish_file_dir} ${publish_file}
else
    ln -s ${publish_file_path} ${archive_file}
fi

cp ${archive_file} ${publish_dir} 
rm -rf ${archive_file}

# md5sum the file
if [ x${genenrate_md5} = "xY" ];then
    md5sum ${publish_dir}/$archive_file | awk '{print $1}' > ${publish_dir}/${archive_file}.md5
fi

# make soft link
cd ${publish_dir}

function _make_link()
{
    target=$1
    link_name=$2
    old_link_name=$3
    if [ -L $link_name ];then
        mv -f $link_name $old_link_name
    fi
    # for sure
    rm -rf $link_name

    # make softlink
    ln -s $target $link_name
}

if [ x${if_tar_file} = "xY" ];then
    _make_link $archive_file ${publish_file}.latest.tar.gz ${publish_file}.yesterday.tar.gz
    if [ x${genenrate_md5} = "xY" ];then
        _make_link $archive_file.md5 ${publish_file}.latest.tar.gz.md5 ${publish_file}.yesterday.tar.gz.md5
    fi
else
    _make_link $archive_file ${publish_file}.latest ${publish_file}.yesterday
    if [ x${genenrate_md5} = "xY" ];then
        _make_link $archive_file.md5 ${publish_file}.latest.md5 ${publish_file}.yesterday.md5
    fi
fi

cd - &>/dev/null

exit 0

