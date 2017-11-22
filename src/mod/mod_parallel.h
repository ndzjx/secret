
#ifndef MOD_PARALLEL
#define MOD_PARALLEL

#include "mod_boost.h"

namespace secret {

// ParallelCore - 并行调度器
// 多线程并行模型
// 使用 post 发起一个任务, 该任务可能由任意一个线程执行
class ParallelCore : public boost::noncopyable
{
public:
	ParallelCore()
	{
		start() ;
	}

	~ParallelCore()
	{
		stop() ;
	}

public:
	void start()
	{
		auto hc = boost::thread::hardware_concurrency() ;
		if (0 == hc)
		{
			hc = 1;
		}
		while ( hc-- )
		{
			m_tg.create_thread( [=]{ m_ios.run() ; } ) ;
		}
	}

	void stop()
	{
		m_ios.stop() ;
		m_tg.join_all() ;
	}

	template<class T>
	auto post( T&& task )
	{
		return m_ios.post( task ) ;
	}

private:
	boost::asio::io_service			m_ios			;
	boost::asio::io_service::work	m_work{ m_ios }	;
	boost::thread_group				m_tg			;
};

// ParallelMapReduce - 并行任务
// 理论依据 MapReduce
// map 函数负责描述单项任务
// reduce 函数负责将 map 函数的返回值汇总处理
// 模板参数是 map 函数的返回值类型
template<class T_MAP>
class ParallelMapReduce : public boost::noncopyable
{
public:
	template<class T_TASK>
	auto map( ParallelCore& pc, T_TASK task )
	{
		auto result = make_shared<task_promise_t>() ;
		m_all.push_back( result ) ;
		pc.post( std::bind( task, result ) ) ;
		return m_all.size() ;
	}

	template<class T_TASK>
	auto reduce( T_TASK task ) const
	{
		for ( auto&& result : m_all )
		{
			task( result->get_future().get() ) ;
		}
	}

private:
	using task_promise_t	= std::promise<T_MAP> ;
	using task_result_t		= std::shared_ptr<task_promise_t> ;
	
	vector<task_result_t> m_all ;
};

}

#endif
