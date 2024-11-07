del build\* /S /Q
rmdir /s /q build
mkdir build
cd build
cmake ..
cmake --build .  --config Release
cmake --install .
cpack
cd ..
