#
# build file of newiploc
# @author Tang Wentao(tangwentao@baidu.com)
# @date 2014-01-17
#

canonicalize_path=`readlink -f $0`
sdk_dir=`dirname ${canonicalize_path}`
cd $sdk_dir

# protobuf env
svn co https://svn.baidu.com/third-64/tags/protobuf/protobuf_2-4-1-700_PD_BL protobuf

# prepare necessary files
protobuf/bin/protoc -I$sdk_dir/src --cpp_out=$sdk_dir/src --python_out=$sdk_dir/bin $sdk_dir/src/iplibrary.proto
cp ../tools/iprecords.py $sdk_dir/bin
cp ../iploc/common/scripts/publish.sh $sdk_dir/bin 
mv $sdk_dir/src/*.h $sdk_dir/include 

make

rm libs/lib* -rf
cp .libs/* libs/

cd - &>/dev/null

