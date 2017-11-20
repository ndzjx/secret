
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
	file_to_service( service, "g:/2017.jpg", "testhackpro@126.com" ) ;

	return 0 ;
}

//////////////////////////////////////////////////////////////////////////
