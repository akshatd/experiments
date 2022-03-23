# Experiments

## Prerequisites
1. Install conan, systemwide is OK through pamac or something
2. After installation, check that the c++ abi used is post c++11 by following [this link](https://docs.conan.io/en/latest/howtos/manage_gcc_abi.html)
  - Basically in `~/.conan/profiles/default`, the `compiler.libcxx` needs to be `libstdc++11` and not `libstdc++`
  - can be done easily by running this 
	```bash
	conan profile update settings.compiler.libcxx=libstdc++11 default
	```

## Build
```
mkdir build && cd build
conan install .. --build=missing
cmake .. && cmake --build . -j $(nproc)
```

if easylogging gives issues, just do this and try conan install again
```
conan remove easyloggingpp
```
