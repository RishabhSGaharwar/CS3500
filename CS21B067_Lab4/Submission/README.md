**README : CS21B067**

*Known Errors :*
Some edge cases MIGHT'VE been missed in string parsing (igniring whitespaces, though, through robust testing, it still passed all custome testcases)

**All of the code in my submission has been written entirely by myself, without referring to any source (except documentation for sstream and getline)**
I referred to documentation for chrono, ctime and stringstream

**ASSUMPTIONS :**
1. None of the file names have any extension : I am adding .txt suffix to all file names
2. ADD & SUB commands are of the form add/sub x, y, z (there are commas present)
3. LOAD commands are of the form load a, x (there is a comma present)
4. All commands are in correct form (no command error handling, only whitespaces are being handled)
5. All files are .txt files
6. There is a buffer space between Virtual Memory and Main Memory
7. If Swapout can not be implemented (because VM is full) then the process is killed
8. Swapout is called within Swapin. If no process has been run before Swapout, LRU is empty and nothing happens.
9. If memory is full and process can not be loaded into memory, it is discarded
10. Exit is printed in Stdout and I exit