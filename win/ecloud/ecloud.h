
#pragma once

#include <mod_db.h>
using namespace secret ;

// 并行调度器
ParallelCore& global_pc( ParallelCore* pc ) ;
ParallelCore& global_pc() ;

// 数据库访问
ORMapper& global_db( ORMapper* db ) ;
ORMapper& global_db() ;

// 检查文件是否在云端存在
bool global_cloudfile_exist( const string& id ) ;

// 获取云端所有节点
vector<service_meta> global_cloudnodes() ;

// 从云端下载文件
bool global_cloudfile_download( const string& id, const string& file ) ;
