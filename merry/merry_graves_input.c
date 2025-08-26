#include "merry_graves_input.h"

mbool_t merry_graves_reader_confirm_input_file(MerryGravesInput *reader,
                                               MerryErrorStack *st) {
  merry_check_ptr(reader);

  reader->fd = fopen(reader->file_path, "rb");
  if (!reader->fd) {
    if (errno == EISDIR) {
      PUSH(st, "File is directory", "Failed to read input file",
           "Opening Input File");
      merry_error_stack_fatality(st);
      return mfalse;
    }
    PUSH(st, "File Doesn't Exist", "Failed to read input file",
         "Opening Input File");
    merry_error_stack_fatality(st);
    return mfalse;
  }

  fseek(reader->fd, 0, SEEK_END);
  reader->file_size = ftell(reader->fd);
  rewind(reader->fd);

  if (reader->file_size < _MERRY_MINIMUM_NORMAL_INPUT_FILE_SIZE_) {
    PUSH(st, "Improper File Structure", "Not much information provided",
         "Understanding the File Structure");
    merry_error_stack_fatality(st);
    fclose(reader->fd);
    return mfalse;
  }

  for (msize_t i = 0; i < __CORE_TYPE_COUNT; i++) {
    reader->_instruction_for_core_already_read[i] = mfalse;
  }

  return mtrue;
}

MerryGravesInput *merry_graves_initialize_reader(mstr_t inp_path,
                                                 MerryErrorStack *st) {
  merry_check_ptr(inp_path);

  MerryGravesInput *reader =
      (MerryGravesInput *)malloc(sizeof(MerryGravesInput));

  if (!reader) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "Initializing Reader");
    merry_error_stack_fatality(st);
    return RET_NULL;
  }

  reader->file_path = inp_path;

  if (!merry_graves_reader_confirm_input_file(reader, st)) {
    free(reader);
    return RET_NULL;
  }

  for (msize_t i = 0; i < __CORE_TYPE_COUNT; i++) {
    reader->iram[i] = NULL;
    reader->instruction_offsets[i] = NULL;
    reader->_instruction_for_core_already_read[i] = mfalse;
  }
  reader->data_ram = NULL;
  reader->data_offsets = NULL;
  reader->string_offsets = NULL;
  return reader;
}

mret_t merry_graves_reader_read_input(MerryGravesInput *reader,
                                      MerryErrorStack *st) {
  merry_check_ptr(reader);

  if (merry_graves_reader_parse_identification_header(reader, st) ==
      RET_FAILURE)
    goto __parsing_error;
  if (merry_graves_reader_parse_ITIT_header(reader, st) == RET_FAILURE)
    goto __parsing_error;
  if (merry_graves_reader_parse_data_and_string_header(reader) == RET_FAILURE)
    goto __parsing_error;
  if (merry_graves_reader_perform_checksum(reader, st) == RET_FAILURE)
    goto __parsing_error;
  if (merry_graves_reader_parse_ITIT(reader, st) == RET_FAILURE)
    goto __parsing_error;
  if (merry_graves_reader_parse_instruction_sections(reader, st) == RET_FAILURE)
    goto __parsing_error;
  if (merry_graves_reader_parse_data_type_metadata(reader, st) == RET_FAILURE)
    goto __parsing_error;
  if (merry_graves_reader_parse_data_section(reader, st) == RET_FAILURE)
    goto __parsing_error;
  if (merry_graves_reader_parse_string_section(reader, st) == RET_FAILURE)
    goto __parsing_error;
  if (merry_graves_reader_prep_memory(reader, st) == RET_FAILURE)
    goto __parsing_error;
  if (merry_graves_reader_load_instructions(
          reader, reader->itit.entries[0].type, 0, st) == RET_FAILURE)
    goto __parsing_error;
  if (merry_graves_reader_load_data(reader, 0, st) == RET_FAILURE)
    goto __parsing_error;

  return RET_SUCCESS;

__parsing_error:
  PUSH(st, NULL, "Parsing of input file failed.", "Parsing Input File");
  return RET_FAILURE;
}

void merry_graves_reader_destroy(MerryGravesInput *reader,
                                 MerryErrorStack *st) {
  merry_check_ptr(reader);
  merry_check_ptr(reader->fd);

  for (msize_t i = 0; i < __CORE_TYPE_COUNT; i++) {
    if (reader->iram[i] != NULL)
      merry_destroy_RAM(reader->iram[i], st);
    if (reader->instruction_offsets[i] != NULL)
      free(reader->instruction_offsets[i]);
  }

  if (reader->data_ram != NULL)
    merry_destroy_RAM(reader->data_ram, st);
  if (reader->itit.entries != NULL)
    free(reader->itit.entries);
  if (reader->data_offsets != NULL)
    free(reader->data_offsets);
  if (reader->string_offsets != NULL)
    free(reader->string_offsets);

  fclose(reader->fd);
  free(reader);
}

mret_t merry_graves_reader_parse_identification_header(MerryGravesInput *reader,
                                                       MerryErrorStack *st) {
  merry_check_ptr(reader);

  mbyte_t magic_bytes[4] = {0};
  fread((void *)magic_bytes, 1, 3, reader->fd);

  if (strcmp((mstr_t)magic_bytes, "beb") != 0) {
    PUSH(st, "Invalid Identification Header",
         "Unknown File type: magic bytes must 'beb'", "Parsing Input File");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }

  fread(&reader->metadata.type, 1, 1, reader->fd);

  switch (reader->metadata.type) {
  case _NORMAL_INPUT_FILE:
    break;
  default:
    PUSH(st, "Invalid File Type", "This file type is not supported",
         "Parsing Input File");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }

  msize_t tmp = 0;

  fread(&tmp, 1, 4, reader->fd);

  return RET_SUCCESS;
}

mret_t merry_graves_reader_parse_ITIT_header(MerryGravesInput *reader,
                                             MerryErrorStack *st) {
  merry_check_ptr(reader);
  MerryHostMemLayout le;
  le.whole_word = 0;
  fread(&le.whole_word, 8, 1, reader->fd);
#if _MERRY_BYTE_ORDER_ == _MERRY_BIG_ENDIAN_
  merry_LITTLE_ENDIAN_to_BIG_ENDIAN(&le);
#endif
  reader->metadata.ITIT_len = le.whole_word;

  if ((reader->metadata.ITIT_len % _MERRY_NORMAL_INPUT_FILE_ITIT_ENTRY_LEN_) !=
      0) {
    PUSH(st, "Invalid ITIT Header",
         "ITIT length is misaligned: It must be 16-byte aligned",
         "Parsing Input File");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }
  if ((reader->metadata.ITIT_len / _MERRY_NORMAL_INPUT_FILE_ITIT_ENTRY_LEN_) >
      __CORE_TYPE_COUNT) {
    PUSH(st, "Invalid File Structure",
         "Number of ITIT entries cannot be more than the number of supported "
         "core types",
         "Parsing Input File");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }

  return RET_SUCCESS;
}

mret_t
merry_graves_reader_parse_data_and_string_header(MerryGravesInput *reader) {
  merry_check_ptr(reader);
  MerryHostMemLayout le;
  fread(&le.whole_word, 8, 1, reader->fd);
#if _MERRY_BYTE_ORDER_ == _MERRY_BIG_ENDIAN_
  merry_LITTLE_ENDIAN_to_BIG_ENDIAN(&le);
#endif
  reader->metadata.data_section_len = le.whole_word;
  le.whole_word = 0;
  fread(&le.whole_word, 8, 1, reader->fd);
#if _MERRY_BYTE_ORDER_ == _MERRY_BIG_ENDIAN_
  merry_LITTLE_ENDIAN_to_BIG_ENDIAN(&le);
#endif
  reader->metadata.string_section_len = le.whole_word;

  return RET_SUCCESS;
}

mret_t merry_graves_reader_perform_checksum(MerryGravesInput *reader,
                                            MerryErrorStack *st) {
  merry_check_ptr(reader);

  // We read the DI length in here since only the length is of interest to us.
  MerryHostMemLayout le;
  fread(&le.whole_word, 8, 1, reader->fd);
#if _MERRY_BYTE_ORDER_ == _MERRY_BIG_ENDIAN_
  merry_LITTLE_ENDIAN_to_BIG_ENDIAN(&le);
#endif
  reader->metadata.DI_len = le.whole_word;

  if ((reader->metadata.DI_len + reader->metadata.ITIT_len +
       reader->metadata.data_section_len + reader->metadata.string_section_len +
       _MERRY_INPUT_FILE_HEADER_SIZE_ + _MERRY_DATA_METADATA_LENGTH_) >
      reader->file_size) {
    PUSH(st, "Inavlid File Structure",
         "File Information doesn't complement header information",
         "Parsing Input File");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }
  /// NOTE: This is not the correct check actually.
  /// We haven't even read the length of each instruction section.
  /// Thus, this is a pre-check.
  return RET_SUCCESS;
}

mret_t merry_graves_reader_parse_ITIT(MerryGravesInput *reader,
                                      MerryErrorStack *st) {
  merry_check_ptr(reader);

  reader->itit.entry_count =
      reader->metadata.ITIT_len / _MERRY_NORMAL_INPUT_FILE_ITIT_ENTRY_LEN_;
  reader->itit.entries = (MerryITITEntry *)malloc(sizeof(MerryITITEntry) *
                                                  reader->itit.entry_count);
  if (!reader->itit.entries) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "Allocating buffer for ITIT entries");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }

  reader->metadata.total_instructions_len = 0;

  // Now we have to parse each of the section.
  for (msize_t i = 0; i < reader->itit.entry_count; i++) {
    MerryHostMemLayout c_type;
    MerryHostMemLayout section_len;
    fread(&c_type.whole_word, 8, 1, reader->fd);
    fread(&section_len.whole_word, 8, 1, reader->fd);
#if _MERRY_BYTE_ORDER_ == _MERRY_BIG_ENDIAN_
    merry_LITTLE_ENDIAN_to_BIG_ENDIAN(&c_type);
    merry_LITTLE_ENDIAN_to_BIG_ENDIAN(&section_len);
#endif
    c_type.bytes.b7 = c_type.bytes.b7 & __CORE_TYPE_COUNT;
    if (reader->_instruction_for_core_already_read[c_type.bytes.b7] == mtrue) {
      PUSH(st, "Invalid File Structure",
           "Multiple ITIT entries for the same core type",
           "Parsing Input File");
      merry_error_stack_fatality(st);
      return RET_FAILURE;
    }
    if ((section_len.whole_word % 8) != 0) {
      PUSH(st, "Invalid File Structure",
           "Instructions are misaligned: Instructions must be 8-byte aligned",
           "Parsing Input File");
      merry_error_stack_fatality(st);
      return RET_FAILURE;
    }
    if (section_len.whole_word == 0) {
      PUSH(st, "Invalid File Structure",
           "Instruction section length cannot be 0", "Parsing Input File");
      merry_error_stack_fatality(st);
      return RET_FAILURE;
    }
    reader->_instruction_for_core_already_read[c_type.bytes.b7] = mtrue;
    reader->itit.entries[i].type = c_type.bytes.b7;
    reader->itit.entries[i].section_len = section_len.whole_word;
    reader->metadata.total_instructions_len += section_len.whole_word;
  }

  if ((reader->metadata.DI_len + reader->metadata.ITIT_len +
       reader->metadata.data_section_len + reader->metadata.string_section_len +
       reader->metadata.total_instructions_len +
       _MERRY_INPUT_FILE_HEADER_SIZE_ + _MERRY_DATA_METADATA_LENGTH_) >
      reader->file_size) {
    PUSH(st, "Inavlid File Structure",
         "File Information doesn't complement header information",
         "Parsing Input File");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }
  return RET_SUCCESS;
}

mret_t merry_graves_reader_parse_instruction_sections(MerryGravesInput *reader,
                                                      MerryErrorStack *st) {
  merry_check_ptr(reader);

  msize_t off = ftell(reader->fd);
  for (msize_t i = 0; i < reader->itit.entry_count; i++) {
    MerryITITEntry entry = reader->itit.entries[i];
    msize_t curr_section_len = entry.section_len;
    msize_t section_count =
        entry.section_len / _MERRY_PAGE_LEN_ +
        ((entry.section_len % _MERRY_PAGE_LEN_) > 0 ? 1 : 0);
    reader->instruction_offsets_count[entry.type] = section_count;

    if ((reader->instruction_offsets[entry.type] = (MerrySection *)malloc(
             sizeof(MerrySection) * section_count)) == NULL) {
      PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
           "Allocating memory to store instruction offsets");
      merry_error_stack_fatality(st);
      return RET_FAILURE;
    }
    for (msize_t j = 0; j < section_count; j++) {
      reader->instruction_offsets[entry.type][j].offset = off;
      reader->instruction_offsets[entry.type][j].section_length =
          ((curr_section_len - _MERRY_PAGE_LEN_) < 0 ? curr_section_len
                                                     : _MERRY_PAGE_LEN_);
      curr_section_len -= _MERRY_PAGE_LEN_;
      off += reader->instruction_offsets[entry.type][j].section_length;
    }
    fseek(reader->fd, entry.section_len, SEEK_CUR);
  }

  return RET_SUCCESS;
}

mret_t merry_graves_reader_parse_data_type_metadata(MerryGravesInput *reader,
                                                    MerryErrorStack *st) {
  merry_check_ptr(reader);

  // Exactly 24 bytes
  fread(&reader->qword.off_ed, 8, 1, reader->fd);
  fread(&reader->dword.off_ed, 8, 1, reader->fd);
  fread(&reader->word.off_ed, 8, 1, reader->fd);
#if _MERRY_BYTE_ORDER_ == _MERRY_BIG_ENDIAN_
  MerryHostMemLayout l;
  l.whole_word = reader->qword.off_ed;
  merry_LITTLE_ENDIAN_to_BIG_ENDIAN(&l);
  reader->qword = l.whole_word;

  l.whole_word = reader->dword.off_ed;
  merry_LITTLE_ENDIAN_to_BIG_ENDIAN(&l);
  reader->dword = l.whole_word;

  l.whole_word = reader->word.off_ed;
  merry_LITTLE_ENDIAN_to_BIG_ENDIAN(&l);
  reader->word = l.whole_word;
#endif
  if ((reader->qword.off_ed + reader->dword.off_ed + reader->word.off_ed) !=
      reader->metadata.data_section_len) {
    PUSH(st, "Inavlid File Structure",
         "Data Metadata Information doesn't complement parsed information",
         "Parsing Input File");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }
  if ((reader->qword.off_ed % 8) != 0) {
    PUSH(st, "Inavlid File Structure",
         "Misaligned QWORD data section: must be 8-byte aligned",
         "Parsing Input File");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }
  if ((reader->dword.off_ed % 4) != 0) {
    PUSH(st, "Inavlid File Structure",
         "Misaligned DWORD data section: must be 4-byte", "Parsing Input File");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }
  if ((reader->word.off_ed % 2) != 0) {
    PUSH(st, "Inavlid File Structure",
         "Misaligned WORD data section: must be 2-byte aligned",
         "Parsing Input File");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }
  return RET_SUCCESS;
}

mret_t merry_graves_reader_parse_data_section(MerryGravesInput *reader,
                                              MerryErrorStack *st) {
  merry_check_ptr(reader);
  merry_check_ptr(st);

  if (reader->metadata.data_section_len == 0)
    return RET_SUCCESS;
  msize_t off = ftell(reader->fd);
  msize_t section_count =
      reader->metadata.data_section_len / _MERRY_PAGE_LEN_ +
      ((reader->metadata.data_section_len % _MERRY_PAGE_LEN_) > 0 ? 1 : 0);
  reader->data_offsets_count = section_count;

  if ((reader->data_offsets = (MerrySection *)malloc(sizeof(MerrySection) *
                                                     section_count)) == NULL) {
    PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
         "Allocating memory to store data offsets");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }

  reader->qword.off_st = off;
  reader->qword.off_ed = reader->qword.off_ed + off;

  reader->dword.off_st = reader->qword.off_ed;
  reader->dword.off_ed = reader->dword.off_ed + reader->dword.off_st;

  reader->word.off_st = reader->dword.off_ed;
  reader->word.off_ed = reader->word.off_ed + reader->word.off_st;

  msize_t data_section_len = reader->metadata.data_section_len;

  for (msize_t i = 0; i < section_count; i++) {
    reader->data_offsets[i].offset = off;
    reader->data_offsets[i].section_length =
        ((int64_t)(data_section_len - _MERRY_PAGE_LEN_) < 0 ? data_section_len
                                                            : _MERRY_PAGE_LEN_);
    data_section_len -= reader->data_offsets[i].section_length;
    off += reader->data_offsets[i].section_length;
  }
  fseek(reader->fd, reader->metadata.data_section_len, SEEK_CUR);

  return RET_SUCCESS;
}

mret_t merry_graves_reader_parse_string_section(MerryGravesInput *reader,
                                                MerryErrorStack *st) {
  merry_check_ptr(reader);
  merry_check_ptr(st);

  if (reader->metadata.string_section_len == 0)
    return RET_SUCCESS;

  msize_t off = ftell(reader->fd);
  msize_t string_section_len = reader->metadata.string_section_len;
  msize_t last_data_page_len =
      reader->metadata.data_section_len % _MERRY_PAGE_LEN_;
  msize_t section_count = 0;
  msize_t i = 0;

  if (last_data_page_len > 0) {
    // The last page has some space left
    msize_t space_left = _MERRY_PAGE_LEN_ - last_data_page_len;
    if (string_section_len < space_left) {
      space_left = string_section_len;
      string_section_len = 0;
    } else {
      string_section_len -= space_left;
    }
    section_count = string_section_len / _MERRY_PAGE_LEN_ +
                    ((string_section_len % _MERRY_PAGE_LEN_) > 0 ? 1 : 0) + 1;

    if ((reader->string_offsets = (MerrySection *)malloc(
             sizeof(MerrySection) * (section_count))) == NULL) {
      PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
           "Allocating memory to store string offsets");
      merry_error_stack_fatality(st);
      return RET_FAILURE;
    }
    reader->string_offsets[i].section_length = space_left;
    reader->string_offsets[i].offset = off;
    off += space_left;
    reader->string_offsets_count = section_count;
    i++;
  } else {
    section_count = string_section_len / _MERRY_PAGE_LEN_ +
                    ((string_section_len % _MERRY_PAGE_LEN_) > 0 ? 1 : 0);

    if ((reader->string_offsets = (MerrySection *)malloc(
             sizeof(MerrySection) * section_count)) == NULL) {
      PUSH(st, "Memory Allocation Failure", "Failed to allocate memory",
           "Allocating memory to store string offsets");
      merry_error_stack_fatality(st);
      return RET_FAILURE;
    }
    reader->string_offsets_count = section_count;
  }
  for (; i < section_count; i++) {
    reader->string_offsets[i].offset = off;
    reader->string_offsets[i].section_length =
        ((int64_t)(string_section_len - _MERRY_PAGE_LEN_) < 0
             ? string_section_len
             : _MERRY_PAGE_LEN_);
    string_section_len -= reader->data_offsets[i].section_length;
    off += reader->data_offsets[i].section_length;
  }
  fseek(reader->fd, reader->metadata.string_section_len, SEEK_CUR);

  return RET_SUCCESS;
}

mret_t merry_graves_reader_prep_memory(MerryGravesInput *reader,
                                       MerryErrorStack *st) {
  merry_check_ptr(reader);

  if ((reader->data_ram = merry_create_RAM(
           reader->data_offsets_count + reader->string_offsets_count, st)) ==
      RET_NULL) {
    PUSH(st, NULL, "Failed to read input file", "Preparing Memory");
    return RET_FAILURE;
  }

  for (msize_t i = 0; i < reader->itit.entry_count; i++) {
    MerryITITEntry e = reader->itit.entries[i];
    if ((reader->iram[e.type] = merry_create_RAM(
             reader->data_offsets_count + reader->string_offsets_count, st)) ==
        RET_NULL)
      return RET_FAILURE;
  }

  return RET_SUCCESS;
}

mret_t merry_graves_reader_load_instructions(MerryGravesInput *reader,
                                             mcore_t c_type, msize_t pgnum,
                                             MerryErrorStack *st) {
  merry_check_ptr(reader);

  if (surelyF(!reader->iram[c_type])) {
    // The memory for this core type doesn't exist yet because
    // the input file didn't have any instructions for it
    PUSH(st, "Invalid Load Request",
         "Instruction load requested for core which wasn't provided in the "
         "input file",
         "Load Instruction");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }

  if (surelyF(pgnum >= reader->instruction_offsets_count[c_type])) {
    PUSH(st, "Invalid Load Request",
         "Instruction load requested for page that doesn't exist",
         "Load Instruction");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }

  MerryRAM *ram = reader->iram[c_type];
  if (ram->pages[pgnum]->init == mtrue)
    return RET_SUCCESS;
  if (merry_initialize_normal_memory_page(ram->pages[pgnum], st) ==
      RET_FAILURE) {
    PUSH(st, NULL, "Failed to load instruction", "Loading Instruction");
    return RET_FAILURE;
  }

  MerrySection section = reader->instruction_offsets[c_type][pgnum];

  fseek(reader->fd, section.offset, SEEK_SET);

  fread(ram->pages[pgnum]->buf, 8, section.section_length / 8, reader->fd);
#if _MERRY_BYTE_ORDER_ == _MERRY_BIG_ENDIAN_
  mqptr_t buf = (mqptr_t)ram->pages[pgnum]->buf;
  for (msize_t i = 0; i < (section.section_len / 8); i++) {
    MerryHostMemLayout le;
    le.whole_word = buf[i];
    merry_LITTLE_ENDIAN_to_BIG_ENDIAN(&le);
    buf[i] = le.whole_word;
  }
#endif

  return RET_SUCCESS;
}

mret_t merry_graves_reader_read_qword(MerryGravesInput *reader, msize_t pg_num,
                                      MerrySection *s, msize_t *tr) {
  // everything is confirmed before this function is called

  if (!(s->offset >= reader->qword.off_st && s->offset < reader->qword.off_ed))
    return RET_SUCCESS;

  // Get the size of how much to read
  // Since the call will be forwarded to dword and word
  // one after the other and the function won't be called
  // again for the same page, we can safely update 's'
  msize_t to_read = reader->qword.off_ed - s->offset;
  if (to_read > s->section_length) {
    to_read = s->section_length;
    s->section_length = 0;
  } else {
    s->section_length -= to_read;
    s->offset += to_read;
  }

  // 'fd' should be at the correct position
  fread(reader->data_ram->pages[pg_num]->buf, 8, to_read / 8, reader->fd);
#if _MERRY_BYTE_ORDER_ == _MERRY_BIG_ENDIAN_
  mqptr_t buf = (mqptr_t)reader->data_ram->pages[pg_num]->buf;
  for (msize_t i = 0; i < (to_read / 8); i++) {
    MerryHostMemLayout le;
    le.whole_word = buf[i];
    merry_LITTLE_ENDIAN_to_BIG_ENDIAN(&le);
    buf[i] = le.whole_word;
  }
#endif
  // Now, for this we have successfully read how much should be read.

  // must provide to_read back to the callee for the next function
  *tr = to_read; // at this point, it is more appropriate to call it
                 // 'read' as in the past tense
  return RET_SUCCESS;
}

mret_t merry_graves_reader_read_dword(MerryGravesInput *reader, msize_t pg_num,
                                      MerrySection *s, msize_t *tr) {
  if (!(s->offset >= reader->dword.off_st && s->offset < reader->dword.off_ed))
    return RET_SUCCESS;

  msize_t to_read = reader->dword.off_ed - s->offset;
  if (to_read > s->section_length) {
    to_read = s->section_length;
    s->section_length = 0;
  } else {
    s->section_length -= to_read;
    s->offset += to_read;
  }

  fread((reader->data_ram->pages[pg_num]->buf + *tr), 4, to_read / 4,
        reader->fd);
#if _MERRY_BYTE_ORDER_ == _MERRY_BIG_ENDIAN_
  mdptr_t buf = (mdptr_t)((mbptr_t)reader->data_ram->pages[pg_num]->buf + *tr);
  for (msize_t i = 0; i < (to_read / 4); i++) {
    MerryHostMemLayout l, r;
    l.whole_word = *buf;
    r.bytes.b7 = l.bytes.b4;
    r.bytes.b6 = l.bytes.b5;
    r.bytes.b5 = l.bytes.b6;
    r.bytes.b4 = l.bytes.b7;
    *buf = r.w1;
  }
#endif
  *tr += to_read; // at this point, it is more appropriate to call it
  return RET_SUCCESS;
}

mret_t merry_graves_reader_read_word(MerryGravesInput *reader, msize_t pg_num,
                                     MerrySection *s, msize_t *tr) {
  if (!(s->offset >= reader->word.off_st && s->offset < reader->word.off_ed))
    return RET_SUCCESS;

  msize_t to_read = reader->word.off_ed - s->offset;
  if (to_read > s->section_length) {
    to_read = s->section_length;
    s->section_length = 0;
  } else {
    s->section_length -= to_read;
    s->offset += to_read;
  }

  fread((reader->data_ram->pages[pg_num]->buf + *tr), 2, to_read / 2,
        reader->fd);
#if _MERRY_BYTE_ORDER_ == _MERRY_BIG_ENDIAN_
  mwptr_t buf = (mwptr_t)((mbptr_t)reader->data_ram->pages[pg_num]->buf + *tr);
  for (msize_t i = 0; i < (to_read / 2); i++) {
    MerryHostMemLayout l, r;
    l.whole_word = *buf;
    r.bytes.b7 = l.bytes.b6;
    r.bytes.b6 = l.bytes.b7;
    *buf = r.word.w4;
  }
#endif
  *tr += to_read; // at this point, it is more appropriate to call it
  return RET_SUCCESS;
}

mret_t merry_graves_reader_load_data(MerryGravesInput *reader, msize_t pgnum,
                                     MerryErrorStack *st) {
  merry_check_ptr(reader);

  if (surelyF(pgnum >=
              (reader->data_offsets_count + reader->string_offsets_count))) {
    PUSH(st, "Invalid Load Request",
         "Data load requested for page that doesn't exist", "Load Data");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }
  MerryRAM *ram = reader->data_ram;
  MerrySection section;

  if (ram->pages[pgnum]->init == mtrue)
    return RET_SUCCESS;
  if (merry_initialize_normal_memory_page(ram->pages[pgnum], st) ==
      RET_FAILURE) {
    PUSH(st, NULL, "Failed to load data", "Loading Data");
    return RET_FAILURE;
  }

  // The section could be within the data part only
  // or in junction of data and string
  // or entirely on the string part as well
  if (pgnum < reader->data_offsets_count) {
    // all of it is in data part only
    msize_t tr = 0;
    section = reader->data_offsets[pgnum];
    merry_graves_reader_read_qword(reader, pgnum, &section, &tr);
    merry_graves_reader_read_dword(reader, pgnum, &section, &tr);
    merry_graves_reader_read_word(reader, pgnum, &section, &tr);
    // everything should be read
  } else if (pgnum == reader->data_offsets_count) {
    // This is at the junction
    // i.e some data is in the string section while some is on
    // the data section
    msize_t tr = 0;
    section = reader->data_offsets[pgnum - 1];
    merry_graves_reader_read_qword(reader, pgnum, &section, &tr);
    merry_graves_reader_read_dword(reader, pgnum, &section, &tr);
    merry_graves_reader_read_word(reader, pgnum, &section, &tr);

    // if there is anything left, then read it from the string section
    if (reader->string_offsets_count > 0) {
      section = reader->string_offsets[0];
      fread((ram->pages[pgnum]->buf + tr), 1, section.section_length,
            reader->fd); // read the rest to fill the page
    }
  } else {
    if (reader->string_offsets_count == 0) {
      // This is an error and unreachable code
      merry_unreachable("How did we get here?", NULL);
    }
    section = reader->string_offsets[pgnum - reader->data_offsets_count];
    fseek(reader->fd, section.offset, SEEK_SET);
    fread(ram->pages[pgnum]->buf, 1, section.section_length, reader->fd);
  }

  // done.... probably?
  return RET_SUCCESS;
}

MerryRAM *merry_graves_reader_get_data_RAM(MerryGravesInput *reader,
                                           MerryErrorStack *st) {
  MerryRAM *ram = merry_copy_RAM(reader->data_ram, st);
  if (!ram) {
    PUSH(st, NULL, "Failed to get a private DataRAM copy", "Copying DataRAM");
    return RET_NULL;
  }
  return ram;
}
