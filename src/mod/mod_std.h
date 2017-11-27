
#ifndef MOD_STD
#define MOD_STD

#include <io.h>
#include <thread>
#include <future>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <iostream>
using namespace std ;

template<class T>
inline auto plan_make( T plan )
{
	return std::shared_ptr<void>( nullptr, [ plan ]( void* ){ return plan() ; } ) ;
}

#endif
