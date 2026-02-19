# HackerOne Program Fetcher

A C program that automatically fetches all hacker programs from the HackerOne API and saves them in JSON format.

## Features

- ✅ Integration with HackerOne API v1
- ✅ Automatic pagination support
- ✅ Download all programs in one go
- ✅ Create output file in JSON format
- ✅ Basic Authentication support

## Requirements

You need the following libraries to compile and run this program:

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

### 1. Setting Up Credentials

Update your username and password in lines 44-45 of the `main.c` file:

```c
curl_easy_setopt(curl, CURLOPT_USERPWD,
    "username:password");
```

**Note:** You can use your HackerOne API token. The token format is usually `username:token`.

### 2. Running the Program

```bash
./hackerone_fetcher
```

When the program runs:
- Progress messages are displayed for each page
- All data is saved to the `output.json` file
- A success message is displayed when the process is complete

### 3. Output File

The program writes all downloaded program data to the `output.json` file. Each page is stored in JSON format on a separate line.

## Example Output

```
Fetching page 1...
Fetching page 2...
Fetching page 3...
All pages downloaded ✅
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
