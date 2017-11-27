
#ifndef MOD_META
#define MOD_META

#include "mod_email.h"
#include "mod_encrypt.h"
#include "mod_parallel.h"
#include <iguana/json.hpp>

namespace secret {

//////////////////////////////////////////////////////////////////////////
// File META
struct file_meta
{
	string		id		= ""	;
	string		tag		= ""	;
	int64_t		bytes	= 0		;
	int64_t		beg		= 0		;
	int64_t		end		= 0		;
};

REFLECTION( file_meta, id, tag, bytes, beg, end ) ;

template<class T>
inline auto meta_to_json( T&& obj )
{
	iguana::string_stream ss ;

	try
	{
		iguana::json::to_json( ss, obj ) ;
	}

	catch ( ... )
	{

	}

	return ss.str() ;
}

inline int meta_from_json( file_meta& obj, const char* str )
{
	try
	{
		iguana::json::from_json( obj, str ) ;
	}

	catch ( ... )
	{
		return 1 ;
	}

	return 0 ;
}

inline int meta_from_file( file_meta& obj, const char* file )
{
	using namespace boost::filesystem ;
	using namespace secret ;

	obj.id = hash_md5( file ) ;
	if ( obj.id.empty() )
	{
		return 1 ;
	}

	obj.tag = system_complete( file ).filename().generic_string() ;
	if ( obj.tag.empty() )
	{
		return 2 ;
	}

	obj.bytes = file_size( file ) ;
	if ( obj.bytes == 0 )
	{
		return 3 ;
	}

	obj.beg = 0 ;
	obj.end = obj.bytes ;
	return 0 ;
}

//////////////////////////////////////////////////////////////////////////
// Service META
struct service_meta
{
	string		user	= ""	;
	string		pawd	= ""	;
	string		smtp	= ""	;
	string		pop3	= ""	;
	int64_t		mails	= 0		;
	int64_t		octets	= 0		;
};

REFLECTION( service_meta, user, pawd, smtp, pop3, mails, octets ) ;

//////////////////////////////////////////////////////////////////////////

inline auto service_update( service_meta& service )
{
	auto stat = email_stat( service.pop3.c_str(), service.user.c_str(), service.pawd.c_str() ) ;
	if ( stat.first != service.mails || stat.second != service.octets )
	{
		auto old = service.mails ;
		service.mails = stat.first ;
		service.octets = stat.second ;
		return stat.first - old ;
	}

	return decltype( service.mails )( 0 ) ;
}

// inline auto file_from_single_chunk(
// 	service_cloud_t& cloud,
// 	service_index_t& index,
// 	const char* filehash,
// 	const char* file )
// {
// 	// 索引里有这个文件
// 	if ( index.find( filehash ) == index.end() )
// 	{
// 		return false ;
// 	}
// 
// 	for ( auto&& chunk : index[ filehash ] )
// 	{
// 		const auto& service = cloud[ chunk.first.first ] ;
// 		auto uidl = chunk.first.second ;
// 		auto& fm = chunk.second ;
// 		if ( fm.bytes != ( fm.end - fm.beg ) )
// 		{
// 			continue ;
// 		}
// 		
// 		if ( file_create( file, fm.bytes ) != 0 )
// 		{
// 			break ;
// 		}
// 
// 		if ( email_recv(
// 			service.pop3.c_str(),
// 			service.user.c_str(),
// 			service.pawd.c_str(),
// 			uidl,
// 			file,
// 			0, fm.bytes, 0 ) != 200 )
// 		{
// 			continue ;
// 		}
// 
// 		return true ;
// 	}
// 
// 	return false ;
// }

inline int file_to_service( const service_meta& service, file_meta fm, const char* file, const char* to )
{
	auto subject = meta_to_json( fm ) ;
	if ( subject.empty() )
	{
		return 1 ;
	}
	
	boost::algorithm::replace_all( subject, "\"", "\"\"" ) ;
	return email_send( service.smtp.c_str(), service.user.c_str(), service.pawd.c_str(), to, subject.c_str(), file ) ;
}

//////////////////////////////////////////////////////////////////////////

}

#endif
