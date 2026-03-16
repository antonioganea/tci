mkdir release
cd release
mkdir TCI
cd TCI
mkdir injector
copy ".\..\..\build\Debug\AutoInjector.exe" ".\injector"
copy ".\..\..\build\Debug\AutoInjector.pdb" ".\injector"
copy ".\..\..\build\Debug\TCI_DLL.dll" ".\injector"
copy ".\..\..\build\Debug\TCI_DLL.pdb" ".\injector"
copy ".\..\..\start-injected.bat" "."
xcopy /E /I ".\..\..\lua" ".\lua"
xcopy /E /I ".\..\..\mpmissions" ".\mpmissions"
7z a -tzip ".\..\..\TCI.zip" ".\*"
cd ".\..\.."
rmdir /s /q ".\release"
pause