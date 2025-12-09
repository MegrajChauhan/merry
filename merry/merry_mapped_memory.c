#include <merry_mapped_memory.h>

mresult_t merry_mapped_memory_create(MerryMappedMemory **mem) {
	if (!mem)
		return MRES_INVALID_ARGS;
	if ((*mem)->map)
		return MRES_NOT_ALLOWED;
	MerryMappedMemory *m;
	mresult_t res;
	if ((res = merry_interface_init(&m, INTERFACE_TYPE_MAPPED_MEMORY)) != MRES_SUCCESS)
		return res;
	m->memory.map = NULL;
	m->memory.len = 0;
	m->memory.refr_count = 0;
	m->memory.flags.share = 0;
	*mem = m;
	return MRES_SUCCESS;
}

mresult_t merry_mapped_memory_map(MerryMappedMemory *mem, msize_t len, msize_t flags) {
	if (!mem || len == 0)
		return MRES_INVALID_ARGS;
	if (mem->interface_t != INTERFACE_TYPE_MAPPED_MEMORY)
		return MRES_CONFIGURATION_INVALID;
	if (mem->memory.map)
		return MRES_NOT_ALLOWED;
#ifdef _USE_LINUX_
	int f = MAP_ANONYMOUS | MAP_PRIVATE;
	int prot = PROT_READ | PROT_WRITE;
	if ((mem->memory.map = mmap(NULL, len, prot, f)) == NULL) 
		return MRES_SYS_FAILURE;
#endif
    mem->memory.len = len;
    mem->memory.refr_count++;
    mem->memory.flags.share = (flags & _MERRY_MEMORY_MAPPED_SHAREABLE_);
    return MRES_SUCCESS;
}

mresult_t merry_mapped_memory_unmap(MerryMappedMemory *mem) {
	if (!mem)
		return MRES_INVALID_ARGS;
	if (mem->interface_t != INTERFACE_TYPE_MAPPED_MEMORY)
		return MRES_CONFIGURATION_INVALID;
	if (!mem->memory.map)
		return MRES_NOT_ALLOWED;
	if (mem->memory.refr_count > 1) {
		mem->memory.refr_count--;
		return MRES_SUCCESS;
	}
#ifdef _USE_LINUX_
	munmap(mem->memory.map, mem->memory.len);
#endif
	mem->memory.map = NULL;
	return MRES_SUCCESS;
}

mresult_t merry_mapped_memory_destroy(MerryMappedMemory *mem) {
	if (!mem)
		return MRES_INVALID_ARGS;
	if (mem->interface_t != INTERFACE_TYPE_MAPPED_MEMORY)
		return MRES_CONFIGURATION_INVALID;
	if (mem->memory.map)
		return MRES_NOT_ALLOWED;
	if (mem->memory.refr_count > 0) 
		return MRES_NOT_ALLOWED;
	merry_interface_destroy(mem);
	return MRES_SUCCESS;
}
