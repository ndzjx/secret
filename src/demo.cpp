
#include <mod_email.h>
#include <mod_encrypt.h>

int main()
{
	using namespace secret ;

	auto ss = hash_md5( "g:/2017.jpg" ) ;

	return 0 ;
}
