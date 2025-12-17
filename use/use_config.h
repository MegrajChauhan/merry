#ifndef _USE_CONFIG_
#define _USE_CONFIG_

#if __SIZEOF_VOID__ == 4
#error Merry Only supports 64-bit systems
#endif

#define _MLITTLE_ENDIAN_ 0x00
#define _MBIG_ENDIAN_ 0x01

#define _MERRY_VERSION_CHANGE_ 1
#define _MERRY_VERSION_MINOR_ 1
#define _MERRY_VERSION_MAJOR_ 0

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define _MBYTE_ORDER_ _MLITTLE_ENDIAN_
#else
#define _MBYTE_ORDER_ _MBIG_ENDIAN_
#endif

// Merry's endianness
#define _MENDIANNESS_ _MBYTE_ORDER_

// Host CPU detection
#if defined(__amd64) || defined(__amd64__)
#define _HOST_CPU_AMD_ 1
#define _HOST_ID_ARCH_ 0x00
#endif

// Architecture detection
#if defined(__x86_64)
#define _HOST_CPU_x86_64_ARCH_ 1
#endif

// Host OS detection
#if defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
#define _HOST_OS_LINUX_ 1
#define _USE_LINUX_
#define _HOST_ID_OS_ 0x00
#ifndef __USE_MISC // for STDLIB
#define __USE_MISC
#endif
#endif

#if defined(_WIN64)
#define _HOST_OS_WINDOWS_ 1
#define _USE_WIN_
#define _HOST_ID_OS_ 0x01
#endif

#if __SIZEOF_LONG__ == __SIZEOF_LONG_LONG__
#define _LONG_ long
#else
#define _LONG_ long long
#endif

#endif
