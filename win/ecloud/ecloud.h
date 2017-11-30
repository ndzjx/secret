
#pragma once

#include <mod_db.h>
using namespace secret ;

// ���е�����
ParallelCore& global_pc( ParallelCore* pc ) ;
ParallelCore& global_pc() ;

// ���ݿ����
ORMapper& global_db( ORMapper* db ) ;
ORMapper& global_db() ;

// ��ȡ�ƶ����нڵ�
vector<dbmeta_cloudnode> global_cloudnodes() ;

// �����ƶ�״̬
void global_cloudnodes_update( std::shared_ptr<void> fina ) ;

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

// ����ļ��Ƿ����ƶ˴���
bool global_cloudfile_exist( const string& id ) ;

// ���ƶ������ļ�
bool global_cloudfile_download( const string& id, const string& file ) ;
