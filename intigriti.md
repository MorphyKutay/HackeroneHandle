# Intigriti Scope Domain Fetcher

This project uses the Intigriti Researcher API to collect scope/domain endpoints from programs you can access and save them to files.

## Features

- Fetches the program list from the API.
- Collects domain/scope endpoints from each program detail.
- Prints results to the terminal.
- Saves results to output files.
- Supports `-w` mode to keep only wildcard (`*`) endpoints.

## Requirements

- `gcc`
- `libcurl`
- `cJSON` (`-lcjson`)

Example (Arch/CachyOS):

```bash
sudo pacman -S gcc curl cjson
```

## Build

```bash
gcc main.c -o scope -lcurl -lcjson
```

## Usage

Normal mode (all endpoints):

```bash
./scope
```

Wildcard-only mode:

```bash
./scope -w
```

When the program starts, it asks for your API key:

```text
Enter API key:
```

## Output Files

- Normal mode: `domains.txt`
- Wildcard mode (`-w`): `wildcard.txt`

## Notes

- Program detail endpoint:
  - `GET /external/researcher/v1/programs/{programId}`
- Fallback endpoint (when needed):
  - `GET /external/researcher/v1/programs/{programId}/domains/{versionId}`
- The program list parser supports multiple response shapes, including `records`.

## Security

- Do not hardcode your API key in source code.
- If you shared your API key, rotate/regenerate it immediately.
