# Navodila uporabe: backend rudniki:

### MacOS:

Namestitev, knjižnice Boost.beast:
`brew install boost` \
Namestitev knjižnice mongocxx:
`brew install mongo-cxx-driver`\
Prevajanje programa z prevajalnikom `clang++`:
```zsh
rudniki_backend % clang++ -std=c++17 \
  src/index.cpp src/HttpServer.cpp src/DatabaseHandler.cpp \
  -o output \
  -I./includes \
  -I/opt/homebrew/include \
  -I/opt/homebrew/opt/mongo-cxx-driver/include/bsoncxx/v_noabi \
  -I/opt/homebrew/opt/mongo-cxx-driver/include/mongocxx/v_noabi \
  -L/opt/homebrew/opt/mongo-cxx-driver/lib \
  -L/opt/homebrew/opt/boost/lib \
  -lboost_system -lpthread \
  -lmongocxx -lbsoncxx
```
Zagon programa:
```zsh
./output
```
