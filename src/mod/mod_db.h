
#ifndef MOD_DB
#define MOD_DB

#include <ormlite/ormlite.h>
using namespace BOT_ORM ;
using namespace BOT_ORM::Expression ;

#include "mod_meta.h"

namespace secret {

//////////////////////////////////////////////////////////////////////////

struct dbmeta_cloudnode
{
	string		user	;
	string		pawd	;
	string		smtp	;
	string		pop3	;
	string		mails	;
	string		octets	;

	auto to_meta() const
	{
		service_meta ret ;
		ret.user = user ;
		ret.pawd = pawd ;
		ret.smtp = smtp ;
		ret.pop3 = pop3 ;
		ret.mails = stoull( mails ) ;
		ret.octets = stoull( octets ) ;
		return ret ;
	}

	void from_meta( const service_meta& param )
	{
		user = param.user ;
		pawd = param.pawd ;
		smtp = param.smtp ;
		pop3 = param.pop3 ;
		mails = to_string( param.mails ) ;
		octets = to_string( param.octets ) ;
	}
	
	ORMAP( "dbmeta_cloudnode", user, pawd, smtp, pop3, mails, octets ) ;
};

struct dbmeta_cloudfile
{
	string		id		;
	string		tag		;
	string		bytes	;
	string		beg		;
	string		end		;
	
	string		service	;
	string		number	;

	void from_meta( const file_meta& param, const string& node, int64_t i )
	{
		id = param.id ;
		tag = param.tag ;
		bytes = to_string( param.bytes ) ;
		beg = to_string( param.beg ) ;
		end = to_string( param.end ) ;

		service = node ;
		number = to_string( i ) ;
	}

	auto to_meta() const
	{
		file_meta fm ;
		fm.id = id ;
		fm.tag = tag ;
		fm.bytes = stoull( bytes ) ;
		fm.beg = stoull( beg ) ;
		fm.end = stoull( end ) ;
		return fm ;
	}

	ORMAP( "dbmeta_cloudfile", id, tag, bytes, beg, end, service, number ) ;
};

//////////////////////////////////////////////////////////////////////////

}

#endif
