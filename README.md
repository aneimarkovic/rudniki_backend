# Navodila uporabe: backend rudniki:

### MacOS:

Namestitev, knjižnice Boost.beast:
`brew install boost` \
Prevajanje programa z prevajalnikom `clang++`:
```zsh
clang++ -std=c++17 src/index.cpp src/HttpServer.cpp -o output \
  -I./includes \
  -I/opt/homebrew/opt/boost/include \
  -L/opt/homebrew/opt/boost/lib \
  -lboost_system -lpthread
```
Zagon programa:
```zsh
./output
```
