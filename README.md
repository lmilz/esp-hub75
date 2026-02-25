# esp-hub75

A header-only C++20 library for driving matrix panels, with an ESP-IDF backend for hardware and an SDL2 backend for desktop simulation.

![hub75-matrix](/matrix_effect.jpg)

## Features

- Header-only, no build step required
- ESP32 (ESP-IDF) and SDL2 desktop simulation backends

## Requirements

| Backend | Requirement |
|---------|-------------|
| ESP32   | ESP-IDF 5.x, C++20 |
| Desktop | SDL2, CMake, C++20 |

## Build 

Running the tests
```bash
./build.sh
```

Build the examples
```bash
./run_examples.sh
```

## License

MIT — see [LICENSE](LICENSE).
