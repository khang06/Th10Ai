#pragma once

#include "Th10Base/Common.h"

// 默认的通用interprocess_condition性能太低
#ifndef BOOST_INTERPROCESS_FORCE_NATIVE_EMULATION
#define BOOST_INTERPROCESS_FORCE_NATIVE_EMULATION
#endif

#include <boost/interprocess/managed_windows_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp> 
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

namespace th
{
	namespace ip = boost::interprocess;

	using SharedMemory = ip::managed_windows_shared_memory;
	using SegmentManager = SharedMemory::segment_manager;

	using VoidAllocator = ip::allocator<void, SegmentManager>;
	template <typename T>
	using Allocator = ip::allocator<T, SegmentManager>;

	template <typename T>
	using ManagedVector = ip::vector<T, Allocator<T>>;

	template <typename T>
	using ManagedPtr = ip::managed_unique_ptr<T, SharedMemory>::type;
}
