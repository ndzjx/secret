
#ifndef MOD_ENCRYPT
#define MOD_ENCRYPT

#include "mod_std.h"
#include "mod_boost.h"
#include "mod_pipe.h"
#include "mod_file.h"
#include "openssl/des.h"
#include "LzmaLib.h"

namespace secret {

inline string hash_md5( const char* file )
{
	using namespace boost::filesystem ;

	string ret ;
	
	string raw = ( system_complete( __argv[ 0 ] ).remove_filename() /= ( "hash_md5.exe" ) ).generic_string() ;
	
	raw += R"( ")" ;
	raw += file ;
	raw += R"(")" ;
	
	SECURITY_ATTRIBUTES sa = { 0 } ;
	sa.nLength = sizeof( sa ) ;
	sa.bInheritHandle = TRUE ;
	sa.lpSecurityDescriptor = NULL ;

	HANDLE hReadPipe = NULL ;
	HANDLE hWritePipe = NULL ;
	if ( CreatePipe( &hReadPipe, &hWritePipe, &sa, 0 ) != TRUE )
	{
		return ret ;
	}
	
	if ( exec_proxy( raw.c_str(), hWritePipe ) != 200 )
	{
		CloseHandle( hReadPipe ) ;
		CloseHandle( hWritePipe ) ;
		return ret ;
	}

	try
	{
		CloseHandle( hWritePipe ) ;
		auto file = file_handle( hReadPipe ) ;
		if ( file )
		{
			const auto len = file_size( file.get() ) ;
			ret.resize( len + 1 ) ;
			fread( &ret.at( 0 ), 1, len, file.get() ) ;
			ret.resize( len ) ;
		}
	}

	catch ( ... )
	{

	}

	return ret ;
}

template<class T>
inline auto base64_encode( T&& param )
{
	constexpr char EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" ;
	auto Data = param.data() ;
	const size_t DataByte = param.size() ;

	string ret ;
	unsigned char Tmp[ 4 ] = { 0 } ;
	for ( size_t i = 0 ; i < size_t( DataByte / 3 ) ; ++i )
    {
		Tmp[ 1 ] = *Data++ ;
		Tmp[ 2 ] = *Data++ ;
		Tmp[ 3 ] = *Data++ ;
        ret += EncodeTable[ Tmp[ 1 ] >> 2 ] ;
        ret += EncodeTable[ ( ( Tmp[ 1 ] << 4 ) | ( Tmp[ 2 ] >> 4 ) ) & 0x3F ] ;
        ret += EncodeTable[ ( ( Tmp[ 2 ] << 2 ) | ( Tmp[ 3 ] >> 6 ) ) & 0x3F ] ;
        ret += EncodeTable[ Tmp[ 3 ] & 0x3F ] ;
    }
	
	auto Mod = size_t( DataByte % 3 ) ;
    if ( Mod == 1 )
    {
		Tmp[ 1 ] = *Data++ ;
		ret += EncodeTable[ ( Tmp[ 1 ] & 0xFC ) >> 2 ] ;
		ret += EncodeTable[ ( ( Tmp[ 1 ] & 0x03 ) << 4 ) ] ;
		ret += "==" ;
		return ret ;
    }

    if ( Mod == 2 )
    {
		Tmp[ 1 ] = *Data++ ;
		Tmp[ 2 ] = *Data++ ;
        ret += EncodeTable[ ( Tmp[ 1 ] & 0xFC ) >> 2 ] ;
        ret += EncodeTable[ ( ( Tmp[ 1 ] & 0x03 ) << 4 ) | ( ( Tmp[ 2 ] & 0xF0 ) >> 4 ) ] ;
        ret += EncodeTable[ (( Tmp[ 2 ] & 0x0F ) << 2 ) ] ;
        ret += '=' ;
		return ret ;
    }
	
	return decltype( ret )() ;
}

template<class T = vector<char>>
inline auto base64_decode( const char* str )
{
    constexpr char DecodeTable[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
		0, 0, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
		0, 0, 0, 0, 0, 0,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
    } ;
	
	T ret ;
	auto Data = str ;
	const auto DataByte = strlen( str ) ;
	size_t nValue = 0 ;
	size_t i = 0 ;
	while ( i < DataByte )
    {
		nValue = DecodeTable[ *Data++ ] << 18 ;
		nValue += DecodeTable[ *Data++ ] << 12 ;
		ret.push_back( char( ( nValue & 0x00FF0000 ) >> 16 ) ) ;
		
		if ( *Data != '=' )
		{
			nValue += DecodeTable[ *Data++ ] << 6 ;
			ret.push_back( char( ( nValue & 0x0000FF00 ) >> 8 ) ) ;
			
			if ( *Data != '=' )
			{
				nValue += DecodeTable[ *Data++ ] ;
				ret.push_back( char( nValue & 0x000000FF ) ) ;
			}
		}
		
		i += 4 ;
	}
	
	return ret ;
}

inline bool encrypt(std::istream &source, std::ostream &destination, const string &key)
{
	const int c_ReadSize = 120;

	// 秘钥
	DES_key_schedule keySchedule;
	// 初始化向量
	DES_cblock desIv;
	const_DES_cblock cDesKey[1];
	DES_string_to_key(key.c_str(), cDesKey);
	DES_set_key_checked(cDesKey, &keySchedule);

	char szPlainBlock[c_ReadSize] = { 0 };
	char szCipherBlock[c_ReadSize] = { 0 };

	while (!source.eof())
	{
		source.read(szPlainBlock, c_ReadSize);
		if (source.gcount() == c_ReadSize)
		{
			memset((char*)&desIv, 0, sizeof(desIv));
			DES_ncbc_encrypt((const unsigned char*)szPlainBlock, (unsigned char*)szCipherBlock,
				c_ReadSize, &keySchedule, &desIv, DES_ENCRYPT);
			destination.write(szCipherBlock, c_ReadSize);
		}
	}
	if (source.gcount())
	{
		// 设置大小标志
		memset(szPlainBlock + source.gcount(), 0, c_ReadSize - 1 - source.gcount());
		szPlainBlock[c_ReadSize - 1] = (char)source.gcount();

		memset((char*)&desIv, 0, sizeof(desIv));
		DES_ncbc_encrypt((const unsigned char*)szPlainBlock, (unsigned char*)szCipherBlock,
			c_ReadSize, &keySchedule, &desIv, DES_ENCRYPT);
		destination.write(szCipherBlock, c_ReadSize);
	}
	else
	{
		// 写一个空标志
		memset(szPlainBlock, 0, sizeof(szCipherBlock));
		memset((char*)&desIv, 0, sizeof(desIv));
		DES_ncbc_encrypt((const unsigned char*)szPlainBlock, (unsigned char*)szCipherBlock,
			c_ReadSize, &keySchedule, &desIv, DES_ENCRYPT);
		destination.write(szCipherBlock, c_ReadSize);
	}
	return true;
}

inline bool decrypt(std::istream &source, std::ostream &destination, const string &key)
{
	const int c_ReadSize = 120;

	source.seekg(0, ios::end);
	auto nLength = source.tellg();
	source.seekg(0, ios::beg);

	// 秘钥
	DES_key_schedule keySchedule;
	// 初始化向量
	DES_cblock desIv;
	const_DES_cblock cDesKey[1];
	DES_string_to_key(key.c_str(), cDesKey);
	DES_set_key_checked(cDesKey, &keySchedule);

	int nReadLength(0);
	char szPlainBlock[c_ReadSize] = { 0 };
	char szCipherBlock[c_ReadSize] = { 0 };

	while (!source.eof())
	{
		source.read(szCipherBlock, c_ReadSize);

		memset((char*)&desIv, 0, sizeof(desIv));
		DES_ncbc_encrypt((const unsigned char*)szCipherBlock, (unsigned char*)szPlainBlock,
			c_ReadSize, &keySchedule, &desIv, DES_DECRYPT);
		nReadLength += c_ReadSize;

		// 认为最后一个是数据加标志
		if (nReadLength < nLength)
		{
			destination.write(szPlainBlock, c_ReadSize);
		}
		else
		{
			// 最后一块的处理要考虑标志位
			if (0 != szPlainBlock[c_ReadSize - 1])
			{
				destination.write(szPlainBlock, szPlainBlock[c_ReadSize - 1]);
			}
			break;
		}
	}

	return true;
}

inline bool encrypt(const string &source, string &destination, const string &key)
{
	istringstream oIn(source, ios::binary);
	ostringstream oOut(ios::binary);
	bool bRet = encrypt(oIn, oOut, key);
	if (bRet)
	{
		destination = oOut.str();
	}
	return bRet;
}

inline bool decrypt(const string &source, string &destination, const string &key)
{
	istringstream oIn(source, ios::binary);
	ostringstream oOut(ios::binary);
	bool bRet = decrypt(oIn, oOut, key);
	if (bRet)
	{
		destination = oOut.str();
	}
	return bRet;
}

inline bool lzmaCompress(std::istream &source, std::ostream &destination)
{
    source.seekg(0, ios::end);
    size_t nSrcLen = source.tellg();
    if(0 == nSrcLen)
    {
        return true;
    }
    source.seekg(0, source.beg);

    string sSrcBuffer;
    string sDestBuffer;

    sSrcBuffer.resize(nSrcLen, 0);
    source.read((char*)sSrcBuffer.data(), nSrcLen);

    size_t nDestLen = nSrcLen * 2;
    sDestBuffer.resize(nDestLen, 0);

    unsigned char aProp[5] = {0};
    size_t nProp = 5;

    int nRet = LzmaCompress((unsigned char*)sDestBuffer.c_str(), &nDestLen,
                            (unsigned char*)sSrcBuffer.c_str(), nSrcLen,
                            aProp, &nProp,
                            2, (1 << 18), 3, 0, 2, 32, 2);
    if (SZ_OK != nRet)
    {
        return false;
    }

    __int64 nSrcLenValue = nSrcLen;
    __int64 nPropValue = nProp;
    destination.write((char*)&nSrcLenValue, sizeof(__int64));
    destination.write((char*)&nPropValue, sizeof(__int64));
    destination.write((char*)aProp, sizeof(aProp));
    destination.write(sDestBuffer.c_str(), nDestLen);

    return true;
}

inline bool lzmaUnCompress(std::istream &source, std::ostream &destination)
{
    source.seekg(0, ios::end);
    size_t nDataSize = source.tellg();
    if(0 == nDataSize)
    {
        return true;
    }
    source.seekg(0, ios::beg);

    size_t nDestLen = 0;
    unsigned char aProp[5] = {0};
    size_t nProp = 0;

    __int64 nDestLenValue;
    __int64 nPropValue;
    source.read((char*)&nDestLenValue, sizeof(__int64));
    source.read((char*)&nPropValue, sizeof(__int64));
    source.read((char*)aProp, sizeof(aProp));

    nDestLen = nDestLenValue;
    nProp = nPropValue;

    string sSrcBuffer;
    string sDestBuffer;

    size_t nSrcLen = nDataSize - source.tellg();
    sSrcBuffer.resize(nSrcLen, 0);
    source.read((char*)sSrcBuffer.data(), nSrcLen);
    sDestBuffer.resize(nDestLen, 0);

    int nRet = LzmaUncompress((unsigned char*)sDestBuffer.c_str(), &nDestLen,
                              (unsigned char*)sSrcBuffer.c_str(),
                              &nSrcLen, aProp, nProp);
    if(SZ_OK != nRet)
    {
        return false;
    }    
    destination.write(sDestBuffer.c_str(), nDestLen);

    return true;
}

inline bool lzmaCompress(const string &source, string &destination)
{
    istringstream oIn(source, ios::binary);
    ostringstream oOut(ios::binary);
    bool bRet = lzmaCompress(oIn, oOut);
    if(bRet)
    {
        destination = oOut.str();
    }
    return bRet;
}

inline bool lzmaUnCompress(const string &source, string &destination)
{
    istringstream oIn(source, ios::binary);
    ostringstream oOut(ios::binary);
    bool bRet = lzmaUnCompress(oIn, oOut);
    if(bRet)
    {
        destination = oOut.str();
    }
    return bRet;
}

}

#endif
