set q2go_release=Desktop_Qt_6_7_3_MinGW_64_bit-Release
set q2go_debug=Desktop_Qt_6_7_3_MinGW_64_bit-Debug

copy ..\build\%q2go_release%\q2go.exe .\q2go.exe
copy ..\build\%q2go_debug%\q2go.exe .\q2go-debug.exe