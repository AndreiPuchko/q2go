cd zlib-1.3.1
echo %cd%

if not exist build 	(
	mkdir build
	)
cd build
set "@cmake=\qt\Tools\CMake_64\bin\cmake.exe"

%@cmake% -S .. -B . -DCMAKE_INSTALL_PREFIX="../../zlib.install"
%@cmake% --build . --config Release
%@cmake% --install . --config Release

cd ../..