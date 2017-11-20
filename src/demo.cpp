
#include <mod_meta.h>

//////////////////////////////////////////////////////////////////////////

int main()
{
	using namespace secret ;
	
	service_meta service ;
	service.smtp = "smtp.163.com" ;
	service.pop3 = "pop3.163.com" ;
	service.user = "user@163.com" ;
	service.pawd = "password" ;
	file_to_service( service, "g:/2017.jpg", "qqlilichong@163.com" ) ;

	return 0 ;
}

//////////////////////////////////////////////////////////////////////////
