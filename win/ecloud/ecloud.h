
#pragma once

#include <mod_db.h>
using namespace secret ;

// ���е�����
ParallelCore& global_pc( ParallelCore* pc ) ;
ParallelCore& global_pc() ;

// ���ݿ����
ORMapper& global_db( ORMapper* db ) ;
ORMapper& global_db() ;

// ����ļ��Ƿ����ƶ˴���
bool global_cloudfile_exist( const string& id ) ;

// ��ȡ�ƶ����нڵ�
vector<service_meta> global_cloudnodes() ;

// ���ƶ������ļ�
bool global_cloudfile_download( const string& id, const string& file ) ;
