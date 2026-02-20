# HackerOne Scope Fetcher

A C program that scans all HackerOne programs via the API, fetches **structured scopes** (asset_identifier) for each program, and saves bounty/submission-eligible scopes to `scopes.txt`.

## Features

- ✅ HackerOne API v1 integration
- ✅ Automatic pagination for program list
- ✅ Structured scopes fetch for each program
- ✅ Filters only scopes where `eligible_for_bounty` and `eligible_for_submission` are true
- ✅ Output: `scopes.txt` (one asset_identifier per line)
- ✅ Optional wildcard extraction: `wildcards.txt` (scopes starting with `*.`)
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

When the program runs, it prompts you for your username and API key. The HackerOne API token format is typically `username:token`.

### 2. Running the Program

```bash
./hackerone_fetcher
```

When the program runs:
- First enter your **Username**
- Then enter your **API Key**
- Progress messages are displayed for each program page
- Scopes are fetched for each program
- Eligible scopes are written to `scopes.txt`
- A success message is shown when the process completes
- Optionally, you can extract wildcard scopes (Y/N) to create `wildcards.txt`

### 3. Output Files

- **scopes.txt**: All eligible `asset_identifier` values (one per line, e.g. `*.example.com`, `api.example.com`)
- **wildcards.txt** (optional): If you choose Y when prompted, only scopes starting with `*.` are extracted to this file

## Example Output

```
Enter Username : your_username
Enter Api Key : your_api_token
Fetching programs page 1...
Scopes fetched: program1
Scopes fetched: program2
...
All scopes saved to scopes.txt ✅

Do you Want Wildcard Scopes (Y/N) : Y
Finded Wildcard: *.example.com
Finded Wildcard: *.api.example.com
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
