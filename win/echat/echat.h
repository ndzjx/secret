
#pragma once

#include <mod_db.h>
using namespace secret ;

using msgsig_t = boost::signals2::signal<void(string, service_meta, file_meta, int64_t)> ;
constexpr int64_t CHUNK_SIZE = 1024 * 1024 * 8 ;

// ���е�����
ParallelCore& global_pc( ParallelCore* pc ) ;
ParallelCore& global_pc() ;

// ���ݿ����
ORMapper& global_db( ORMapper* db ) ;
ORMapper& global_db() ;

// ��ȡ�ƶ����нڵ�
vector<dbmeta_cloudnode> global_cloudnodes_all() ;
vector<dbmeta_cloudnode> global_cloudnodes_sender() ;
vector<dbmeta_cloudnode> global_cloudnodes_recver() ;

// ���ɷ��ͼ�Ⱥ��ַ
string global_cloudnodes_dist() ;

// �����ƶ�״̬
void global_cloudnodes_update( msgsig_t& sig, std::shared_ptr<void> fina ) ;

// ����ƶ˽ڵ�
bool global_cloudnode_add( const dbmeta_cloudnode& dbnode ) ;

// ɾ���ƶ˽ڵ�
bool global_cloudnode_del( const dbmeta_cloudnode& dbnode ) ;

// �����ƶ˽ڵ�
bool global_cloudnode_fix( const dbmeta_cloudnode& dbnode ) ;

// ��ȡ�ڵ�
bool global_cloudnode_get( const string& user, dbmeta_cloudnode& dbnode ) ;

// ��ȡ�ƶ������ļ�
vector<dbmeta_cloudfile> global_cloudfiles() ;

// ��ȡȥ������Ŀ¼
map< pair<string, string>, string > global_clouddirs() ;

// ����ļ��Ƿ����ƶ˴���
bool global_cloudfile_exist( const string& id ) ;

// ����ļ����Ƿ����ƶ˴���
bool global_cloudchunk_exist( const dbmeta_cloudfile& chunk ) ;

// ���ƶ������ļ�
bool global_cloudfile_download( const string& id, const string& file ) ;
