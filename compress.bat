@echo off
set "sourceFolder=%cd%"
set "zipFile=BUAA_Compiler.zip"

if exist "%zipFile%" (
    del "%zipFile%"
)

7z a -r "%zipFile%" "%sourceFolder%\src\*"

echo 压缩完成！