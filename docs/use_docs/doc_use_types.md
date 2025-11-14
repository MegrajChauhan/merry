# Constants

| Name | Description |
| ---- | ----------- |
| TRUE | Boolean true |
| FALSE | Boolean false |
| RET_NULL | NULL |
| RET_SUCCESS | TRUE |
| RET_FAILURE | FALSE |

# Types

| Name | Description |
| ---- | ----------- |
| bool_t | Boolean data type |
| byte_t | 1-byte values |
| word_t | 2-byte values |
| dword_t | 4-byte values |
| qword_t | 8-byte values |
| sbyte_t | signed 1-byte values |
| sword_t | signed 2-byte values |
| sdword_t | signed 4-byte values |
| sqword_t | signed 8-byte values |
| address_t | 8-byte addresses |
| ret_t | Either RET_FAILURE or RET_SUCCESS |
| bptr_t | byte_t pointer |
| wptr_t | word_t pointer |
| dptr_t | dword_t pointer |
| qptr_t | qptr_t pointer |
| str_t | Character array |
| cstr_t | constant character array |
| ptr_t | generic pointer |

# Macros

1. **\_MERRY\_DEFINE\_FUNC\_PTR\_(return_type, ptr_name, ...)**
   Defines a function pointer of name **ptr_name** with return type **return_type**.
   Example:
      ```c
       _MERRY_DEFINE_FUNC_PTR_(int, tmp_t, void);
      // Equivalent to
      // typedef int (*tmp_t)(void);
      ```
