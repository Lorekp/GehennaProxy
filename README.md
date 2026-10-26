# GehennaProxy

GehennaProxy is a C++17 networking prototype for accepting authenticated game clients and game servers over TCP. It combines SFML socket handling with SQLite key lookups and a simple client-to-client message broadcast loop.

The implemented scope is an authentication and messaging experiment. Game-server connections can authenticate, but forwarding game traffic is unfinished. This is not a production proxy or a complete game backend.

## Implemented behavior

- Accepts TCP connections through an SFML socket selector.
- Reads an authentication key from the first `sf::Packet` and checks it with parameterized SQLite queries.
- Distinguishes client and game-server keys using permission levels `1` and `2`, respectively.
- Requires `USED = 0` and rejects keys already connected in the corresponding in-memory connection map.
- Replies with `authenticated` or `denied`.
- Broadcasts a client's subsequent string messages to the other connected clients.
- Removes disconnected sockets from the selector and connection maps.
- Provides console commands: `1` lists connected clients, and `test` broadcasts a test message to clients.

The connection limit is checked against the client map; it is not a combined client-and-server capacity limit. Successful authentication does not update the database's `USED` field.

## Source layout

| Path | Purpose |
| --- | --- |
| `src/Main.cpp` | Creates the data handler, packet-handler placeholder, and server. |
| `src/Server.cpp`, `src/Server.h` | Authentication, socket selection, connection maps, broadcasting, and console thread. |
| `src/backend/DataHandler.cpp`, `src/backend/DataHandler.h` | Creates the working directory's `data` folder and initializes SQLite. |
| `src/backend/PacketHandler.*` | Empty placeholder for future packet handling. |
| `CMakeLists.txt` | Builds `GehennaProxy`, fetches SFML, and links SQLite. |

## Build prerequisites

- CMake 3.28 or newer.
- A C++17 compiler and a compatible CMake build tool.
- Git and network access for CMake's SFML fetch.
- SQLite headers, a compatible link library, and, for the supplied Windows build configuration, `sqlite3.dll`.

The current build setup targets a local Windows environment. Before configuring, edit `SQLITE3_DIR` in `CMakeLists.txt` to your SQLite installation. The file expects `include/sqlite3.h`, a library under `lib`, and `sqlite3.dll` at the installation root. Its ordinary `set()` assignment means a command-line cache override alone does not replace the configured path.

SFML is fetched from the moving `2.6.x` branch rather than an immutable release. The Windows build also copies SFML's OpenAL DLL. Use a fresh build directory; the committed `cmake-build-debug` directory contains machine-specific generated files.

```sh
cmake -S . -B build
cmake --build build --config Debug
```

## Database setup and running

Runtime data is resolved relative to the process's **working directory**, not the executable location. Use an isolated working directory and disposable test credentials.

There is a schema mismatch to resolve before authentication can work on a new database: `DataHandler` creates `KEYS` with only `USED` and `HASH`, while `Server::validateKey` also queries `PERMISSIONLEVEL`. A usable test database therefore needs all three columns and locally provisioned test rows. The repository supplies an empty, compatible schema in `data/key-schema.sql`; authentication databases and credentials are not tracked. From the chosen working directory, create `data/` and load that schema into `data/key.db` with SQLite before starting the server. Provision fresh test rows locally; there is no key-creation command, migration, or bundled client. Do not reuse keys from older copies of this prototype.

For a single-configuration Windows build, after preparing the SQLite configuration and test database, run the executable from the chosen working directory:

```powershell
& <checkout>\build\bin\GehennaProxy.exe
```

Replace `<checkout>` with your checkout path. Multi-configuration generators may place the executable in `build/bin/Debug` instead. Clients must use SFML packet serialization: an initial string key followed by string message packets. A raw text TCP client is not a complete protocol example.

## Current limitations

Authentication receives synchronously and has no timeout. The implementation has no TLS, logs authentication keys, and shares connection maps between the network and console threads without synchronization. There is no graceful shutdown path. Game-server packets are received but not routed, and the packet-handler class has no behavior.

No automated test suite or CI workflow is provided. Generated build artifacts and an empty historical test log do not establish a successful or reproducible build.

## Dependencies and license

The project uses SFML for networking and SQLite for storage; their implementations are third-party dependencies. Retain their applicable notices when distributing them. No project-level license is included, so reuse and redistribution permissions for this repository's own code are unspecified.
