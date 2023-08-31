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
| 2.0K       | :heavy_check_mark: / :heavy_check_mark: | :x: / :heavy_check_mark: | :heavy_check_mark: / :heavy_check_mark: |
| 2.0L       | :heavy_check_mark: / :heavy_check_mark: | :x: / :heavy_check_mark: | :heavy_check_mark: / :heavy_check_mark: |
| ique_v1.5  | :x: | :x: | :x: |

## Preparation

After cloning the repo, put a copy of the target archive(s) in their correct version folder in `base/`.
For example, if your target archive is libgultra_rom.a 2.0L then you'd place it in `base/L/`.

## Build dependencies

The build process requires the following packages:

- build-essential
- python3
- binutils-mips-linux-gnu (libultra* only)

Under Debian / Ubunutu you can install them with the following commands:

```bash
sudo apt update
sudo apt install build-essential python3
```

If building any libultra you can install binutils-mips-linux-gnu with:

```bash
sudo apt install binutils-mips-linux-gnu
```

## Building

Run make setup with the proper flags set followed by make with optional jobs.
For example, if building the 2.0L PC archive you'd do the following: 

- `make VERSION=L TARGET=libgultra_rom setup`
- `make VERSION=L TARGET=libgultra_rom`

Every target flag combination requires separate a setup command.
