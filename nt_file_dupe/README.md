# Nt Filesystem Dupe  
A library for file & module redirection and hiding, by hooking various Nt APIs.  
This project is inspired by `CODEX` Steam emu and based on reversing it. Credits to them.

---

## Solution structure
The solution is divided into 3 projects:
1. `nt_file_dupe`: The actual library, this is a static library (`.lib`)
2. `dll_interface`: A thin wrapper `.dll` project around the static library, exporting the necessary functions
3. `testxxx`: A simple console app to test the library + helpers

## JSON file format:
```json
{
  "myfile.txt": {
    "mode": "file_redirect",
    "to": "myfile.org",
    "file_must_exist": true
  },
  "path/myfile_22.txt": {
    "mode": "file_redirect",
    "to": "path/myfile_22.org",
  },
  "../../folder/some_file.txt": {
    "mode": "file_redirect",
    "to": "../../folder/some_file.org",
  },

  "hideme.txt": {
    "mode": "file_hide"
  },
  "../hideme_22.txt": {
    "mode": "file_hide"
  },

  "prevent_me.dll": {
    "mode": "module_prevent_load"
  },
  "prevent_me": {
    "mode": "module_prevent_load"
  },

  "my_module_org.dll": {
    "mode": "module_redirect",
    "to": "my_module_mod.dll"
  },
  "my_module_org": {
    "mode": "module_redirect",
    "to": "my_module_mod"
  }
}
```
Each JSON key is considered the *original* file, the value/object for that key defines the action for the original file.  
The entry type is determined by the `mode` JSON key.
* `mode`  
  
  ---

  - `file_redirect`  
    Redirect original file creation/opening `to` a target file.  
    Target files are always hidden.
  - `file_hide`  
    Hide the file, as if it doesn't exist on disk.

  ---

  - `module_prevent_load`  
    Prevent loading the module via `LoadLibrary()` and its variants.
  - `module_redirect`  
    Redirect the loading operation `to` another target module.  
    Target modules are always hidden and cannot be loaded by the process.
  - `module_hide_handle`  
    Prevent `GetModuleHandle()` from succeeding, this won't affect `LoadLibrary()` and its variants.  
    As if the module doesn't exist in the current process memory.

* `to`  
  Defines which target file/module to redirect the original file/module to.  
  Only useful when `mode` is:
  - `file_redirect`
  - `module_redirect`

* `file_must_exist` (default = `false`)  
  When set to `true`, the JSON entry will be skipped if the original file doesn't exist, or the target file doesn't exist in `file_redirect` mode

Check the example [sample.json](./example/sample.json)

## Behavior
In case this is a **file** entry, the paths to the original or target files could be absolute or relative.  
Relative paths will be relative to the **location of the current `.exe`**, not the current directory.

In case this is a **module** entry, the paths to the original or target modules must be just their filenames.  

Both the original and target files/modules must have the same filename length.  
Additionally, if this is a **file** entry, they must exist in the same dir.  

Target files/modules are hidden by default, for example, in the JSON file defined above:
* `myfile.org` Will be hidden and cannot be opened/created
* `my_module_mod.dll` Will be hidden and cannot be loaded

If `file_must_exist` = `true`, then this JSON entry will be ignored without an error if:
* The original file was missing
* The `mode` = `file_redirect` and the target file, defined by the JSON key `to`, is missing

The dll will try to load only one of the following files upon startup in that order:
* A JSON file with the same name as the `.dll` itself
* `nt_file_dupe.json`
* `nt_file_dupe_config.json`
* `nt_fs_dupe.json`
* `nt_fs_dupe_config.json`
* `nt_dupe.json`
* `nt_dupe_config.json`  

Any of these files must exist **beside** the `.dll`, not the current running `.exe`

Upon startup, the dll will try to hide itself, both on disk and in memory.  
Additionally in the debug build, it will try to hide the log file on disk.  

## How use the pre-built .dll:
1. Create a `.json` file with some entries as shown above
2. Rename the `.json` file to the same name of the `.dll`, for example if the `.dll` is called `nt_file_dupe.dll`, then the `.json` file must be named `nt_file_dupe.json`
3. Place both the `.dll` and the `.json` files beside each other in the same folder
4. Load the `.dll` inside your target, either modify the imports table with something like `CFF Explorer`, or use any dll loader/injector  


Note that the functions to add entries are not thread safe.  


## How to link and use as a static lib (.lib):
1. Open the Visual Studio solution file `nt_file_dupe.sln` and build the project `nt_file_dupe`.  
   Make sure to select the right architecture (`x64` or `x86`) and build type (`release` or `debug`)
2. Assuming for example you've selected `Debug | x64`, the library will be built in  
   ```batch
   bin\x64\nt_file_dupe\nt_file_dupe_static.lib
   ```
3. In your own Visual Studio project, you must use C++ language version >= `C++17`,  
   add the static `.lib` file as an input to the linker: [.lib files as linker input](https://learn.microsoft.com/en-us/cpp/build/reference/dot-lib-files-as-linker-input#to-add-lib-files-as-linker-input-in-the-development-environment)
4. Finally, add the folder `nt_file_dupe\include` as an extra include directory: [Additional include directories](https://learn.microsoft.com/en-us/cpp/build/reference/i-additional-include-directories#to-set-this-compiler-option-in-the-visual-studio-development-environment)
5. Everything will be under the namespace `ntfsdupe::`
6. Check how the `.dll` wrapper project is importing the required `.hpp` files, and using the library in its [DllMain](./dll_interface/dllmain.cpp)

## How to link and use as a dynamic lib (.dll):
1. Open the Visual Studio solution file `nt_file_dupe.sln` and build the project `dll_interface`.  
   Make sure to select the right architecture (`x64` or `x86`) and build type (`release` or `debug`)
2. Assuming for example you've selected `Debug | x64`, the library will be built as 2 parts in  
   ```batch
   bin\Debug\x64\dll_interface\nt_file_dupe.dll
   bin\Debug\x64\dll_interface\nt_file_dupe.lib
   ```
   Notice that the `.dll` file must be accompanied by a small-sized `.lib` file which we'll use next
3. In your own Visual Studio project, you must use C++ language version >= `C++17`,  
   add the static `.lib` file as an input to the linker: [.lib files as linker input](https://learn.microsoft.com/en-us/cpp/build/reference/dot-lib-files-as-linker-input#to-add-lib-files-as-linker-input-in-the-development-environment)
4. Finally, add the folder `dll_interface\include` as an extra include directory: [Additional include directories](https://learn.microsoft.com/en-us/cpp/build/reference/i-additional-include-directories#to-set-this-compiler-option-in-the-visual-studio-development-environment)
5. All available exports will have this prefix `ntfsdupe_`
   ```c++
   #include "nt_file_dupe.hpp"

   int main() {
       ntfsdupe_load_file(L"myfile.json");
       ntfsdupe_add_entry(ntfsdupe::itf::Mode::file_hide, L"some_file.dll", nullptr);
       return 0;
   }
   ```
6. The file `nt_file_dupe.dll` will be added to your imports table, so make sure to copy this `.dll` file beside your project's build output
