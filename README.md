# OrionServer

**Decentralized Minecraft launcher backend** — lightweight C++20 HTTP server that runs alongside your Minecraft server. Each server runs its own instance. Clients connect directly to the server's IP address. No central infrastructure required.

## Why Decentralized?

- **No hosting costs** — You don't pay for a central server
- **No single point of failure** — One server down doesn't affect others
- **Privacy** — No central entity sees all player activity
- **Simple deployment** — Just run the binary next to your Minecraft server

<!--
## Architecture

```
┌─────────────────┐         ┌─────────────────────────┐
│  Minecraft      │         │  OrionServer              │
│  Server A       │◄───────►│  (port 8080)            │
│  (with mods)    │         │  - serves /manifest     │
└─────────────────┘         │  - filters files        │
▲                           │  - rate limiting        │
│                           └───────────┬─────────────┘
│ Direct HTTP                           │
│ requests                              │
│                                       │
│                           ┌───────────▼─────────────┐
│                           │  Client Launcher        │
│                           │  - gets /manifest       │
│                           │  - compares versions    │
└──────────────────────────►│  - downloads only diffs │
                            └─────────────────────────┘
```
-->
## How It Works

1. **Admin** places `OrionServer_backend` executable next to Minecraft server
2. **Admin** configures `rules.json` (what files to send to clients)
3. **Admin** runs the backend (e.g., on port 8080)
4. **Client** launches their local launcher, enters Server IP:Port
5. **Client** requests `GET /manifest` → gets file list with hashes
6. **Client** compares with local files
7. **Client** requests only missing/changed files via `GET /file/{name}`
8. **Backend** applies filters (allowlist/blocklist) before responding

## API Endpoints (Deprecated)

| Endpoint | Method | Response | Description |
|----------|--------|----------|-------------|
| `/manifest` | GET | JSON | List of files with names, sizes, hashes |
| `/file/{name}` | GET | Binary | Download specific file |
| `/filelist` | POST | ZIP or JSON | Bulk download multiple files |
| `/health` | GET | Plain text | Health check for monitoring |

### Example `/manifest` response

```json
{
  "files": [
    {"name": "mods/example.jar", "size": 1024000, "hash": "a1b2c3d4"},
    {"name": "config/options.txt", "size": 512, "hash": "e5f6g7h8"}
  ],
  "generated_at": "2026-05-02T10:00:00Z"
}
```

## Filtering System (rules.json)

Backend filters files **before** adding them to manifest:

```json
{
  "mode": "allowlist",
  "client_server": ["mods/shared.jar", "config/common.json"],
  "client_only": ["mods/minimap.jar", "mods/jei.jar", "mods/client_sponge.jar"],
  "server_only": ["mods/sponge.jar", "mods/server_only_mod.jar"],
  "blacklist": ["mods/broken.jar"]
}
```

| Field | Purpose |
|-------|---------|
| `mode` | `allowlist` (only send listed) or `blocklist` (send all except listed) |
| `client_server` | Files both sides need → send |
| `client_only` | Client-only mods → send |
| `server_only` | Server-only files → DON'T send |


## Security & DoS Protection

Since the backend is exposed directly to the internet (no central proxy):

| Feature | Description |
|---------|-------------|
| **Rate limiting** | Per-IP request limits (configurable, e.g., 60 req/min) |
| **Burst protection** | Allow small bursts before strict limiting kicks in |
| **Max file size** | Prevent oversized file requests |
| **Connection timeouts** | Abort slow connections after timeout |
| **Request logging** | All requests logged with IP, timestamp, endpoint |
| **IP whitelist (optional)** | Restrict to known player IP ranges |
| **Concurrent limits** | Limit simultaneous downloads per IP |

### Configuration Example (config.yml)

```yaml
server:
  port: 8080
  threads: 4
  server_root: ./minecraft_server
  rules_file: ./rules.json

rate_limit:
  requests_per_minute: 60
  burst_limit: 10
  ban_after_violations: 5
  ban_duration_minutes: 15

security:
  max_file_size_mb: 500
  request_timeout_seconds: 30
  max_concurrent_per_ip: 2
  ip_whitelist: []

logging:
  enabled: true
  level: info
  log_file: ./OrionServer.log
  access_log: ./access.log
```

## Tech Stack

| Component | Technology |
|-----------|------------|
| Language | C++20 |
| Networking | Boost.Asio (async HTTP server) |
| JSON | nlohmann/json |
| Hashing | xxHash or MD5 |
| Filesystem | std::filesystem |
| Rate limiting | Custom sliding window |
| Build system | CMake |

## Testing

Project use unit-tests (GoogleTest) and integration tests.
- **Config**: 12 tests (load, valid, filter)
- **Router**: 8 tests (route, methods)
- **Handlers**: 6 tests (manifest, sync)
- **Integration**: 3 tests (HTTP request)

Run: `ctest --output-on-failure`

## Build

```bash
git clone https://github.com/Barkosss/OrionServer
cd OrionServer
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Windows Build

```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

## Usage

### On the Game Server (Linux)

```bash
./OrionServer --config ./config.yml
```

### On the Game Server (Windows)

```cmd
OrionServer.exe --config config.yml
```

### Client-Side Integration Example (Python launcher)

```python
import requests

SERVER_IP = "game-server.example.com"
PORT = 8080

manifest = requests.get(f"http://{SERVER_IP}:{PORT}/manifest").json()

for file_info in manifest["files"]:
    if not local_file_exists(file_info["name"]) or \
       local_hash(file_info["name"]) != file_info["hash"]:
        data = requests.get(f"http://{SERVER_IP}:{PORT}/file/{file_info['name']}")
        save_file(file_info["name"], data.content)
```

## Directory Structure (on game server)

```
/path/to/minecraft_server/
├── OrionServer                  # executable
├── config.yml                   # backend config
├── rules.json                   # file filtering rules
├── mods/                        # server/client mods
├── config/                      # shared configs
└── world/                       # server-only (filtered out)
```

## Use Cases

- **Small Minecraft communities** who can't afford custom launcher development
- **Technical admins** who want full control over updates
- **Multiple servers** — Each server runs its own backend independently

## Roadmap

- [ ] HTTPS support (Let's Encrypt)
- [ ] Delta updates (binary diff instead of full file)
- [ ] Compression on-the-fly (gzip)
- [ ] Web dashboard for monitoring
- [ ] Docker image

## Requirements

- C++20 compatible compiler (GCC 13+, Clang 14+, MSVC 2022+)
- CMake 3.31+
- Boost.Asio (header-only)

## License

MIT