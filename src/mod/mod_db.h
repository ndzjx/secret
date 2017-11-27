
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
	string		user	= ""	;
	string		pawd	= ""	;
	string		smtp	= ""	;
	string		pop3	= ""	;
	int64_t		mails	= 0		;
	int64_t		octets	= 0		;

	auto to_meta() const
	{
		service_meta ret ;
		ret.user = user ;
		ret.pawd = pawd ;
		ret.smtp = smtp ;
		ret.pop3 = pop3 ;
		ret.mails = mails ;
		ret.octets = octets ;
		return ret ;
	}

	void from_meta( const service_meta& param )
	{
		user = param.user ;
		pawd = param.pawd ;
		smtp = param.smtp ;
		pop3 = param.pop3 ;
		mails = param.mails ;
		octets = param.octets ;
	}
	
	ORMAP( "dbmeta_cloudnode", user, pawd, smtp, pop3, mails, octets ) ;
};

struct dbmeta_cloudfile
{
	string		id		= ""	;
	string		tag		= ""	;
	int64_t		bytes	= 0		;
	int64_t		beg		= 0		;
	int64_t		end		= 0		;
	
	string		service	= ""	;
	int64_t		number	= 0		;

	void from_meta( const file_meta& param, const string& node, int64_t i )
	{
		id = param.id ;
		tag = param.tag ;
		bytes = param.bytes ;
		beg = param.beg ;
		end = param.end ;

		service = node ;
		number = i ;
	}

	ORMAP( "dbmeta_cloudfile", id, tag, bytes, beg, end, service, number ) ;
};

//////////////////////////////////////////////////////////////////////////

}

#endif
