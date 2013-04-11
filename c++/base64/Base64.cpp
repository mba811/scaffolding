/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2010 Live Networks, Inc.  All rights reserved.
// Base64 encoding and decoding
// implementation

#include "base64.h"

#include <string.h>

static char decode_map[256];


static void initBase64DecodeMap()
{
	memset(decode_map,-1,sizeof(decode_map));
	for (int i = 'A'; i <= 'Z'; ++i) decode_map[i] = 0 + (i - 'A');
	for (int i = 'a'; i <= 'z'; ++i) decode_map[i] = 26 + (i - 'a');
	for (int i = '0'; i <= '9'; ++i) decode_map[i] = 52 + (i - '0');
	decode_map[(unsigned char)'+'] = 62;
	decode_map[(unsigned char)'/'] = 63;
	decode_map[(unsigned char)'='] = 0;
}

static const char encode_map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

bool CBase64::Encode(const string& strIn,string& strOut)
{
	size_t nInLen = strIn.length();
	size_t numOrig24BitValues = nInLen/3;
	bool havePadding = (nInLen != numOrig24BitValues*3);
	bool havePadding2 = (nInLen == numOrig24BitValues*3 + 1);
	size_t numResultBytes = 4*(numOrig24BitValues + havePadding);
	strOut.clear();
	for (size_t i = 0; i < numOrig24BitValues; ++i) 
	{
		strOut.append(1,encode_map[(strIn[3*i] >> 2) & 0x3F]);
		strOut.append(1,encode_map[((strIn[3*i] << 4) & 0x30) | ((strIn[3*i+1] >> 4) & 0x0F)]);
		strOut.append(1,encode_map[((strIn[3*i+1] << 2) & 0x3C) | ((strIn[3*i+2] >> 6) & 0x03)]);
		strOut.append(1,encode_map[strIn[3*i+2] & 0x3F]);

	}

	if (havePadding) 
	{
		size_t i = numOrig24BitValues;
		strOut.append(1,encode_map[(strIn[3*i] >> 2) & 0x3F]);
		if (havePadding2)
		{
			strOut.append(1,encode_map[((strIn[3*i] << 4) & 0x30) | ((strIn[3*i+1] >> 4) & 0x0F)]);
			strOut.append(1,'=');
		} 
		else 
		{
			strOut.append(1,encode_map[((strIn[3*i] << 4) & 0x30) | ((strIn[3*i+1] >> 4) & 0x0F)]);
			strOut.append(1,encode_map[((strIn[3*i+1] << 2) & 0x3C) | ((strIn[3*i+2] >> 6) & 0x03)]);
		}
		strOut.append(1,'=');
	}

	return true;
}
#include <iostream>
bool CBase64::Decode(const string& strIn,string& strOut,bool fCheckInputValid/* = false*/)
{
	size_t nInlen = strIn.length();
	if (nInlen < 4 || (nInlen % 4) != 0)
	{
		return false;
	}

	static bool bInit = false;
	if (!bInit)
	{
		initBase64DecodeMap();
		bInit = true;
	}

	if (fCheckInputValid)
	{
		for (size_t i = 0; i < nInlen; ++i)
		{
			if (decode_map[(unsigned char)strIn[i]] == -1)
			{
				return false;
			}
		}
	}
	size_t nOutLen = (nInlen * 3) / 4;
	string strTmpOut;
	strTmpOut.resize(nOutLen);
	size_t nLoopLen = nOutLen / 3;
	for (size_t i = 0; i < nLoopLen;++i)
	{
		strTmpOut[i*3] = ((decode_map[strIn[i*4]] << 2) & 0xFC) | ((decode_map[strIn[i*4+1]] >> 4) & 0x03);
		strTmpOut[i*3+1] = ((decode_map[strIn[i*4+1]] << 4) & 0xF0) | ((decode_map[strIn[i*4+2]] >> 2) & 0x0F);
		strTmpOut[i*3+2] = ((decode_map[strIn[i*4+2]] << 6) & 0xC0) | (decode_map[strIn[i*4+3]] & 0x3F);
	}

	if (strIn[nInlen - 1] == '=')
	{
		nOutLen--;
		if (strIn[nInlen - 2] == '=')
		{
			nOutLen--;
		}
	}
	const char* pData = strTmpOut.data();
	strOut.clear();
	strOut.append(pData,pData + nOutLen);
	return true;
}