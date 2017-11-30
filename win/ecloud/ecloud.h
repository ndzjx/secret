
#pragma once

#include <mod_db.h>
using namespace secret ;

constexpr int64_t CHUNK_SIZE = 1024 * 1024 * 8 ;

// 并行调度器
ParallelCore& global_pc( ParallelCore* pc ) ;
ParallelCore& global_pc() ;

// 数据库访问
ORMapper& global_db( ORMapper* db ) ;
ORMapper& global_db() ;

// 获取云端所有节点
vector<dbmeta_cloudnode> global_cloudnodes() ;

// 更新云端状态
void global_cloudnodes_update( std::shared_ptr<void> fina ) ;

// 添加云端节点
bool global_cloudnode_add( const dbmeta_cloudnode& dbnode ) ;

// 删除云端节点
bool global_cloudnode_del( const dbmeta_cloudnode& dbnode ) ;

// 更新云端节点
bool global_cloudnode_fix( const dbmeta_cloudnode& dbnode ) ;

// 获取节点
bool global_cloudnode_get( const string& user, dbmeta_cloudnode& dbnode ) ;

// 获取云端所有文件
vector<dbmeta_cloudfile> global_cloudfiles() ;

// 检查文件是否在云端存在
bool global_cloudfile_exist( const string& id ) ;

// 检查文件块是否在云端存在
bool global_cloudchunk_exist( const dbmeta_cloudfile& chunk ) ;

// 从云端下载文件
bool global_cloudfile_download( const string& id, const string& file ) ;
