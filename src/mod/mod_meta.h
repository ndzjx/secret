
#ifndef MOD_META
#define MOD_META

#include <mod_email.h>
#include <mod_encrypt.h>
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
	const char*	smtp	= ""	;
	const char* pop3	= ""	;
	const char* user	= ""	;
	const char* pawd	= ""	;
	int64_t		mails	= 0		;
	int64_t		octets	= 0		;
};

REFLECTION( service_meta, smtp, pop3, user, pawd, mails, octets ) ;

//////////////////////////////////////////////////////////////////////////

<<<<<<< HEAD
=======
inline auto service_update( service_meta& service )
{
	if ( service.index == 0 )
	{
		service.index = std::hash<string>()( service.smtp + service.pop3 + service.user ) ;
	}

	auto stat = email_stat( service.pop3.c_str(), service.user.c_str(), service.pawd.c_str() ) ;
	if ( stat.first != service.mails || stat.second != service.octets )
	{
		service.mails = stat.first ;
		service.octets = stat.second ;
		return true ;
	}

	return false ;
}

inline void service_makeindex( const service_meta& service, service_index_t& indexs )
{
	for ( decltype( service.mails ) i = 1 ; i <= service.mails ; ++i )
	{
		auto subject = email_subject( service.pop3.c_str(), service.user.c_str(), service.pawd.c_str(), i ) ;

		file_meta fm ;
		meta_from_json( fm, subject.c_str() ) ;
		if ( fm.id.empty() )
		{
			continue ;
		}

		if ( fm.bytes == 0 || fm.end == 0 )
		{
			continue ;
		}

		indexs[ fm.id ].push_back( make_pair( make_pair( service.index, i ), fm ) ) ;
	}
}

>>>>>>> parent of 042d443... finish single file recv
inline int file_to_service( const service_meta& service, const char* file, const char* to )
{
	file_meta fm ;
	if ( meta_from_file( fm, file ) != 0 )
	{
		return 1 ;
	}

	auto subject = meta_to_json( fm ) ;
	if ( subject.empty() )
	{
		return 2 ;
	}

	boost::algorithm::replace_all( subject, "\"", "\"\"" ) ;
	return email_send( service.smtp, service.user, service.pawd, to, subject.c_str(), file ) ;
}

//////////////////////////////////////////////////////////////////////////

}

#endif
