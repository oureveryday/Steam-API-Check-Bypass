# Steam API Check Bypass

Bypasses Steam API dll integrity/size check by hooking CreateFile API.

## Build

* Build with Visual Studio 2022.

## Usage

* If you are using x32 version please use `_x32` version dll.

### Method 1

* Use CFF Explorer and add any dll import to game main exe, then put the `SteamAPICheckBypass(_x32).dll` dll beside game exe.

### Method 2 (VersionShim) (x64 for pre-built version.dll)

* Rename `SteamAPICheckBypass(_x32).dll` to `version.dll` and put it beside game exe.

## Configuration (Optional)

* Create `SteamAPICheckBypass.json` and write file names you want to replace. Example:

```json
{
    "steam_api64.dll":
    {
      "mode": "file_redirect",
      "to": "steam_api64.dll.bak",
      "hook_times_mode": "nth_time_only",
      "hook_time_n": [1,2,3]
    },
    "game.exe":
    {
        "mode": "file_redirect",
        "to": "game.exe.bak"
    }
}
  
```

* `mode`: `file_redirect` or `file_hide`.
* `to` : The target file relative path.
* `hook_times_mode`: `all`, `nth_time_only` or `not_nth_time_only`.
* `hook_time_n`: The list of nth time to hook / not hook. (Start from 1, LoadLibrary triggered hook is not counted)
* The `hook time` option is useful for custom requirements.

## Internal Process

* Please refer to [nt_file_dupe Readme](nt_file_dupe/README.md) for more information.

## Dependencies

* <https://github.com/Xpl0itR/VersionShim>
* <https://github.com/otavepto/nt-fs-dupe>

## Bugs

If You Have Any Bugs, Please Submit a Issue On Github.  
cs.rin.ru thread: <https://cs.rin.ru/forum/viewtopic.php?f=10&t=136448>

## Donate

Bitcoin: bc1qk39k55wxwx8yj4w35qu4vh3x4nhn90kq2mempn  
Bitcoin Cash: qzlc8qv59y5lssm9rct2rq5puznqpv9s4qhvhw3k7j  
Ethereum: 0xCBEF4582Fd0d049F3eBB7552027848f54C99cb38  
Stellar: GCMQOHLH6I6QZKCFXWS6VKTDHABFK5KA3CQIZW5JL6GBQYSDCFF5VL2E  
