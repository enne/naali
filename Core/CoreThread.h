// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_CoreThread_h
#define incl_CoreThread_h

// Disable warning C4244 coming from boost
#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning( disable : 4244 )
#endif

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition.hpp>

#ifdef _MSC_VER
#pragma warning( pop )
#endif


//! generic mutex
typedef boost::mutex Mutex;
//! recursive mutex
typedef boost::recursive_mutex RecursiveMutex;
//! Raii style mutex locking
typedef boost::lock_guard<Mutex> MutexLock;
//! Raii style recursive mutex locking
typedef boost::lock_guard<RecursiveMutex> RecursiveMutexLock;
//! Scoped lock
typedef boost::mutex::scoped_lock ScopedLock;

//! Thread
typedef boost::thread Thread;
//! Condition
typedef boost::condition Condition;

#endif

