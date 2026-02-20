# HackerOne Scope Fetcher

A C program that scans all HackerOne programs via the API, fetches **structured scopes** (asset_identifier) for each program, and saves bounty/submission-eligible scopes to `scopes.txt`.

## Features

- ✅ HackerOne API v1 integration
- ✅ Automatic pagination for program list
- ✅ Structured scopes fetch for each program
- ✅ Filters only scopes where `eligible_for_bounty` and `eligible_for_submission` are true
- ✅ Output: `scopes.txt` (one asset_identifier per line)
- ✅ Basic Authentication support

## Requirements

- **libcurl**: For HTTP requests
- **cJSON**: For JSON parsing
- **GCC** or a compatible C compiler

### Installation on Windows

```powershell
# Using vcpkg (recommended)
vcpkg install libcurl cjson

# Or using Chocolatey
choco install curl
```

### Installation on Linux

```bash
# Ubuntu/Debian
sudo apt-get install libcurl4-openssl-dev libcjson-dev

# Fedora/RHEL
sudo dnf install libcurl-devel libcjson-devel

# Arch Linux
sudo pacman -S curl cjson
```

### Installation on macOS

```bash
brew install curl cjson
```

## Building

```bash
gcc main.c -o hackerone_fetcher -lcurl -lcjson
```

## Usage

### 1. API Credentials

Update the credentials on lines 107-108 of `main.c`:

```c
curl_easy_setopt(curl, CURLOPT_USERPWD,
    "<username>:<apikey>");
```

**Note:** You can use your HackerOne API token. The format is typically `username:token`.

### 2. Running the Program

```bash
./hackerone_fetcher
```

When the program runs:
- Progress messages are displayed for each program page
- Scopes are fetched for each program
- All eligible scopes are written to `scopes.txt`
- A success message is shown when the process completes

### 3. Output File

The program writes `asset_identifier` values of eligible scopes to `scopes.txt`. One asset per line (e.g. `*.example.com`, `api.example.com`).

## Example Output

```
Fetching programs page 1...
Scopes fetched: program1
Scopes fetched: program2
...
All scopes saved to scopes.txt ✅
```

## API Documentation

This program uses the HackerOne API v1. For more information:
- [HackerOne API Documentation](https://api.hackerone.com/docs)

## Security Notes

⚠️ **IMPORTANT:** 
- Never commit your credentials to Git
- Use environment variables in production environments
- Keep your API tokens secure

## Troubleshooting

### "Curl init failed" error
- libcurl may not be installed correctly
- Make sure you added the `-lcurl` parameter during compilation

### "JSON parse error" error
- Invalid response may be received from the API
- Check your credentials
- You may have hit the API rate limit

### "Request failed" error
- Check your internet connection
- Make sure the API endpoint is accessible
- Verify that your credentials are correct

## License

This project is licensed under the GNU General Public License v3.0 (GPL-3.0).

See the [LICENSE](LICENSE) file for details, or visit:
- [GNU GPL v3.0 Full Text](https://www.gnu.org/licenses/gpl-3.0.html)
