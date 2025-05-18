# Navodila uporabe: backend rudniki

Ta dokument vsebuje navodila za prevajanje in zagon projekta "rudniki-backend" s pomočjo CMake.

## Predpogoji

Preden nadaljujete, se prepričajte, da imate na svojem sistemu nameščeno naslednje:

1.  **C++ Prevajalnik**: Prevajalnik, ki podpira C++20 (npr. GCC, Clang, MSVC).
2.  **CMake**: Minimalna priporočena različica je 3.16.
3.  **Knjižnica Boost**: Potrebne so komponente `system`, `beast` in `asio`.
4.  **MongoDB C++ gonilnik (mongocxx)**.
5.  **Internetna povezava**: Za prvi prenos testnega ogrodja Catch2 (če še ni predpomnjen s strani CMake).

### Namestitev odvisnosti (Boost in MongoDB C++ gonilnik)

**macOS (z uporabo Homebrew):**

Odprite terminal in zaženite naslednje ukaze za namestitev knjižnic Boost in MongoDB C++ gonilnika:

```bash
brew install boost
brew install mongo-cxx-driver
```

Če Homebrew ni nameščen, ga lahko namestite s [spletne strani Homebrew](https://brew.sh).

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
    .\vcpkg install boost-system boost-beast boost-asio mongo-cxx-driver
```

**Linux (primer za Debian/Ubuntu):**

Za distribucije, ki temeljijo na Debianu (kot je Ubuntu), lahko knjižnice namestite z naslednjimi ukazi:

```bash
sudo apt update
sudo apt install libboost-system-dev libboost-beast-dev libboost-asio-dev libmongocxx-dev libbsoncxx-dev
```
Za druge distribucije Linuxa uporabite ustrezen upravitelj paketov.

**Testno ogrodje Catch2 bo samodejno preneseno in konfigurirano s strani CMake med postopkom konfiguracije.**

## Postopek prevajanja in zagona s CMake

1.  **Odprite terminal ali ukazno vrstico.**

2.  **Pomaknite se v korensko mapo projekta** (mapa, ki vsebuje datoteko `CMakeLists.txt`):
    ```bash
    cd pot/do/projekta/rudniki-backend
    ```

3.  **Konfigurirajte projekt s CMake.** Ta korak bo ustvaril gradbene datoteke v podmapi `build` in po potrebi prenesel ter konfiguriral Catch2.
    *   **Če ste na Windows uporabili `.\vcpkg integrate install` (ali za macOS/Linux):**
        ```bash
        cmake -S . -B build
        ```
    *   **Če na Windows niste uporabili `.\vcpkg integrate install` (ali želite eksplicitno navesti toolchain):**
        (Zamenjajte `[pot/do/vcpkg]` z dejansko potjo do vaše `vcpkg` namestitve)
        ```bash
        cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=[pot/do/vcpkg]/scripts/buildsystems/vcpkg.cmake
        ```

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
    Izvedljiva datoteka `rudnik_app` (ali `rudnik_app.exe` na Windows) bo ustvarjena v ustrezni podmapi znotraj mape `build` (npr. `build/Debug` ali `build/Release`, odvisno od `--config` zastavice).

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
    Ta ukaz bo prevedel teste, če še niso, in jih nato zagnal.
    ```bash
    cmake --build build --target test --config Debug
    ```    (Zamenjajte `Debug` z `Release` po potrebi)

3.  **Neposredno z izvajanjem testne datoteke (iz mape `build`):**
    Testna izvedljiva datoteka (npr. `rudniki_tests` ali `rudniki_tests.exe`) bo v podmapi znotraj mape `build` (npr. `build/test/Debug/` ali `build/test/Release/`).
    * Na Linux/macOS:
      ```bash
      ./build/test/Debug/rudniki_tests
      ```
    * Na Windows:
      ```powershell
      .\build\test\Debug\rudniki_tests.exe
      ```
 