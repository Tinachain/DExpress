/*
 * encode.h
 *
 *  Created on: 2017年7月3日
 *      Author: fxh7622
 */

#ifndef ENCODE_H_
#define ENCODE_H_

#include <string>
#include <iostream>
using namespace std;

static const char safe[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

std::string encode(const std::string &uri)
{
	string ret;
	const unsigned char *ptr = (const unsigned char *)uri.c_str();
	ret.reserve(uri.length());
	for (; *ptr ; ++ptr)
	{
		if (!safe[*ptr])
		{
			char buf[5] = {0};
			//memset(buf, 0, 5);
#ifdef WIN32
			_snprintf_s(buf, 5, "%%%X", (*ptr));
#else
			snprintf(buf, 5, "%%%X", (*ptr));
#endif
			ret.append(buf);
		}
		else if (*ptr==' ')
		{
			ret+='+';
		}
		else
		{
			ret += *ptr;
		}
	}
	return ret;
}

std::string decode(const std::string &uri)
{
	//Note from RFC1630:  "Sequences which start with a percent sign
	//but are not followed by two hexadecimal characters (0-9,A-F) are reserved
	//for future extension"
	const unsigned char *ptr = (const unsigned char *)uri.c_str();
	string ret;
	ret.reserve(uri.length());
	for (; *ptr; ++ptr)
	{
		if (*ptr == '%')
		{
			if (*(ptr + 1))
			{
				char a = *(ptr + 1);
				char b = *(ptr + 2);
				if (!((a >= 0x30 && a < 0x40) || (a >= 0x41 && a < 0x47))) continue;
				if (!((b >= 0x30 && b < 0x40) || (b >= 0x41 && b < 0x47))) continue;
				char buf[3];
				buf[0] = a;
				buf[1] = b;
				buf[2] = 0;
				ret += (char)strtoul(buf, NULL, 16);
				ptr += 2;
				continue;
			}
		}
		if (*ptr=='+')
		{
			ret+=' ';
			continue;
		}
		ret += *ptr;
	}
	return ret;
}

std::string UrlEncode(const std::string& szToEncode)
{
	std::string src = szToEncode;
	char hex[] = "0123456789ABCDEF";
	string dst;
	for (size_t i = 0; i < src.size(); ++i)
	{
		unsigned char cc = src[i];
		if (isascii(cc))
		{
			if (cc == ' ')
			{
				dst += "%20";
			}
			else
				dst += cc;
		}
		else
		{
			unsigned char c = static_cast<unsigned char>(src[i]);
			dst += '%';
			dst += hex[c / 16];
			dst += hex[c % 16];
		}
	}
	return dst;
}

std::string UrlDecode(const std::string& szToDecode)
{
	std::string result;
	int hex = 0;
	for (size_t i = 0; i < szToDecode.length(); ++i)
	{
		switch (szToDecode[i])
		{
			case '+':
			{
				result += ' ';
				break;
			}
			case '%':
			{
				if (isxdigit(szToDecode[i + 1]) && isxdigit(szToDecode[i + 2]))
				{
					std::string hexStr = szToDecode.substr(i + 1, 2);
					hex = strtol(hexStr.c_str(), 0, 16);
					//字母和数字[0-9a-zA-Z]、一些特殊符号[$-_.+!*'(),] 、以及某些保留字[$&+,/:;=?@]
					//可以不经过编码直接用于URL
					if (!((hex >= 48 && hex <= 57) || //0-9
							(hex >=97 && hex <= 122) ||   //a-z
							(hex >=65 && hex <= 90) ||    //A-Z
							//一些特殊符号及保留字[$-_.+!*'(),]  [$&+,/:;=?@]
							hex == 0x21 || hex == 0x24 || hex == 0x26 || hex == 0x27 || hex == 0x28 || hex == 0x29
							|| hex == 0x2a || hex == 0x2b|| hex == 0x2c || hex == 0x2d || hex == 0x2e || hex == 0x2f
							|| hex == 0x3A || hex == 0x3B|| hex == 0x3D || hex == 0x3f || hex == 0x40 || hex == 0x5f))
					{
						result += char(hex);
						i += 2;
					}
					else result += '%';
				}
				else
				{
					result += '%';
				}
				break;
			}
			default:
			{
				result += szToDecode[i];
				break;
			}
		}
	}
	return result;
}


unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
}

//标准实现;
std::string url_encode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if(isalnum((unsigned char)str[i]) || (str[i] == '-') || (str[i] == '_') || (str[i] == '.') || (str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}

std::string url_decode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (str[i] == '+') strTemp += ' ';
		else if (str[i] == '%')
		{
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += high*16 + low;
		}
		else strTemp += str[i];
	}
	return strTemp;
}























#endif /* ENCODE_H_ */
