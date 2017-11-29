
#include <QApplication>
#include "ecloud.h"
#include "CUploadWidget.h"
#include "CSettingWidget.h"

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
	ParallelCore pc_task ;
	global_pc( &pc_task ) ;
	
	ORMapper db( ( boost::filesystem::system_complete( __argv[ 0 ] ).remove_filename() /= ( "ecloud.db" ) ).generic_string() ) ;
	global_db( &db ) ;
	
	QApplication app( argc, argv ) ;

//    CUploadWidget w ;
    CSettingWidget w;
    w.show() ;

	

	plan_update_cloud() ;
	return app.exec() ;
}

//////////////////////////////////////////////////////////////////////////
