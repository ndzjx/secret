
#pragma once

#include <mod_db.h>
using namespace secret ;
ParallelCore& global_pc( ParallelCore* pc ) ;
ParallelCore& global_pc() ;
ORMapper& global_db( ORMapper* db ) ;
ORMapper& global_db() ;
void global_update_cloud() ;
bool global_cloudfile_exist( const string& id ) ;
vector<service_meta> global_cloudnodes() ;
bool global_cloudfile_download( const string& id, const string& file ) ;
void global_insert_cloudnode(const service_meta &data);
void global_delete_cloudnode(const service_meta &data);
vector<dbmeta_cloudfile> global_cloudfiles();

