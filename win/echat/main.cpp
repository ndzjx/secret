
#include <QDir>
#include <QTemporaryFile>
#include <QTextCodec>
#include <QApplication>
#include <QSplashScreen>
#include "MessageScrollWidget.h"
#include "echat.h"
#include "MainForm.h"
#include "Widget.h"
#include "eChatWidget.h"

//////////////////////////////////////////////////////////////////////////

// 计划: 更新云端信息
void plan_cloudnodes_update( msgsig_t& sig )
{
	auto timer = std::make_shared<boost::asio::deadline_timer>( global_pc().get_ios(), boost::posix_time::seconds( 3 ) ) ;

	auto fina = shared_ptr<void>( nullptr, [ timer, &sig ](void*)
	{
		if ( global_pc().get_ios().stopped() == false )
		{
			plan_cloudnodes_update( sig ) ;
		}
	} ) ;
	
	timer->async_wait( [ timer, fina, &sig ]( boost::system::error_code ec )
	{
		if ( ec == 0 )
		{
			global_cloudnodes_update( sig, fina ) ;
		}
	} ) ;
}

// 同步节点最新数据
void sync_cloudnodes( msgsig_t& sig )
{
	auto fina = shared_ptr<void>( nullptr, [ &sig ](void*)
	{
		if ( global_pc().get_ios().stopped() == false )
		{
			plan_cloudnodes_update( sig ) ;
		}
	} ) ;

	auto task_sync = [ fina ]( auto ptr_service )
	{
		auto&& service = *ptr_service ;
		if ( service_update( service ) )
		{
			dbmeta_cloudnode cn ;
			cn.from_meta( service ) ;
			global_cloudnode_fix( cn ) ;
		}
	} ;

	for ( auto&& node : global_cloudnodes_recver() )
	{
		auto ptr_service = std::make_shared< decltype( node.to_meta() ) >( node.to_meta() ) ;
		global_pc().post( std::bind( task_sync, ptr_service ) ) ;
	}
}

QString ToQString(const string& cstr)  
{  
	QTextCodec* pCodec = QTextCodec::codecForName("gbk");  
	if(!pCodec) return "";  
  
	return pCodec->toUnicode(cstr.c_str(), cstr.size());  
} 

int main( int argc, char *argv[] )
{
	QApplication app( argc, argv ) ;
/*    MessageScrollWidget msw ;
    msw.setStyleSheet(".QWidget{background:lightgreen; border:2px groove gray;border-radius:10px;padding:2px 4px;}");
    msw.show() ;

    QPixmap map1;
    map1.load(":/res/1.png");

    QString nick1("jack");
    QPixmap map2;
    map2.load(":/res/2.png");
    msw.addImageMsg(map1, nick1, map2);

    QString nick2("rose");
    msw.addTextMsg(map1, nick2, nick1);

    QPixmap map3(":/res/3.png");
    msw.addImageMsg(map1, nick2, map3);

//    QPixmap map4(":/res/logo.png");
//    msw.addImageMsg(map1, nick2, map4);



    app.exec() ;
	app.exec() ;*/
	auto pLogo = new QSplashScreen( QPixmap( ":/res/logo.png" ) ) ;
	pLogo->show() ;
	app.processEvents() ;
	
	pLogo->showMessage( "Initialization Start Engines ...", Qt::AlignRight | Qt::AlignBottom, Qt::yellow ) ;
	ParallelCore pc_task( 6 ) ;
	global_pc( &pc_task ) ;
	boost::this_thread::sleep( boost::posix_time::milliseconds( 300 ) ) ;
	
	pLogo->showMessage( "Initialization SQLite ...", Qt::AlignRight | Qt::AlignBottom, Qt::yellow ) ;
	ORMapper db( ( boost::filesystem::system_complete( __argv[ 0 ] ).remove_filename() /= ( "echat.db" ) ).generic_string() ) ;
	global_db( &db ) ;
	boost::this_thread::sleep( boost::posix_time::milliseconds( 300 ) ) ;

	pLogo->showMessage( "Ready Widgets ...", Qt::AlignRight | Qt::AlignBottom, Qt::yellow ) ;
	MainForm wMain ;
    //eChatWidget wEdit(nullptr);
	boost::this_thread::sleep( boost::posix_time::milliseconds( 300 ) ) ;

	msgsig_t sig ;
	sig.connect( [ &wMain ]( string msg, service_meta service, file_meta fm, int64_t number )
	{
		vector<string> meta ;
		boost::algorithm::split( meta, msg, boost::is_any_of( "?" ) ) ;
		if ( meta.size() != 4 )
		{
			return ;
		}

		const auto id = meta[ 3 ] ;
		if ( meta[ 1 ] == "text" )
		{
			auto txt = email_content( service.pop3.c_str(),
				service.user.c_str(), service.pawd.c_str(), number ) ;
			if ( !txt.empty() )
			{
				wMain.m_pChatWidget->m_pscrollWidget->
					addTextMsg( QPixmap(":/res/audiomode.png"),
						ToQString( fm.tag ),
						ToQString( txt ) ) ;
			}

			return ;
		}

		if ( meta[ 1 ] == "image" )
		{
			string img ;
			{
				QTemporaryFile tmp_file( QDir::tempPath() + QDir::separator() +
					QCoreApplication::applicationName() + "_XXXXXX." + ToQString( meta[ 2 ] ) ) ;
				if ( tmp_file.open() )
				{
					img = tmp_file.fileName().toLocal8Bit().toStdString() ;
					tmp_file.setAutoRemove( true ) ;
					tmp_file.close() ;
				}
			}

			if ( file_create( img.c_str(), fm.bytes ) != 0 )
			{
				return ;
			}

			auto auto_remove = file_plan_remove( img.c_str() ) ;

			if ( email_recv(
				service.pop3.c_str(),
				service.user.c_str(),
				service.pawd.c_str(),
				number, img.c_str(), 0, fm.bytes, 0 ) == 200 )
			{
				wMain.m_pChatWidget->m_pscrollWidget->
				addImageMsg( QPixmap(":/res/audiomode.png"),
					ToQString( fm.tag ),
					QPixmap( ToQString( img ) ) ) ;
			}

			return ;
		}

		if ( meta[ 1 ] == "audio" )
		{
			string img ;
			{
				QTemporaryFile tmp_file( QDir::tempPath() + QDir::separator() +
					QCoreApplication::applicationName() + "_XXXXXX." + ToQString( meta[ 2 ] ) ) ;
				if ( tmp_file.open() )
				{
					img = tmp_file.fileName().toLocal8Bit().toStdString() ;
					tmp_file.setAutoRemove( true ) ;
					tmp_file.close() ;
				}
			}

			if ( file_create( img.c_str(), fm.bytes ) != 0 )
			{
				return ;
			}

			if ( email_recv(
				service.pop3.c_str(),
				service.user.c_str(),
				service.pawd.c_str(),
				number, img.c_str(), 0, fm.bytes, 0 ) == 200 )
			{
				wMain.m_pChatWidget->m_pscrollWidget->
					addVoiceMsg( QPixmap(":/res/audiomode.png"),
						ToQString( fm.tag ),
						ToQString( img ) ) ;
			}

			return ;
		}
	} ) ;
	
	pLogo->showMessage( "Initialization Tasks ...", Qt::AlignRight | Qt::AlignBottom, Qt::yellow ) ;
	sync_cloudnodes( sig ) ;
	boost::this_thread::sleep( boost::posix_time::milliseconds( 300 ) ) ;

	pLogo->showMessage( "Welcome!", Qt::AlignRight | Qt::AlignBottom, Qt::yellow ) ;
	boost::this_thread::sleep( boost::posix_time::milliseconds( 300 ) ) ;
	delete pLogo ;

    wMain.show() ;
	//wMain.show() ;
	app.exec() ;
	
	global_pc().stop() ;
	return 0 ;
}
