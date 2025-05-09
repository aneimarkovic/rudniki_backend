# Navodila uporabe: backend rudniki

Ta dokument vsebuje navodila za prevajanje in zagon projekta "rudniki-backend" s pomočjo CMake.

## Predpogoji

Preden nadaljujete, se prepričajte, da imate na svojem sistemu nameščeno naslednje:

1.  **C++ Prevajalnik**: Prevajalnik, ki podpira C++20 (npr. GCC, Clang, MSVC).
2.  **CMake**: Minimalna priporočena različica je 3.16.

Knjižnica Boost (Beast, Asio, System) in MongodbConnector bost samodejno prenešeni in prevedeni s strani CMake, če ne bosta najdeni na vašem sistemu. Za to je potrebna internetna povezava ob prvi konfiguraciji.

## Postopek prevajanja in zagona s CMake

1.  **Odprite terminal ali ukazno vrstico.**

2.  **Pomaknite se v korensko mapo projekta** (mapa, ki vsebuje datoteko `CMakeLists.txt`):
    ```bash
    cd pot/do/projekta/rudniki-backend
    ```

3.  **Konfigurirajte projekt s CMake.** Ta korak bo ustvaril gradbene datoteke v podmapi `build` in po potrebi prenesel ter konfiguriral knjižnico Boost. Prva izvedba je lahko dolgotrajna zaradi prenosa Boosta.
    ```bash
    cmake -S . -B build
    ```

4.  **Prevedite projekt.** Ta ukaz bo zagnal dejanski proces prevajanja.
    *   Za Debug konfiguracijo (priporočljivo za razvoj):
        ```bash
        cmake --build build --config Debug
        ```
    *   Za Release konfiguracijo (priporočljivo za končno različico):
        ```bash
        cmake --build build --config Release
        ```

5.  **Zagon programa.**
    Izvedljiva datoteka `rudnik_app` (ali `rudnik_app.exe` na Windows) bo ustvarjena v ustrezni podmapi znotraj mape `build` (npr. `build/Debug` ali `build/Release`).

    *   Na Linux/macOS:
        ```bash
        ./build/Debug/rudnik_app  
        ```
    *   Na Windows:
        ```bash
        .\build\Debug\rudnik_app.exe  
        ```
