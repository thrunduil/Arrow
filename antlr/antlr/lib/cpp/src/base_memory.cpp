#include "base_memory.inl"
#include "memory_stats.inl"
#include "../data_str/ref_count/root_table.inl"
#include "allocator_map.h"
#include "allocator_map.h"
#include "pool.inl"
#include "global_lib/src/exception/base_exception.h"

namespace memory
{
void	free_garbage()
{
	release_memory();
	reference_counting::details::Root_Table::collect();
	release_memory();
};

double get_allocated_memory_size()
{
	return details::total_mem_stat::space_alloc - details::total_mem_stat::space_dealloc;
};

void release_memory()
{
	details::allocator_map::get()->release_memory();
};
namespace details
{
root_allocator::root_allocator(size_t size)
{
	assertion(sizeof(allocator_malloc)==4,"invalid block size");
	new (block_allocator) allocator_malloc();

	allocator_malloc* ptr = reinterpret_cast<allocator_malloc*>(block_allocator);
	assertion(sizeof(pool::details::pool)<40,"invalid block size");
	new (block_pool) pool::details::pool(ptr,size);

	details::allocator_map::get()->register_allocator(reinterpret_cast<pool::details::pool*>(block_pool));
};
root_allocator::~root_allocator()
{
	reinterpret_cast<pool::details::pool*>(block_pool)->~pool();
	reinterpret_cast<allocator_malloc*>(block_allocator)->~allocator_malloc();
};

void* root_allocator::base_ordered_malloc()
{
	return reinterpret_cast<pool::details::pool*>(block_pool)->ordered_malloc();
};
void* root_allocator::base_ordered_malloc(size_t N)
{
	return reinterpret_cast<pool::details::pool*>(block_pool)->ordered_malloc(N);
};
void root_allocator::base_ordered_free(void* ptr)
{
	return reinterpret_cast<pool::details::pool*>(block_pool)->ordered_free(ptr);
};
void root_allocator::base_ordered_free(void* ptr,size_t N)
{
	return reinterpret_cast<pool::details::pool*>(block_pool)->ordered_free(ptr,N);
};
void root_allocator::base_release_memory()
{
	reinterpret_cast<pool::details::pool*>(block_pool)->release_memory();
};

};
}; /* namespace memory */


