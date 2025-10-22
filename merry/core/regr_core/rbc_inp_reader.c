#include <regr_core/comp/inp/rbc_inp_reader.h>

_MERRY_INTERNAL_ mret_t rbc_input_parse_header(RBCInput *inp, msize_t flen) {
  // inp will be valid
  mbyte_t chunk[8] = {0};
  MerryHostMemLayout dlen, ilen, dbg_len;
  dlen.whole_word = 0;
  ilen.whole_word = 0;
  dbg_len.whole_word = 0;
  // If the IO operations performed here ever fail, the only reason would be
  // INTERFACE_HOST_FAILURE since the interface should be perfectly configured.
  minterfaceRet_t ret;

  // Read the header
  if ((ret = merry_file_read(inp->input_file, chunk, 8)) != INTERFACE_SUCCESS)
    goto OPERATION_FAILURE;

  // The magic bytes for RBC is RIF(RBC Input File)
  if (chunk[0] != 'R' || chunk[1] != 'I' || chunk[2] != 'F') {
    MLOG("RBC",
         "Unknown Input File Type received: The IDENTIFICATION bytes %b%b%b "
         "expected but got %b%b%b",
         'R', 'I', 'F', chunk[0], chunk[1], chunk[2]);
    return RET_FAILURE;
  }

  if (chunk[3] != _MERRY_BYTE_ORDER_) {
    MFATAL("RBC",
           "Mismatched ENDIANNESS. The host and the inpu file must have the "
           "same endianness.",
           NULL);
    return RET_FAILURE;
  }

  // Now time for the lengths
  if ((ret = merry_file_read(inp->input_file, chunk, 8)) != INTERFACE_SUCCESS)
    goto OPERATION_FAILURE;

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
    return RET_FAILURE;
  }
  // Must be divisible by 8
  if (ilen.whole_word % 8 != 0) {
    MLOG("RBC",
         "Mis-aligned instruction section length: Must be divisible by 8",
         NULL);
    return RET_FAILURE;
  }

  if ((ret = merry_file_read(inp->input_file, chunk, 8)) != INTERFACE_SUCCESS)
    goto OPERATION_FAILURE;

  dlen.bytes.b0 = chunk[0];
  dlen.bytes.b1 = chunk[1];
  dlen.bytes.b2 = chunk[2];
  dlen.bytes.b3 = chunk[3];
  dlen.bytes.b4 = chunk[4];
  dlen.bytes.b5 = chunk[5];
  dlen.bytes.b6 = chunk[6];
  dlen.bytes.b7 = chunk[7];

  if ((ret = merry_file_read(inp->input_file, chunk, 8)) != INTERFACE_SUCCESS)
    goto OPERATION_FAILURE;

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
    return RET_FAILURE;
  }

  inp->data_len = dlen.whole_word;
  inp->instruction_len = ilen.whole_word;

  return RET_SUCCESS;
OPERATION_FAILURE:
  switch (ret) {
  case INTERFACE_HOST_FAILURE:
    MLOG("RBC", "Failed to parse the header for input file(Host Failure): %s",
         strerror(errno));
    return RET_FAILURE;
  default:
    merry_unreachable();
  }
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

mret_t rbc_input_read(RBCInput *inp, mstr_t path) {
  merry_check_ptr(inp);
  merry_check_ptr(path);

  mbool_t res = mfalse;
  inp->input_file = merry_open_file(path, _MERRY_FOPEN_READ_, 0, &res);
  if (!inp->input_file) {
    if (!res) {
      MFATAL("RBC", "Failed to read input file: FILE=%s because %s", path,
             strerror(errno));
      return RET_FAILURE;
    } else {
      merry_unreachable();
    }
  }
  msize_t fsize = 0;
  if (merry_file_size(inp->input_file, &fsize) != INTERFACE_SUCCESS)
    merry_unreachable();
  if (fsize == 0) {
    MFATAL("RBC", "Empty Input File! Nothing to execute! FILE=%s", path);
    merry_destroy_file(inp->input_file);
    return RET_FAILURE;
  }

  if (rbc_input_parse_header(inp, fsize) != RET_SUCCESS) {
    MLOG("RBC", "While parsing input file: PATH=%s", path);
    merry_destroy_file(inp->input_file);
    return RET_FAILURE;
  }

  msize_t original_ilen = inp->instruction_len;

  inp->instruction_len =
      rbc_align(inp->instruction_len, _RBC_PAGE_LEN_IN_BYTES_);
  inp->data_len = rbc_align(inp->data_len, _RBC_PAGE_LEN_IN_BYTES_);

  msize_t curr_pos = 0;

  if (merry_file_tell(inp->input_file, &curr_pos) != INTERFACE_SUCCESS) {
    // Most likely INTERFACE_HOST_FAILURE
    MFATAL("RBC",
           "Couldn't deduce file position for some reason: PATH=%s, ERRNO=%s",
           path, strerror(errno));
    merry_destroy_file(inp->input_file);
    return RET_FAILURE;
  }

  // Now finally allocate the memory
  inp->instructions = (mbptr_t)merry_get_anonymous_memory(inp->instruction_len);
  if (!inp->instructions) {
    MFATAL("RBC", "Failed to obtain memory for instructions: PATH=%s", path);
    merry_destroy_file(inp->input_file);
    return RET_FAILURE;
  }

  if (merry_map_file_explicit(inp->instructions, curr_pos, inp->instruction_len,
                              inp->input_file) == RET_FAILURE) {
    MFATAL("TC", "Failed to map memory for execution: PATH=%s", path);
    merry_destroy_file(inp->input_file);
    merry_return_memory(inp->instructions, inp->instruction_len);
    return RET_FAILURE;
  }

  inp->data = (mbptr_t)merry_get_anonymous_memory(inp->data_len);
  if (!inp->data) {
    MFATAL("RBC", "Failed to obtain memory for data: PATH=%s", path);
    merry_return_memory(inp->instructions, inp->instruction_len);
    merry_destroy_file(inp->input_file);
    return RET_FAILURE;
  }

  if (merry_map_file_explicit(inp->instructions, curr_pos + original_ilen,
                              inp->data_len, inp->input_file) == RET_FAILURE) {
    MFATAL("RBC", "Failed to map memory for execution: PATH=%s", path);
    merry_destroy_file(inp->input_file);
    merry_return_memory(inp->instructions, inp->instruction_len);
    merry_return_memory(inp->data, inp->data_len);
    return RET_FAILURE;
  }
  return RET_SUCCESS;
}

void rbc_input_destroy(RBCInput *inp) {
  merry_check_ptr(inp);
  merry_check_ptr(inp->input_file);
  merry_check_ptr(inp->data);
  merry_check_ptr(inp->instructions);

  merry_return_memory(inp->data, inp->data_len);
  merry_return_memory(inp->instructions, inp->instruction_len);
  merry_destroy_file(inp->input_file);
  free(inp);
}
