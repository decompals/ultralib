# ultralib

Reverse engineering of libultra

## Compatibility

Currently this repo supports building the following versions:

- [ ] libgultra 2.0E
- [ ] libgultra 2.0F
- [ ] libgultra 2.0G
- [ ] libgultra 2.0H
- [ ] libgultra 2.0I_patchNOA9801
- [ ] libgultra 2.0J
- [ ] libgultra 2.0J
- [ ] libgultra 2.0K
- [X] libgultra 2.0L
- [ ] ique_v1.5

- libgultra

|      | `libgultra.a` | `libgultra_d.a` | `libgultra_rom.a` |
|------| -             | -               | -                 |
| 2.0H | `[ ]`           | [ ]             | [ ]               |
| 2.0I | [ ]           | [ ]             | [ ]               |
| 2.0J | [ ]           | [ ]             | [ ]               |
| 2.0K | :cross_mark:           | :x:             | [ ]               |
| 2.0L | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |

## Preparation

After clonning the repo, put a copy of `libgultra_rom.a` on the root of this directory.

## Building

- `make setup`
- `make`
