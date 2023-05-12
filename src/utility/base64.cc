/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "base64.hh"
#include "zlib.h"
#include "dsAssert.hh"
#include <cctype>

namespace dsUtility {

static const unsigned char base64Chars[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz"
  "0123456789+/";



std::string encodeBase64(const char *input, size_t length)
{
  std::string ret;

  const unsigned char *ptr = reinterpret_cast<unsigned const char *>(input);

  const size_t remainder = length % 3;
  const size_t triplet_length = length - remainder;

  char triplet[5];
  triplet[4] = 0;

  for (size_t i = 0; i < triplet_length; i += 3)
  {
    triplet[0] = base64Chars[(ptr[i] >> 2)  & 0x3F];
    triplet[1] = base64Chars[((ptr[i] << 4) | (ptr[i+1] >> 4))  & 0x3F];
    triplet[2] = base64Chars[((ptr[i+1] << 2) | (ptr[i+2] >> 6))  & 0x3F];
    triplet[3] = base64Chars[(ptr[i+2]) & 0x3F];
    ret += std::string(triplet);
  }

  if (remainder == 1)
  {
    triplet[0] = base64Chars[(ptr[triplet_length] >> 2)  & 0x3F];
    triplet[1] = base64Chars[(ptr[triplet_length] << 4)  & 0x3F];
    triplet[2] = '=';
    triplet[3] = '=';
    ret += std::string(triplet);
  }
  else if (remainder == 2)
  {
    triplet[0] = base64Chars[(ptr[triplet_length] >> 2)  & 0x3F];
    triplet[1] = base64Chars[((ptr[triplet_length] << 4) | (ptr[triplet_length+1] >> 4))  & 0x3F];
    triplet[2] = base64Chars[(ptr[triplet_length+1] << 2)  & 0x3F];
    triplet[3] = '=';
    ret += std::string(triplet);
  }

  return ret;

}

template <typename T> std::string convertVectorToBase64(const std::vector<T> &x)
{
  const size_t length = sizeof(T) * x.size();
  const char *y = reinterpret_cast<const char *>(&x[0]);
  return encodeBase64(y, length);
}

template <typename T> std::string convertVectorToZlibBase64(const std::vector<T> &x)
{
  const size_t blockSize = 32768;

  const size_t numberOfTPerBlock = blockSize/sizeof(T);

  const size_t maxLength = numberOfTPerBlock * sizeof(T);

  const size_t numberOfFullBlocks = x.size() / numberOfTPerBlock;

  const size_t numberTInPartialBlock = x.size() % numberOfTPerBlock;

  size_t numberOfBlocks = numberOfFullBlocks;
  if (numberTInPartialBlock != 0)
  {
    numberOfBlocks++;
  }

  const int headerLength = numberOfBlocks + 3;
  std::vector <unsigned int> header(headerLength);
  header[0] = numberOfBlocks;
  header[1] = blockSize;
  header[2] = numberTInPartialBlock * sizeof(T);

  std::string        compressedOutput;
  std::vector<Bytef> outputArray(maxLength);
  for (size_t i = 0; i < numberOfBlocks; ++i)
  {
    uLong inputLength = maxLength;
    if (i == (numberOfBlocks - 1))
    {
      inputLength = sizeof(T) * numberTInPartialBlock;
//      header[3 + i] = inputLength;
    }
    else
    {
//      header[3 + i] = maxLength;
    }


    Bytef *inputArray = reinterpret_cast<Bytef *>(const_cast<T *>(&x[numberOfTPerBlock * i]));

    uLong outputLength = maxLength;
    int zlibRet = compress2(&(outputArray[0]), &outputLength, inputArray, inputLength, Z_DEFAULT_COMPRESSION);
    dsAssert(zlibRet == Z_OK, "UNEXPECTED");

    compressedOutput += std::string(reinterpret_cast<const char *>(&outputArray[0]), outputLength);
    header[3 + i] = outputLength;

  }
  const std::string encodedData = encodeBase64(compressedOutput.c_str(), compressedOutput.size());


  return convertVectorToBase64(header) + encodedData;
}

template std::string convertVectorToBase64(const std::vector<double> &x);
template std::string convertVectorToZlibBase64(const std::vector<double> &x);

static const unsigned char decodeBase64Table[256] =
{
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0x3E,0xFF,0xFF,0xFF,0x3F,
  0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,
  0x3C,0x3D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
  0xFF,0x00,0x01,0x02,0x03,0x04,0x05,0x06,
  0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,
  0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,
  0x17,0x18,0x19,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,
  0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
  0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,
  0x31,0x32,0x33,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

namespace {
enum decode_enum {BAD, OK, DONE};

decode_enum decode4(const std::string &in, std::string &tmp, std::string &out)
{
  tmp.resize(4);

  decode_enum result = OK;

  for (size_t i = 0; i < 4; ++i)
  {
    size_t char_index = in[i];
    tmp[i] = decodeBase64Table[char_index];
    if (tmp[i] == -1)
    {
      result = BAD;
      break;
    }

    if ((i < 2) && (in[i] == '='))
    {
      result = BAD;
      break;
    }
  }

  if ((in[2] == '=') && in[3] != '=')
  {
    result = BAD;
  }

  if (result != BAD)
  {
    out += ((tmp[0] << 2) & 0xFC) | ((tmp[1] >> 4) & 0x03);
    if (in[2] != '=')
    {
      out += ((tmp[1] << 4) & 0xF0) | ((tmp[2] >> 2) & 0x0F);
    }
    else
    {
      result = DONE;
    }

    if (in[3] != '=')
    {
      out += ((tmp[2] << 6) & 0xC0) | ((tmp[3] >> 0) & 0x3F);
    }
    else
    {
      result = DONE;
    }
  }
  return result;
}
}

bool decodeBase64(const std::string &input, std::string &output)
{
  bool ret = false;

  decode_enum result = OK;
  output.clear();
  output.reserve(input.size());
  std::string tmp;

  std::string tin;
  tin.resize(4);

  size_t j = 0;
  for (std::string::const_iterator it = input.begin(); it != input.end(); ++it)
  {
    const char &c = *it;
    if (!isspace(c))
    {
      tin[j] = c;
      ++j;
      if (j == 4)
      {
        result = decode4(tin, tmp, output);
        j = 0;
        if (result != OK)
        {
          break;
        }
      }
    }
  }
  if ((result == OK) && (j > 0) && (j != 4))
  {
    for ( ; j < 4; ++j)
    {
      tin[j] = '=';
    }
    result = decode4(tin, tmp, output);
  }

  ret = ((result == OK) || (result == DONE));
  return ret;
}
}

