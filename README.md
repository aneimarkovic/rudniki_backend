# Navodila uporabe: backend rudniki

Ta dokument vsebuje navodila za prevajanje in zagon projekta "rudniki-backend" s pomočjo CMake.

## Predpogoji

Preden nadaljujete, se prepričajte, da imate na svojem sistemu nameščeno naslednje:

1.  **C++ Prevajalnik**: Prevajalnik, ki podpira C++20 (npr. GCC, Clang, MSVC).
2.  **CMake**: Minimalna priporočena različica je 3.16.
3.  **Knjižnica OpenSSL**: Za podporo SSL/TLS.
4.  **Knjižnica Boost**: Potrebne so komponente `system`, `beast` in `asio`.
5.  **MongoDB C++ gonilnik (mongocxx)**.
6.  **Catch2 (verzija 3.x)**: Testno ogrodje.
7.  **Internetna povezava**: Morda potrebna za prenos nekaterih odvisnosti s strani upraviteljev paketov.

### Namestitev odvisnosti (OpenSSL, Boost, MongoDB C++ gonilnik in Catch2)

**macOS (z uporabo Homebrew):**

Odprite terminal in zaženite naslednje ukaze za namestitev knjižnic OpenSSL, Boost, MongoDB C++ gonilnika in Catch2:

```bash
brew install openssl
brew install boost
brew install mongo-cxx-driver
brew install catch2
```

Če Homebrew ni nameščen, ga lahko namestite s [spletne strani Homebrew](https://brew.sh).
**Opomba za OpenSSL na macOS:** Homebrew lahko namesti OpenSSL v pot, ki ni standardna za CMake. Morda boste morali CMake-u pri konfiguraciji sporočiti, kje najti OpenSSL, npr. z nastavitvijo `CMAKE_PREFIX_PATH`:
`cmake -S . -B build -DCMAKE_PREFIX_PATH=$(brew --prefix openssl)`
ali z izvozom spremenljivk okolja pred zagonom CMake:
`export LDFLAGS="-L$(brew --prefix openssl)/lib"`
`export CPPFLAGS="-I$(brew --prefix openssl)/include"`
Podobno velja, če CMake ne najde drugih knjižnic, nameščenih preko Homebrew.

**Windows (z uporabo vcpkg):**

Priporočamo uporabo `vcpkg` za upravljanje C++ knjižnic na Windowsu.

1.  Če še nimate nameščenega `vcpkg`, sledite [uradnim navodilom za namestitev vcpkg](https://vcpkg.io/en/getting-started.html).
2.  **Integrirajte `vcpkg` z vašim gradbenim okoljem (zelo priporočljivo):**
    Po uspešni namestitvi `vcpkg` zaženite naslednji ukaz v PowerShellu ali ukazni vrstici (iz mape, kjer je `vcpkg` nameščen):
    ```powershell
    .\vcpkg integrate install
    ```
    Ta korak omogoča, da CMake samodejno najde `vcpkg` nameščene knjižnice brez potrebe po ročnem specificiranju `-DCMAKE_TOOLCHAIN_FILE` pri vsaki konfiguraciji.

3.  Namestite potrebne knjižnice z naslednjimi ukazi v PowerShellu ali ukazni vrstici (v mapi, kjer je `vcpkg`):
```powershell
    .\vcpkg install openssl boost-system boost-beast boost-asio mongo-cxx-driver catch2
```
Če uporabljate 64-bitno gradnjo (kar je običajno), boste morda želeli specificirati arhitekturo, npr.:
`.\vcpkg install openssl:x64-windows boost-system:x64-windows boost-beast:x64-windows boost-asio:x64-windows mongo-cxx-driver:x64-windows catch2:x64-windows`

**Linux (primer za Debian/Ubuntu):**

Za distribucije, ki temeljijo na Debianu (kot je Ubuntu), lahko knjižnice namestite z naslednjimi ukazi:

```bash
sudo apt update
sudo apt install libssl-dev libboost-system-dev libboost-beast-dev libboost-asio-dev libmongocxx-dev libbsoncxx-dev catch2
```
Za druge distribucije Linuxa uporabite ustrezen upravitelj paketov (npr. `openssl-devel` ali `libopenssl-devel` za OpenSSL na sistemih, ki temeljijo na RPM, kot sta Fedora ali CentOS, in podobno za druge pakete). Ime paketa za Catch2 se lahko razlikuje (npr. `catch2-devel`).

## Postopek prevajanja in zagona s CMake

1.  **Odprite terminal ali ukazno vrstico.**

2.  **Pomaknite se v korensko mapo projekta** (mapa, ki vsebuje datoteko `CMakeLists.txt`):
    ```bash
    cd pot/do/projekta/rudniki-backend
    ```

3.  **Konfigurirajte projekt s CMake.** Ta korak bo ustvaril gradbene datoteke v podmapi `build`.
    *   **Če ste na Windows uporabili `.\vcpkg integrate install` (ali za macOS/Linux, kjer so knjižnice v standardnih poteh in jih CMake najde brez pomoči):**
        ```bash
        cmake -S . -B build
        ```
    *   **Če na Windows niste uporabili `.\vcpkg integrate install` (ali želite eksplicitno navesti toolchain):**
        (Zamenjajte `[pot/do/vcpkg]` z dejansko potjo do vaše `vcpkg` namestitve)
        ```bash
        cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=[pot/do/vcpkg]/scripts/buildsystems/vcpkg.cmake
        ```
    *   **Če na macOS CMake ne najde knjižnic, nameščenih preko Homebrew v nestandardne poti:**
        Lahko poskusite z:
        ```bash
        cmake -S . -B build -DCMAKE_PREFIX_PATH="$(brew --prefix openssl);$(brew --prefix boost);$(brew --prefix mongo-cxx-driver);$(brew --prefix catch2)"
        ```
        (Prilagodite poti, če je potrebno, in dodajte druge knjižnice, če jih CMake ne najde.)

4.  **Prevedite projekt.** Ta ukaz bo zagnal dejanski proces prevajanja, vključno s testno kodo.
    *   Za Debug konfiguracijo (priporočljivo za razvoj):
        ```bash
        cmake --build build --config Debug
        ```
    *   Za Release konfiguracijo (priporočljivo za končno različico):
        ```bash
        cmake --build build --config Release
        ```

5.  **Zagon glavnega programa.**
    Izvedljiva datoteka `rudnik_app` (ali `rudnik_app.exe` na Windows) bo ustvarjena v ustrezni podmapi znotraj mape `build` (npr. `build/src/Debug` ali `build/src/Release`, odvisno od vaše strukture projektov in `--config` zastavice – natančna pot se lahko razlikuje). Predpostavimo, da je v `build/<config>/`:

    *   Na Linux/macOS (če ste prevajali z Debug konfiguracijo):
        ```bash
        ./build/Debug/rudnik_app
        ```
        (ali `./build/Release/rudnik_app` za Release)
    *   Na Windows (če ste prevajali z Debug konfiguracijo):
        ```powershell
        .\build\Debug\rudnik_app.exe
        ```
        (ali `.\build\Release\rudnik_app.exe` za Release)

## Zagon testov

Po uspešnem prevajanju lahko teste zaženete na enega od naslednjih načinov iz korenske mape projekta:

1.  **Z uporabo CTest (iz mape `build`):**
    ```bash
    cd build
    ctest
    ```
    Ali za bolj podroben izpis (verbose):
    ```bash
    ctest -V
    ```
    Če želite zagnati teste za določeno konfiguracijo:
    ```bash
    ctest -C Debug
    # ali
    ctest -C Release
    ```
    (Vrnite se nazaj v korensko mapo projekta z `cd ..`)

2.  **Z gradnjo tarče `test` (iz korenske mape projekta):**
    Ta ukaz bo prevedel teste, če še niso, in jih nato zagnal. Ime testne tarče se lahko razlikuje glede na to, kako je definirana v `test/CMakeLists.txt`. Če CTest najde teste, je to običajno bolj zanesljivo. Če je tarča imenovana `test` ali podobno:
    ```bash
    cmake --build build --target test --config Debug
    ```    (Zamenjajte `Debug` z `Release` po potrebi, in `test` z dejanskim imenom testne tarče, če je drugačno)

3.  **Neposredno z izvajanjem testne datoteke (iz mape `build`):**
    Testna izvedljiva datoteka (npr. `rudniki_tests` ali `rudniki_tests.exe`) bo v podmapi znotraj mape `build` (npr. `build/test/Debug/` ali `build/test/Release/` - natančna pot je odvisna od konfiguracije v `test/CMakeLists.txt`).
    * Na Linux/macOS:
      ```bash
      ./build/test/Debug/rudniki_tests
      ```
    * Na Windows:
      ```powershell
      .\build\test\Debug\rudniki_tests.exe
      ```