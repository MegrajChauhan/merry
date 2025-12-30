#include <regr_core/comp/inp/rbc_inp_reader.h>

_MERRY_INTERNAL_ mresult_t rbc_input_parse_header(RBCInput *inp, msize_t flen) {
  // inp will be valid
  mbyte_t chunk[8] = {0};
  MerryHostMemLayout dlen, ilen, dbg_len;
  dlen.whole_word = 0;
  ilen.whole_word = 0;
  dbg_len.whole_word = 0;
  // If the IO operations performed here ever fail, the only reason would be
  // INTERFACE_HOST_FAILURE since the interface should be perfectly configured.
  mresult_t ret;

  // Read the header
  if ((ret = merry_file_read(inp->input_file, chunk, 8)) != MRES_SUCCESS) {
    goto OPERATION_FAILURE;
  }

  // The magic bytes for RBC is RIF(RBC Input File)
  if (chunk[0] != 'R' || chunk[1] != 'I' || chunk[2] != 'F') {
    MLOG("RBC",
         "Unknown Input File Type received: The IDENTIFICATION bytes %b%b%b "
         "expected but got %b%b%b",
         'R', 'I', 'F', chunk[0], chunk[1], chunk[2]);
    ret = MRES_UNRECOGNIZED;
    goto OPERATION_FAILURE;
  }

  if (chunk[3] != _MERRY_BYTE_ORDER_) {
    MFATAL("RBC",
           "Mismatched ENDIANNESS. The host and the inpu file must have the "
           "same endianness.",
           NULL);
    ret = MRES_UNRECOGNIZED;
    goto OPERATION_FAILURE;
  }

  // Now time for the lengths
  if ((ret = merry_file_read(inp->input_file, chunk, 8)) != MRES_SUCCESS) {
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
    ret = MRES_UNRECOGNIZED;
    goto OPERATION_FAILURE;
  }
  // Must be divisible by 8
  if (ilen.whole_word % 8 != 0) {
    MLOG("RBC",
         "Mis-aligned instruction section length: Must be divisible by 8",
         NULL);
    ret = MRES_UNRECOGNIZED;
    goto OPERATION_FAILURE;
  }

  if ((ret = merry_file_read(inp->input_file, chunk, 8)) != MRES_SUCCESS) {
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

  if ((ret = merry_file_read(inp->input_file, chunk, 8)) != MRES_SUCCESS) {
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
    ret = MRES_UNRECOGNIZED;
    goto OPERATION_FAILURE;
  }

  inp->data_len = dlen.whole_word;
  inp->instruction_len = ilen.whole_word;
  return MRES_SUCCESS;
OPERATION_FAILURE:
  MLOG("RBC", "Failed to parse the header for input file", NULL);
  return ret;
}

RBCInput *rbc_input_init() {
  RBCInput *inp = (RBCInput *)malloc(sizeof(RBCInput));
  if (!inp) {
    MFATAL("RBC", "Failed to allocate memory for input", NULL);
    return RET_NULL;
  }
  if (merry_mapped_file_create(&inp->mapped) != MRES_SUCCESS) {
    free(inp);
    return RET_NULL;
  }
  inp->data = NULL;
  inp->data_len = 0;
  inp->input_file = NULL;
  inp->instruction_len = 0;
  inp->instructions = NULL;
  return inp;
}

mresult_t rbc_input_read(RBCInput *inp, mstr_t path) {
  merry_check_ptr(inp);
  merry_check_ptr(path);

  mresult_t ret =
      merry_open_file(&inp->input_file, path, _MERRY_FOPEN_READ_WRITE_, 0);
  if (ret != MRES_SUCCESS) {
    MFATAL("RBC", "Failed to read input file: FILE=%s", path);
    goto RBC_INP_PARSE_FAILED;
  }
  msize_t fsize = 0;
  if ((ret = merry_file_size(inp->input_file, &fsize)) != MRES_SUCCESS)
    merry_unreachable(); // should never fail
  if (fsize == 0) {
    MFATAL("RBC", "Empty Input File! Nothing to execute! FILE=%s", path);
    goto RBC_INP_PARSE_FAILED;
  }

  if ((ret = rbc_input_parse_header(inp, fsize, res)) != MRES_SUCCESS) {
    MLOG("RBC", "While parsing input file: PATH=%s", path);
    goto RBC_INP_PARSE_FAILED;
  }

  inp->data_len = rbc_align(inp->data_len, _RBC_PAGE_LEN_IN_BYTES_);

  // Now finally allocate the memory
  msize_t total_len = inp->data_len + inp->instruction_len + 32;

  if ((ret = merry_mapped_file_map(inp->mapped, path,
                                   _MERRY_MAPPED_FILE_ALIGN_FILE_LEN_,
                                   _RBC_PAGE_LEN_IN_BYTES_)) != MRES_SUCCESS) {
    goto RBC_INP_PARSE_FAILED;
  }

  ret = merry_mapped_file_obtain_ptr(inp->mapped, &inp->instructions, 32);

  if (ret != MRES_SUCCESS) {
    MFATAL("RBC", "Failed to obtain memory for program: PATH=%s", path);
    goto RBC_INP_PARSE_FAILED;
  }

  ret = merry_mapped_file_obtain_ptr(inp->mapped, &inp->data,
                                     32 + inp->instruction_len);

  if (ret != MRES_SUCCESS) {
    MFATAL("RBC", "Failed to obtain memory for program: PATH=%s", path);
    goto RBC_INP_PARSE_FAILED;
  }

  return MRES_SUCCESS;
RBC_INP_PARSE_FAILED:
  MLOG("RBC", "Failed to parse the header for input file", NULL);
  merry_destroy_file(inp->input_file);
  merry_mapped_file_unmap(inp->mapped);
  return ret;
}

void rbc_input_destroy(RBCInput *inp) {
  merry_check_ptr(inp);
  merry_check_ptr(inp->input_file);
  merry_check_ptr(inp->data);
  merry_check_ptr(inp->instructions);

  merry_mapped_file_unmap(inp->mapped);
  merry_mapped_file_destroy(inp->mapped);
  merry_destroy_file(inp->input_file);
  free(inp);
}
