# Structures

1. **File**:
    Represents a file interface

# Functions

1. **interfaceRet_t open_file(File \*\*file, str_t file_path, str_t modes, int flags)**:
- Description: Opens a file from path **file_path** for operations represented by mode **modes** and **flags**.
- Arguments: **file** is a pointer to a **File *** while **file_path** is a null-terminated string. **modes** can have the following values:
   | Name | Description |
   | ---- | ----------- |
   | O_MODE_READ | Open the file in read-only mode |
   | O_MODE_WRITE | Open the file in write-only mode |
   | O_MODE_READ_WRITE | Open the file in read and write mode |
   | O_MODE_APPEND | Open the file in append mode |

   **flags** can have one of the following values:
  | Name | Description |
  | ---- | ----------- |
  | O_FLAG_CREATE | Create a new file at **file_path** if it doesn't exist |
  | O_FLAG_READ_PERM_FOR_USR, O_FLAG_WRITE_PERM_FOR_USR, O_FLAG_EXEC_PERM_FOR_USR | Give RWX permissions for users to the newly created file if **O_FLAG_CREATE** is passed. | 
  | O_FLAG_READ_PERM_FOR_GRP, O_FLAG_WRITE_PERM_FOR_GRP, O_FLAG_EXEC_PERM_FOR_GRP | Give RWX permissions for groups to the newly created file if **O_FLAG_CREATE** is passed. | 
  | O_FLAG_READ_PERM_FOR_OTH, O_FLAG_WRITE_PERM_FOR_OTH, O_FLAG_EXEC_PERM_FOR_OTH | Give RWX permissions for others to the newly created file if **O_FLAG_CREATE** is passed. | 

  The given flags can be OR'ed together to get the desired behavior.
- Return: If the operation succeeds then **INTERFACE_SUCCESS** is returned else one of the following values are returned:
        1. INTERFACE_HOST_FAILURE
        2. INTERFACE_FAILURE
        3. INTERFACE_INVALID_ARGS

2. **interfaceRet_t close_file(File \*file);**
- Description: Closes **file** once it has served its purpose.
- Return: If the operation succeeds then **INTERFACE_SUCCESS** is returned else one of the following values is returned:
          1. INTERFACE_TYPE_INVALID: **file** is not a valid **File** pointer.
          2. INTERFACE_MISCONFIGURED: **file** was never opened to begin with.

3. **interfaceRet_t destroy_file(File \*file);**
- Description: After closing a file, the pointer is still valid and hence the resources occupied by **file** are freed.
