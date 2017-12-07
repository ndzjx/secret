
#include <QApplication>
#include <QSplashScreen>
#include "ecloud.h"
#include "CUploadWidget.h"
#include "CSettingWidget.h"
#include "CBrowserWidget.h"

//////////////////////////////////////////////////////////////////////////

// 计划: 更新云端信息
void plan_cloudnodes_update()
{
	auto timer = std::make_shared<boost::asio::deadline_timer>( global_pc().get_ios(), boost::posix_time::seconds( 3 ) ) ;

	auto fina = shared_ptr<void>( nullptr, [ timer ](void*)
	{
		if ( global_pc().get_ios().stopped() == false )
		{
			plan_cloudnodes_update() ;
		}
	} ) ;
	
	timer->async_wait( [ timer, fina ]( boost::system::error_code ec )
	{
		if ( ec == 0 )
		{
			global_cloudnodes_update( fina ) ;
		}
	} ) ;
}

int main( int argc, char *argv[] )
{
	QApplication app( argc, argv ) ;
	auto pLogo = new QSplashScreen( QPixmap( ":/res/logo.png" ) ) ;
	pLogo->show() ;
	app.processEvents() ;
	
	pLogo->showMessage( "Initialization Start Engines ...", Qt::AlignRight | Qt::AlignBottom, Qt::yellow ) ;
	ParallelCore pc_task ;
	global_pc( &pc_task ) ;
	boost::this_thread::sleep( boost::posix_time::milliseconds( 300 ) ) ;
	
	pLogo->showMessage( "Initialization SQLite ...", Qt::AlignRight | Qt::AlignBottom, Qt::yellow ) ;
	ORMapper db( ( boost::filesystem::system_complete( __argv[ 0 ] ).remove_filename() /= ( "ecloud.db" ) ).generic_string() ) ;
	global_db( &db ) ;
	boost::this_thread::sleep( boost::posix_time::milliseconds( 300 ) ) ;
	
	pLogo->showMessage( "Initialization Tasks ...", Qt::AlignRight | Qt::AlignBottom, Qt::yellow ) ;
	plan_cloudnodes_update() ;
	boost::this_thread::sleep( boost::posix_time::milliseconds( 300 ) ) ;

	pLogo->showMessage( "Ready Widgets ...", Qt::AlignRight | Qt::AlignBottom, Qt::yellow ) ;
	CUploadWidget wUpload ;
    CSettingWidget wSetting ;
	CBrowserWidget wBrowser ;
	boost::this_thread::sleep( boost::posix_time::milliseconds( 300 ) ) ;

	pLogo->showMessage( "Welcome!", Qt::AlignRight | Qt::AlignBottom, Qt::yellow ) ;
	boost::this_thread::sleep( boost::posix_time::milliseconds( 300 ) ) ;
	delete pLogo ;

	wUpload.show() ;
	app.exec() ;
	
	global_pc().stop() ;
	return 0 ;
}

//////////////////////////////////////////////////////////////////////////
