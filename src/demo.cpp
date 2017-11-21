
#include <mod_meta.h>
using namespace secret ;

//////////////////////////////////////////////////////////////////////////

service_cloud_t g_cloud ;
service_index_t g_index ;

int main()
{
	service_meta s1{ "smtp.126.com", "pop.126.com", "testhackpro@126.com", "123abc", 0, 0, 0 } ;
	service_update( s1 ) ;
	g_cloud.emplace( s1.index, s1 ) ;

	// ���������Ĵ����ļ�����
	for ( auto&& node : g_cloud )
	{
		service_makeindex( node.second, g_index ) ;
	}

	if ( true )
	{
		// ��������ļ�
		const char* id = "FA8249C2124C9F96DCBED0DDAD9040E3" ;
		const char* file = "g:/hehe.png" ;
		file_from_single_chunk( g_cloud, g_index, id, file ) ;
	}

	return 0 ;
}

//////////////////////////////////////////////////////////////////////////
