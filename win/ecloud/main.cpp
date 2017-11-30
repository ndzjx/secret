
#include <QApplication>
#include "ecloud.h"
#include "CUploadWidget.h"
#include "CSettingWidget.h"
#include "CBrowserWidget.h"

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#pragma comment( lib, "cpprest140_2_10.lib" )
using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

//////////////////////////////////////////////////////////////////////////

// 每1秒更新一次云端信息
void plan_update_cloud()
{
	auto timer = std::make_shared<boost::asio::deadline_timer>( global_pc().get_ios(), boost::posix_time::seconds( 1 ) ) ;
	timer->async_wait( [ timer ]( boost::system::error_code ec )
	{
		if ( ec == 0 )
		{
			global_update_cloud() ;
			plan_update_cloud() ;
		}

	} ) ;
}

int main( int argc, char *argv[] )
{
//	        // Create http_client to send the request.
//        http_client client(U("http://www.baidu.com/"));

//        // Build request URI and start the request.
//        uri_builder builder(U("/search"));
//        builder.append_query(U("q"), U("cpprestsdk github"));
//        client.request(methods::GET, builder.to_string());

		

//	return 0 ;

	ParallelCore pc_task ;
	global_pc( &pc_task ) ;
	
	ORMapper db( ( boost::filesystem::system_complete( __argv[ 0 ] ).remove_filename() /= ( "ecloud.db" ) ).generic_string() ) ;
	global_db( &db ) ;
	
	QApplication app( argc, argv ) ;

//    CUploadWidget w ;
//    CSettingWidget w;
    CBrowserWidget w;
    w.show() ;
	
	plan_update_cloud() ;
	return app.exec() ;
}

//////////////////////////////////////////////////////////////////////////
