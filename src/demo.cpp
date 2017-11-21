
#include <mod_meta.h>
using namespace secret ;

//////////////////////////////////////////////////////////////////////////

service_cloud_t g_cloud_service ;
service_index_t g_index_service ;

int main()
{
	service_meta s1{ "smtp.126.com", "pop.126.com", "testhackpro@126.com", "123abc", 0, 0, 0 } ;
	service_update( s1 ) ;
	g_cloud_service.emplace( s1.index, s1 ) ;

	// 根据云中心创建文件索引
	for ( auto&& node : g_cloud_service )
	{
		service_makeindex( node.second, g_index_service ) ;
	}

	if ( true )
	{
		// 下载这个文件
		const char* id = "FA8249C2124C9F96DCBED0DDAD9040E3" ;
		const char* dir = "g:/www" ;

		// 索引里有这个文件
		if ( g_index_service.find( id ) != g_index_service.end() )
		{
			// 尝试单块完成文件
			for ( auto&& chunk : g_index_service[ id ] )
			{
				const auto& service = g_cloud_service[ chunk.first.first ] ;
				auto uidl = chunk.first.second ;
				auto& fm = chunk.second ;
				if ( fm.bytes != ( fm.end - fm.beg ) )
				{
					continue ;
				}

				file_create( "g:/hehe.png", fm.bytes ) ;
				email_recv(
					service.pop3.c_str(),
					service.user.c_str(),
					service.pawd.c_str(),
					uidl,
					"g:/hehe.png",
					0, fm.bytes, 0 ) ;
			}
		}
	}

	return 0 ;
}

//////////////////////////////////////////////////////////////////////////
