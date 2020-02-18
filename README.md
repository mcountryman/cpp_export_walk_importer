# cpp_export_walk_importer

Has been done a million times but I wanted to play around with modern c++ again so here it is. :smile:

For those not familiar what this does is:
* Retreives the process environment block from windows' segment registers
* Walks the default loaded dlls until we find a matched import (https://www.aldeid.com/wiki/PEB_LDR_DATA)
* Walks each matched dll's export directory (https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#the-edata-section-image-only)

What this method of importing is capable of:
* Same as `GetProcAddress(LoadLibraryA(libraryName), procedureName)`
* Provided you don't use C++11 you can dynamically retrieve WinApi procedures in shell code without having to supply pointers to `GetProcAddress`, `LoadLibraryA`, and whatever other routines you plan on using as a remote thread parameter (provided this is how/what your using it for)
