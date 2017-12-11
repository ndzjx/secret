
#ifndef MOD_BOOST
#define MOD_BOOST

#define BOOST_COROUTINES_NO_DEPRECATION_WARNING

#ifdef _WIN32
#define _WIN32_WINNT 0x0501
#endif

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/signals2.hpp>

#endif
