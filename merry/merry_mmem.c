#include <merry_mmem.h>

mresult_t merry_map_memory(MerryMMem **mem, mptr_t map, msize_t len) {
  if (!map || !len)
    return MRES_INVALID_ARGS;
  *mem = (MerryMMem *)malloc(sizeof(MerryMMem));
  if (!(*mem)) {
    return MRES_SYS_FAILURE;
  }
  (*mem)->interface_t = INTERFACE_TYPE_MEM_MAP;
  (*mem)->memory_map.len = len;
  (*mem)->memory_map.map = map;
  (*mem)->memory_map.children_count = 0;
  (*mem)->memory_map.parent = NULL;
  return MRES_SUCCESS;
}
/*
minterfaceRet_t merry_sub_map_memory(MerryMMem *map, MerryMMem **res,
                                     mptr_t sub_map, msize_t len) {
  if (!map || !res || !sub_map || !len)
    return INTERFACE_INVALID_ARGS;

  if (map->interface_t != INTERFACE_TYPE_MEM_MAP)
    return INTERFACE_TYPE_INVALID;

  merry_map_memory(res, sub_map, len);
  if (!(*res))
    return INTERFACE_HOST_FAILURE;

  (*res)->memory_map.parent = map;
  map->memory_map.children_count++;
  return INTERFACE_SUCCESS;
}*/

minterfaceRet_t merry_unmap_memory(MerryMMem *map) {
  if (!map)
    return INTERFACE_INVALID_ARGS;
  if (map->interface_t != INTERFACE_TYPE_MEM_MAP)
    return INTERFACE_TYPE_INVALID;
  if (!map->memory_map.parent) {
    // This owns the map
    if (map->memory_map.children_count)
      return INTERFACE_ACTION_DANGEROUS;
    merry_return_memory(map->memory_map.map, map->memory_map.len);
  } else
    map->memory_map.parent->memory_map.children_count--;
  free(map);
  return INTERFACE_SUCCESS;
}
