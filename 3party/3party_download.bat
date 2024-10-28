@echo off
set "@zlib_version=zlib-1.3.1"

if not exist "%@zlib_version%" (
	echo https://zlib.net/%@zlib_version%.tar.gz
	curl  https://zlib.net/%@zlib_version%.tar.gz -O	
	tar -xzf %@zlib_version%.tar.gz
	del %@zlib_version%.tar.gz
	) else echo %@zlib_version% is here

if not exist quazip (
  git clone -b master --depth 1 --single-branch https://github.com/stachenov/quazip.git
  rmdir /s /q quazip\.git
  rmdir /s /q quazip\.github
  )else echo quazip is here

if not exist bzip2 (
  git clone -b master --depth 1 --single-branch https://sourceware.org/git/bzip2.git
  rmdir /s /q bzip2\.git
  rmdir /s /q bzip2\.github
  )else echo bzip2 is here


