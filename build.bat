@echo off
REM Définir les chemins vers les fichiers Raylib
set RAYLIB_INCLUDE=C:\raylib\include
set RAYLIB_LIB=C:\raylib\lib

REM Vérifier si les chemins de Raylib existent
if not exist %RAYLIB_INCLUDE% (
    echo Erreur : Le chemin %RAYLIB_INCLUDE% n'existe pas.
    pause
    exit /b 1
)

if not exist %RAYLIB_LIB% (
    echo Erreur : Le chemin %RAYLIB_LIB% n'existe pas.
    pause
    exit /b 1
)

REM Créer le dossier build s'il n'existe pas
if not exist build (
    mkdir build
)

REM Compiler le projet avec g++
g++ src\main.cpp src\Game.cpp src\Entities.cpp src\Dwarf.cpp src\Map.cpp ^
-I include -I %RAYLIB_INCLUDE% -L %RAYLIB_LIB% -o build\game.exe ^
-lraylib -lopengl32 -lgdi32 -lwinmm

REM Vérifier si la compilation a réussi
if %ERRORLEVEL% neq 0 (
    echo La compilation a échoué.
    pause
    exit /b %ERRORLEVEL%
)

echo Compilation réussie !
pause
