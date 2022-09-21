# ultralib

Reverse engineering of libultra

## Compatibility

Currently this repo supports building the following versions:

| IDO / GCC  | `libultra.a` / `libgultra.a` | `libultra_d.a` / `libgultra_d.a` | `libultra_rom.a` / `libgultra_rom.a` |
| -          | :-: | :-: | :-: |
| 2.0E       | :x: / N/A | :x: / N/A | :x: / N/A |
| 2.0F       | :x: / N/A | :x: / N/A | :x: / N/A |
| 2.0G       | :x: / N/A | :x: / N/A | :x: / N/A |
| 2.0H       | :x: / :x: | :x: / :x: | :x: / :x: |
| 2.0I       | :x: / :x: | :x: / :x: | :x: / :x: |
| 2.0I_patch | :x: / :x: | :x: / :x: | :x: / :x: |
| 2.0J       | :x: / :x: | :x: / :x: | :x: / :x: |
| 2.0K       | :x: / :x: | :x: / :x: | :x: / :x: |
| 2.0L       | :x: / :heavy_check_mark: | :x: / :heavy_check_mark: | :x: / :heavy_check_mark: |
| ique_v1.5  | ? | ? | ? |

## Preparation

After clonning the repo, put a copy of `libgultra_rom.a` on the root of this directory.

## Building

- `make setup`
- `make`
