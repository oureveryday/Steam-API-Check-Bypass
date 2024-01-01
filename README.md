# Steam API Check Bypass

Bypasses Steam API dll integrity/size check by hooking CreateFile API.

## Build

* Build with Visual Studio 2022.

## Usage

### Method 1

* Use CFF Explorer and add `SteamAPICheckBypass` dll import to game main exe, then put the `SteamAPICheckBypass(x32).dll` dll beside game exe.

### Method 2 (VersionShim) (x64 for pre-built version.dll)

* Put `SteamAPICheckBypass.dll` `version.dll` `libraries.txt` beside game exe.

## Configuration (Optional)

* Create `SteamAPICheckBypass.ini` and write file names you want to replace. Example:

```text
[Replace]
OriginalFileFile=ReplaceFileName
steam_api64.dll=steam_api64.dll.bak
steam_api.dll=steam_api.dll.bak

[AfterFirstTime]
OriginalFileFile=0
steam_api.dll=0
```

* `Replace`: The file name to replace.
* `AfterFirstTime`: Start to replace after the first read of the file.

## Internal Process

1. The Crack will check `useinternallist` in the source code, if it's true, it will use `internalreplaceList`.

2. The Crack will try to parse the ini file and add them into the replace list.

3. If ini file not exist, try find `steam_api(64).dll.bak`, `steam_api(64).org`,`steam_api(64)_o.dll` and add the existed file into replace list.

## Dependencies

* <https://github.com/Xpl0itR/VersionShim>

## Bugs

If You Have Any Bugs, Please Submit a Issue On Github.  
cs.rin.ru thread: <https://cs.rin.ru/forum/viewtopic.php?f=10&t=136448>

## Donate

Bitcoin: bc1qk39k55wxwx8yj4w35qu4vh3x4nhn90kq2mempn  
Bitcoin Cash: qzlc8qv59y5lssm9rct2rq5puznqpv9s4qhvhw3k7j  
Ethereum: 0xCBEF4582Fd0d049F3eBB7552027848f54C99cb38  
Stellar: GCMQOHLH6I6QZKCFXWS6VKTDHABFK5KA3CQIZW5JL6GBQYSDCFF5VL2E  
