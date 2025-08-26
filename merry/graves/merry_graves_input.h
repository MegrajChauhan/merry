#ifndef _MERRY_GRAVES_INPUT_
#define _MERRY_GRAVES_INPUT_

/**
 * Format:
 * We have to consider that we have multiple core types i.e multiple ISAs to fit
 * into a single file. This is why we will make use of sections. Obviously all
 * of the cores share the same memory so data can have a single section.
 * Instructions will require separate sections for all core types or more
 * preciesly, separate Instruction Memory for each core type
 *
 * We also need to get some information about the type of the program that is
 * stored. Since, dynamic library loading would be awesome near future, we need
 * a flexible file format.
 *
 * The format should look something like this:
 * <IDENTIFICATION[Magic Numbers]> <File Type> -> Identification Header[We will
 * have a few bytes left to be used for something else if it comes to that]
 * <Instruction Type Identification Table Length> -> The number of bytes that
 * Instruction Type Identification Table will use <Data Section Length> -> The
 * number of bytes that the Data Section has <String Section Length> -> Contains
 * String[In Bytes] <Debugging Information Table Length> -> A custom section
 *
 * <ITIT>: [Core Type] [Length:8 bytes]
 * <Instruction Sections>
 * <Data Sections>
 * <String Section>
 * <Debugging Information>: [None of VM's business]
 *
 * A detailed explanation for each of the sections is as follow:
 * ITIT: This table provides information about all of the cores types being
 * used. [Core Type] hints at the core that will be executing this particular
 * section. [Length] provides the length of the section. The sections following
 * ITIT should be the instructions exactly as specified in ITIT in the exact
 * same order. Data Section: Contains all of the data that all of the cores are
 * going to share. Since all of the cores will be 64-bit in size despite the
 * names given, they all share the same data memory. String Section: Contains
 * strings only. Part of the data section. DI: This section is a sort of
 * "do-what-you-will" section. As Merry is planned to provide a debugging
 * framework which may be utilized by people to write debuggers. For this cases,
 * people may come up with conventions as to how this section should be
 * structured. Based on those standards and conventions, assemblers and
 * compilers may produce the necessary information. Thus, this is a customizable
 * section.
 *
 * It is also better to note that the contents of the file, except for the
 * Identification Header, must be in Little Endian format. Merry will handle
 * endian conversion if necessary.
 * */

#include <errno.h>
#include <merry_core_types.h>
#include <merry_error_stack.h>
#include <merry_graves_defs.h>
#include <merry_helpers.h>
#include <merry_ram.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct MerryGravesInput MerryGravesInput;
typedef struct MerryInstructionTypeIdentificationTableEntry MerryITITEntry;
typedef struct MerryInstructionTypeIdentificationTable MerryITIT;
typedef struct MerryFileMetadata MerryFileMetadata;
typedef struct MerrySection MerrySection;
typedef struct MerryPortion MerryPortion;

struct MerryInstructionTypeIdentificationTableEntry {
  mcore_t type;
  msize_t section_len;
};

struct MerryInstructionTypeIdentificationTable {
  MerryITITEntry *entries;
  msize_t entry_count;
};

struct MerryFileMetadata {
  MerryFileType type;
  mbool_t st_available;
  msize_t ITIT_len;
  msize_t total_instructions_len;
  msize_t data_section_len;
  msize_t string_section_len;
  msize_t DI_len; // just for checksum
};

// Here: A section is equivalent to a RAM Page.
struct MerrySection {
  msize_t offset;
  msize_t section_length;
};

struct MerryPortion {
  msize_t off_st, off_ed;
};

struct MerryGravesInput {
  FILE *fd;
  mstr_t file_path;
  MerryFileMetadata metadata;
  MerryITIT itit;
  MerrySection *instruction_offsets[__CORE_TYPE_COUNT];
  msize_t instruction_offsets_count[__CORE_TYPE_COUNT];
  MerrySection *data_offsets;
  msize_t data_offsets_count;
  MerrySection *string_offsets;
  msize_t string_offsets_count;
  MerryPortion qword, dword, word;
  MerryRAM *data_ram;
  MerryRAM *iram[__CORE_TYPE_COUNT];
  msize_t file_size;
  mbool_t _instruction_for_core_already_read[__CORE_TYPE_COUNT];
};

mbool_t merry_graves_reader_confirm_input_file(MerryGravesInput *reader,
                                               MerryErrorStack *st);

MerryGravesInput *merry_graves_initialize_reader(mstr_t inp_path,
                                                 MerryErrorStack *st);

mret_t merry_graves_reader_read_input(MerryGravesInput *reader,
                                      MerryErrorStack *st);

void merry_graves_reader_destroy(MerryGravesInput *reader, MerryErrorStack *st);

mret_t merry_graves_reader_parse_identification_header(MerryGravesInput *reader,
                                                       MerryErrorStack *st);

mret_t merry_graves_reader_parse_ITIT_header(MerryGravesInput *reader,
                                             MerryErrorStack *st);

mret_t
merry_graves_reader_parse_data_and_string_header(MerryGravesInput *reader);

mret_t merry_graves_reader_perform_checksum(MerryGravesInput *reader,
                                            MerryErrorStack *st);

mret_t merry_graves_reader_parse_ITIT(MerryGravesInput *reader,
                                      MerryErrorStack *st);

mret_t merry_graves_reader_parse_instruction_sections(MerryGravesInput *reader,
                                                      MerryErrorStack *st);

mret_t merry_graves_reader_parse_data_type_metadata(MerryGravesInput *reader,
                                                    MerryErrorStack *st);

mret_t merry_graves_reader_parse_data_section(MerryGravesInput *reader,
                                              MerryErrorStack *st);

mret_t merry_graves_reader_parse_string_section(MerryGravesInput *reader,
                                                MerryErrorStack *st);

mret_t merry_graves_reader_prep_memory(MerryGravesInput *reader,
                                       MerryErrorStack *st);

mret_t merry_graves_reader_load_instructions(MerryGravesInput *reader,
                                             mcore_t c_type, msize_t pgnum,
                                             MerryErrorStack *st);

mret_t merry_graves_reader_read_qword(MerryGravesInput *reader, msize_t pg_num,
                                      MerrySection *s, msize_t *tr);

mret_t merry_graves_reader_read_dword(MerryGravesInput *reader, msize_t pg_num,
                                      MerrySection *s, msize_t *tr);

mret_t merry_graves_reader_read_word(MerryGravesInput *reader, msize_t pg_num,
                                     MerrySection *s, msize_t *tr);

mret_t merry_graves_reader_load_data(MerryGravesInput *reader, msize_t pgnum,
                                     MerryErrorStack *st);

MerryRAM *merry_graves_reader_get_data_RAM(MerryGravesInput *reader,
                                           MerryErrorStack *st);

#endif
