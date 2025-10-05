#ifndef _MERRY_GRAVES_INPUT_
#define _MERRY_GRAVES_INPUT_

/**
 * Format(v1):
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
 *
 * Format(v2):
 * This new format is specifically designed to improve loading time and offload
 * paging overhead to the operating system. The major pain with this format is
 * that there will be multiple files to keep track of but the plus points are
 * just as impressive: startup time is pretty small, and loading is done
 * efficiently by the operating system on demand, hence, there is little
 * penalty(other than the OS interferring) and only the needed ones are loaded
 * at once instead of every file. The metadata file will have the structure: BIG
 * ENDIAN
 * <MAGIC NUMBERS:3 bytes> <Number of entries: 5 bytes> -> Information
 * required: a single core type can only have one entry each.
 * <CORE_TYPE 1: 8 bytes> [path to the file] -> null-terminated
 * string[same level as metadata file]
 * ......
 * <CORE_TYPE N: 8 bytes>
 * [path to the file] -> null-terminated string[same level as metadata file]
 *
 * That's it! But what about the contents of these files? Well Graves will pass
 * them on to every core. Each core will have its own format to meet its need.
 * */

#include <errno.h>
#include <merry_core_types.h>
#include <merry_graves_defs.h>
#include <merry_list.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mstr_t *merry_graves_parse_metadata_file(mstr_t mfile);

#endif
