
#include <mod_meta.h>

//////////////////////////////////////////////////////////////////////////

int main()
{
	using namespace secret ;

	service_meta service ;
	service.smtp = "smtp.126.com" ;
	service.pop3 = "pop.126.com" ;
	service.user = "testhackpro@126.com" ;
	service.pawd = "123abc" ;

	auto ss = email_subject( service.pop3, service.user, service.pawd, 1 ) ;

	return 0 ;
	
	if ( true )
	{
		file_to_service( service, "g:/2017.jpg", "testhackpro@126.com" ) ;
		return 0 ;
	}

	if ( true )
	{
		return 0 ;
	}

	return 0 ;
}

//////////////////////////////////////////////////////////////////////////
