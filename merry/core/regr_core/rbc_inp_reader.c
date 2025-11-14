#include <regr_core/comp/inp/rbc_inp_reader.h>

_MERRY_INTERNAL_ rbcret_t rbc_input_parse_header(RBCInput *inp, msize_t flen,
                                                 MerryICRes *res) {
  // inp will be valid
  mbyte_t chunk[8] = {0};
  MerryHostMemLayout dlen, ilen, dbg_len;
  dlen.whole_word = 0;
  ilen.whole_word = 0;
  dbg_len.whole_word = 0;
  // If the IO operations performed here ever fail, the only reason would be
  // INTERFACE_HOST_FAILURE since the interface should be perfectly configured.
  msize_t ret;
  mICResSource_t source;

  // Read the header
  if ((ret = merry_file_read(inp->input_file, chunk, 8)) != INTERFACE_SUCCESS) {
    source = IC_SOURCE_INTERFACE;
    goto OPERATION_FAILURE;
  }

  // The magic bytes for RBC is RIF(RBC Input File)
  if (chunk[0] != 'R' || chunk[1] != 'I' || chunk[2] != 'F') {
    MLOG("RBC",
         "Unknown Input File Type received: The IDENTIFICATION bytes %b%b%b "
         "expected but got %b%b%b",
         'R', 'I', 'F', chunk[0], chunk[1], chunk[2]);
    source = IC_SOURCE_CORE;
    ret = RBC_FINPUT_INVAL;
    goto OPERATION_FAILURE;
  }

  if (chunk[3] != _MERRY_BYTE_ORDER_) {
    MFATAL("RBC",
           "Mismatched ENDIANNESS. The host and the inpu file must have the "
           "same endianness.",
           NULL);
    source = IC_SOURCE_CORE;
    ret = RBC_FINPUT_INVAL;
    goto OPERATION_FAILURE;
  }

  // Now time for the lengths
  if ((ret = merry_file_read(inp->input_file, chunk, 8)) != INTERFACE_SUCCESS) {
    source = IC_SOURCE_INTERFACE;
    goto OPERATION_FAILURE;
  }

  ilen.bytes.b0 = chunk[0];
  ilen.bytes.b1 = chunk[1];
  ilen.bytes.b2 = chunk[2];
  ilen.bytes.b3 = chunk[3];
  ilen.bytes.b4 = chunk[4];
  ilen.bytes.b5 = chunk[5];
  ilen.bytes.b6 = chunk[6];
  ilen.bytes.b7 = chunk[7];

  if (ilen.whole_word == 0) {
    MLOG("RBC", "No instructions provided: Instruction section length is 0",
         NULL);
    source = IC_SOURCE_CORE;
    ret = RBC_FINPUT_INVAL;
    goto OPERATION_FAILURE;
  }
  // Must be divisible by 8
  if (ilen.whole_word % 8 != 0) {
    MLOG("RBC",
         "Mis-aligned instruction section length: Must be divisible by 8",
         NULL);
    source = IC_SOURCE_CORE;
    ret = RBC_FINPUT_INVAL;
    goto OPERATION_FAILURE;
  }

  if ((ret = merry_file_read(inp->input_file, chunk, 8)) != INTERFACE_SUCCESS) {
    source = IC_SOURCE_INTERFACE;
    goto OPERATION_FAILURE;
  }

  dlen.bytes.b0 = chunk[0];
  dlen.bytes.b1 = chunk[1];
  dlen.bytes.b2 = chunk[2];
  dlen.bytes.b3 = chunk[3];
  dlen.bytes.b4 = chunk[4];
  dlen.bytes.b5 = chunk[5];
  dlen.bytes.b6 = chunk[6];
  dlen.bytes.b7 = chunk[7];

  if ((ret = merry_file_read(inp->input_file, chunk, 8)) != INTERFACE_SUCCESS) {
    source = IC_SOURCE_INTERFACE;
    goto OPERATION_FAILURE;
  }

  dbg_len.bytes.b0 = chunk[0];
  dbg_len.bytes.b1 = chunk[1];
  dbg_len.bytes.b2 = chunk[2];
  dbg_len.bytes.b3 = chunk[3];
  dbg_len.bytes.b4 = chunk[4];
  dbg_len.bytes.b5 = chunk[5];
  dbg_len.bytes.b6 = chunk[6];
  dbg_len.bytes.b7 = chunk[7];
  if ((ilen.whole_word + dlen.whole_word + dbg_len.whole_word) > flen) {
    MLOG("RBC",
         "Input file header's information doesn't match with what was read",
         NULL);
    source = IC_SOURCE_CORE;
    ret = RBC_FINPUT_INVAL;
    goto OPERATION_FAILURE;
  }

  inp->data_len = dlen.whole_word;
  inp->instruction_len = ilen.whole_word;

  return RBC_SUCCESS;
OPERATION_FAILURE:
  res->source = source;
  res->_core_code = ret;
  switch (source) {
  case IC_SOURCE_CORE:
    if (ret == RBC_SYS_FAILURE)
      res->ERRNO = errno;
    break;
  case IC_SOURCE_MERRY:
    if (ret == MRES_SYS_FAILURE)
      res->ERRNO = errno;
    break;
  case IC_SOURCE_INTERFACE:
    if (ret == INTERFACE_HOST_FAILURE)
      res->ERRNO = errno;
    break;
  default:
    merry_unreachable();
  }
  MLOG("RBC", "Failed to parse the header for input file", NULL);
  return RBC_FAILURE;
}

RBCInput *rbc_input_init() {
  RBCInput *inp = (RBCInput *)malloc(sizeof(RBCInput));
  if (!inp) {
    MFATAL("RBC", "Failed to allocate memory for input", NULL);
    return RET_NULL;
  }
  inp->data = NULL;
  inp->data_len = 0;
  inp->input_file = NULL;
  inp->instruction_len = 0;
  inp->instructions = NULL;
  return inp;
}

rbcret_t rbc_input_read(RBCInput *inp, mstr_t path, MerryICRes *res) {
  merry_check_ptr(inp);
  merry_check_ptr(path);

  mICResSource_t source;
  msize_t ret =
      merry_open_file(&inp->input_file, path, _MERRY_FOPEN_READ_WRITE_, 0);
  if (ret != INTERFACE_SUCCESS) {
    MFATAL("RBC", "Failed to read input file: FILE=%s", path);
    source = IC_SOURCE_INTERFACE;
    goto RBC_INP_PARSE_FAILED;
  }
  msize_t fsize = 0;
  if ((ret = merry_file_size(inp->input_file, &fsize)) != INTERFACE_SUCCESS)
    merry_unreachable(); // should never fail
  if (fsize == 0) {
    MFATAL("RBC", "Empty Input File! Nothing to execute! FILE=%s", path);
    merry_destroy_file(inp->input_file);
    source = IC_SOURCE_CORE;
    goto RBC_INP_PARSE_FAILED;
  }

  if (rbc_input_parse_header(inp, fsize, res) != RET_SUCCESS) {
    MLOG("RBC", "While parsing input file: PATH=%s", path);
    merry_destroy_file(inp->input_file);
    return RBC_FAILURE;
  }

  inp->data_len = rbc_align(inp->data_len, _RBC_PAGE_LEN_IN_BYTES_);

  // Now finally allocate the memory
  msize_t total_len = inp->data_len + inp->instruction_len + 32;
  mptr_t mem;
  ret = merry_get_anonymous_memory(&mem, total_len);
  if (ret != MRES_SUCCESS) {
    MFATAL("RBC", "Failed to obtain memory for program: PATH=%s", path);
    merry_destroy_file(inp->input_file);
    source = IC_SOURCE_MERRY;
    goto RBC_INP_PARSE_FAILED;
  }

  if ((ret = merry_map_memory(&inp->mapped, mem, total_len)) != MRES_SUCCESS) {
    MFATAL("RBC", "Failed to obtain memory for program: PATH=%s", path);
    merry_destroy_file(inp->input_file);
    merry_return_memory(mem, total_len);
    source = IC_SOURCE_MERRY;
    goto RBC_INP_PARSE_FAILED;
  }

  // We need a much better way of doing this
  if ((ret = merry_map_file(inp->mapped->memory_map.map, inp->input_file)) !=
      MRES_SUCCESS) {
    MFATAL("RBC", "Failed to populate memory for execution: PATH=%s", path);
    merry_destroy_file(inp->input_file);
    merry_return_memory(inp->instructions, inp->instruction_len);
    source = IC_SOURCE_MERRY;
    goto RBC_INP_PARSE_FAILED;
  }
  inp->instructions = inp->mapped->memory_map.map + 32;
  inp->data = inp->mapped->memory_map.map + 32 + inp->instruction_len;

  return RBC_SUCCESS;
RBC_INP_PARSE_FAILED:
  res->source = source;
  res->_core_code = ret;
  switch (source) {
  case IC_SOURCE_CORE:
    if (ret == RBC_SYS_FAILURE)
      res->ERRNO = errno;
    break;
  case IC_SOURCE_MERRY:
    if (ret == MRES_SYS_FAILURE)
      res->ERRNO = errno;
    break;
  case IC_SOURCE_INTERFACE:
    if (ret == INTERFACE_HOST_FAILURE)
      res->ERRNO = errno;
    break;
  default:
    merry_unreachable();
  }
  MLOG("RBC", "Failed to parse the header for input file", NULL);
  return RBC_FAILURE;
}

void rbc_input_destroy(RBCInput *inp) {
  merry_check_ptr(inp);
  merry_check_ptr(inp->input_file);
  merry_check_ptr(inp->data);
  merry_check_ptr(inp->instructions);

  merry_unmap_memory(inp->mapped);
  merry_destroy_file(inp->input_file);
  free(inp);
}
