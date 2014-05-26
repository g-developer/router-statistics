/*
 * common functions 
 *
 * @author Tang Wentao(tangwentao@baidu.com)
 * @date 2014-01-27
 */
#ifndef __ICONV_WRAPPER_H__
#define __ICONV_WRAPPER_H__

#include <iconv.h>
#include <string>

class IconvWrapper
{
public:
    IconvWrapper();
    ~IconvWrapper();

    int open(const std::string &fromcode, const std::string &tocode);
    int convert(const std::string &in, std::string &out);

private:
    iconv_t _iconv_handler;
};

#endif

