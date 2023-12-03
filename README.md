# Steam API Check Bypass

Bypasses Steam API dll integrity/size check by hooking CreateFile API.

## Build

* Build with Visual Studio 2022.

## Usage

* Rename original `steam_api(64).dll` to `steam_api(64).org` or `steam_api(64).api.bak` or `steam_api(64)_o.dll` first.

### Method 1

* Use CFF Explorer and add `SteamAPICheckBypass` dll import to game main exe, then put the `SteamAPICheckBypass(x32).dll` dll beside game exe.

### Method 2 (VersionShim) (x64 for pre-built version.dll)

* Put `SteamAPICheckBypass.dll` `version.dll` `libraries.txt` beside game exe.

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
