@echo off
setlocal enabledelayedexpansion

chcp 65001 > nul

REM Cores
set "G=[92m"
set "Y=[93m"
set "R=[91m"
set "B=[94m"
set "X=[0m"

REM Ir para raiz do projeto
cd /d "%~dp0.."

cls
echo.
echo %B%=====================================================================%X%
echo %B%         Kaiser Family Budgets - Deploy Script v1.0                 %X%
echo %B%=====================================================================%X%
echo.
echo Pasta do projeto: %CD%
echo.

REM Verificar projeto
if not exist "FamilyBudgets.cbp" (
    echo %R%ERRO: FamilyBudgets.cbp nao encontrado!%X%
    pause
    exit /b 1
)

REM Menu
echo Selecione o build:
echo   1 - Release
echo   2 - Debug  
echo   3 - Ambos
echo.
set /p ESCOLHA="Digite 1, 2 ou 3: "

if "%ESCOLHA%"=="1" goto release
if "%ESCOLHA%"=="2" goto debug
if "%ESCOLHA%"=="3" goto ambos

echo %R%Opcao invalida!%X%
pause
exit /b 1

REM ====================================================================
:release
set "MODE=Release"
set "BINDIR=bin\Release"
set "SUFFIX=u"
goto processar

REM ====================================================================
:debug
set "MODE=Debug"
set "BINDIR=bin\Debug"
set "SUFFIX=ud"
goto processar

REM ====================================================================
:ambos
REM Primeiro Release
set "MODE=Release"
set "BINDIR=bin\Release"
set "SUFFIX=u"
call :fazer_deploy

REM Depois Debug
set "MODE=Debug"
set "BINDIR=bin\Debug"
set "SUFFIX=ud"
call :fazer_deploy

goto fim

REM ====================================================================
:processar
call :fazer_deploy
goto fim

REM ====================================================================
:fazer_deploy

echo.
echo %B%=====================================================================%X%
echo %B%                       PROCESSANDO %MODE%                           %X%
echo %B%=====================================================================%X%
echo.

REM [1] Verificar executável
echo %Y%[1/6] Verificando executavel...%X%
if not exist "%BINDIR%\FamilyBudgets.exe" (
    echo %R%ERRO: Executavel nao encontrado em %BINDIR%%X%
    echo %R%Compile o projeto no Code::Blocks primeiro!%X%
    pause
    exit /b 1
)
echo %G%OK - Executavel encontrado%X%

REM [2] Criar pastas
echo.
echo %Y%[2/6] Criando pastas de recursos...%X%
mkdir "%BINDIR%\resources" 2>nul
mkdir "%BINDIR%\resources\icons" 2>nul
mkdir "%BINDIR%\resources\database" 2>nul
mkdir "%BINDIR%\data" 2>nul
echo %G%OK - Pastas criadas%X%

REM [3] Copiar ícones
echo.
echo %Y%[3/6] Copiando recursos...%X%

if exist "resources\icons\*.png" (
    copy "resources\icons\*.png" "%BINDIR%\resources\icons\" >nul 2>&1
    echo %G%OK - Icones PNG copiados%X%
) else (
    echo %Y%Aviso: Icones PNG nao encontrados%X%
)

if exist "resources\icons\*.ico" (
    copy "resources\icons\*.ico" "%BINDIR%\resources\icons\" >nul 2>&1
    echo %G%OK - Icones ICO copiados%X%
) else (
    echo %Y%Aviso: Icones ICO nao encontrados%X%
)

if exist "resources\database\schema.sql" (
    copy "resources\database\schema.sql" "%BINDIR%\resources\database\" >nul 2>&1
    echo %G%OK - Schema SQL copiado%X%
) else (
    echo %Y%Aviso: Schema SQL nao encontrado%X%
)

REM [4] Detectar wxWidgets 3.3.0
echo.
echo %Y%[4/6] Detectando wxWidgets 3.3.0...%X%

set "WXDIR="
if exist "C:\wxWidgets-3.3.0\lib\gcc_dll\wxbase330%SUFFIX%_gcc_custom.dll" (
    set "WXDIR=C:\wxWidgets-3.3.0\lib\gcc_dll"
) else if exist "C:\wxWidgets\lib\gcc_dll\wxbase330%SUFFIX%_gcc_custom.dll" (
    set "WXDIR=C:\wxWidgets\lib\gcc_dll"
) else if exist "D:\wxWidgets-3.3.0\lib\gcc_dll\wxbase330%SUFFIX%_gcc_custom.dll" (
    set "WXDIR=D:\wxWidgets-3.3.0\lib\gcc_dll"
)

if "%WXDIR%"=="" (
    echo %Y%wxWidgets 3.3.0 nao detectado automaticamente%X%
    set /p "WXDIR=Digite o caminho da pasta gcc_dll do wxWidgets: "
)

if not exist "%WXDIR%\wxbase330%SUFFIX%_gcc_custom.dll" (
    echo %R%ERRO: DLLs do wxWidgets nao encontradas em %WXDIR%%X%
    pause
    exit /b 1
)

echo %G%OK - wxWidgets: %WXDIR%%X%

REM [5] Detectar MinGW
echo.
echo %Y%[5/6] Detectando MinGW...%X%

set "MINGW="
if exist "C:\msys64\mingw64\bin\gcc.exe" set "MINGW=C:\msys64\mingw64\bin"
if exist "C:\mingw64\bin\gcc.exe" set "MINGW=C:\mingw64\bin"
if exist "C:\msys2\mingw64\bin\gcc.exe" set "MINGW=C:\msys2\mingw64\bin"
if exist "D:\msys64\mingw64\bin\gcc.exe" set "MINGW=D:\msys64\mingw64\bin"

if "%MINGW%"=="" (
    echo %Y%MinGW nao detectado automaticamente%X%
    set /p "MINGW=Digite o caminho do MinGW bin: "
)

if not exist "%MINGW%\gcc.exe" (
    echo %R%ERRO: MinGW nao encontrado em %MINGW%%X%
    pause
    exit /b 1
)

echo %G%OK - MinGW: %MINGW%%X%

REM [6] Copiar DLLs
echo.
echo %Y%[6/6] Copiando DLLs...%X%
echo.

set "COPIED=0"

REM wxWidgets 3.3.0 (do diretório próprio)
echo %Y%wxWidgets 3.3.0:%X%

if exist "%WXDIR%\wxbase330%SUFFIX%_gcc_custom.dll" (
    copy "%WXDIR%\wxbase330%SUFFIX%_gcc_custom.dll" "%BINDIR%\" >nul 2>&1
    echo %G%  + wxbase330%SUFFIX%_gcc_custom.dll%X%
    set /a COPIED+=1
) else (
    echo %R%  - wxbase330%SUFFIX%_gcc_custom.dll [FALTANDO]%X%
)

if exist "%WXDIR%\wxmsw330%SUFFIX%_core_gcc_custom.dll" (
    copy "%WXDIR%\wxmsw330%SUFFIX%_core_gcc_custom.dll" "%BINDIR%\" >nul 2>&1
    echo %G%  + wxmsw330%SUFFIX%_core_gcc_custom.dll%X%
    set /a COPIED+=1
) else (
    echo %R%  - wxmsw330%SUFFIX%_core_gcc_custom.dll [FALTANDO]%X%
)

REM DLLs extras do wxWidgets (opcionais)
if exist "%WXDIR%\wxmsw330%SUFFIX%_adv_gcc_custom.dll" (
    copy "%WXDIR%\wxmsw330%SUFFIX%_adv_gcc_custom.dll" "%BINDIR%\" >nul 2>&1
    set /a COPIED+=1
)

if exist "%WXDIR%\wxmsw330%SUFFIX%_html_gcc_custom.dll" (
    copy "%WXDIR%\wxmsw330%SUFFIX%_html_gcc_custom.dll" "%BINDIR%\" >nul 2>&1
    set /a COPIED+=1
)

if exist "%WXDIR%\wxmsw330%SUFFIX%_propgrid_gcc_custom.dll" (
    copy "%WXDIR%\wxmsw330%SUFFIX%_propgrid_gcc_custom.dll" "%BINDIR%\" >nul 2>&1
    set /a COPIED+=1
)

if exist "%WXDIR%\wxmsw330%SUFFIX%_ribbon_gcc_custom.dll" (
    copy "%WXDIR%\wxmsw330%SUFFIX%_ribbon_gcc_custom.dll" "%BINDIR%\" >nul 2>&1
    set /a COPIED+=1
)

if exist "%WXDIR%\wxmsw330%SUFFIX%_richtext_gcc_custom.dll" (
    copy "%WXDIR%\wxmsw330%SUFFIX%_richtext_gcc_custom.dll" "%BINDIR%\" >nul 2>&1
    set /a COPIED+=1
)

if exist "%WXDIR%\wxmsw330%SUFFIX%_stc_gcc_custom.dll" (
    copy "%WXDIR%\wxmsw330%SUFFIX%_stc_gcc_custom.dll" "%BINDIR%\" >nul 2>&1
    set /a COPIED+=1
)

if exist "%WXDIR%\wxmsw330%SUFFIX%_webview_gcc_custom.dll" (
    copy "%WXDIR%\wxmsw330%SUFFIX%_webview_gcc_custom.dll" "%BINDIR%\" >nul 2>&1
    set /a COPIED+=1
)

if exist "%WXDIR%\wxmsw330%SUFFIX%_xrc_gcc_custom.dll" (
    copy "%WXDIR%\wxmsw330%SUFFIX%_xrc_gcc_custom.dll" "%BINDIR%\" >nul 2>&1
    set /a COPIED+=1
)

if exist "%WXDIR%\wxbase330%SUFFIX%_net_gcc_custom.dll" (
    copy "%WXDIR%\wxbase330%SUFFIX%_net_gcc_custom.dll" "%BINDIR%\" >nul 2>&1
    set /a COPIED+=1
)

if exist "%WXDIR%\wxbase330%SUFFIX%_xml_gcc_custom.dll" (
    copy "%WXDIR%\wxbase330%SUFFIX%_xml_gcc_custom.dll" "%BINDIR%\" >nul 2>&1
    set /a COPIED+=1
)

REM Runtime MinGW (do MinGW)
echo.
echo %Y%Runtime MinGW:%X%
if exist "%MINGW%\libgcc_s_seh-1.dll" (
    copy "%MINGW%\libgcc_s_seh-1.dll" "%BINDIR%\" >nul 2>&1
    echo %G%  + libgcc_s_seh-1.dll%X%
    set /a COPIED+=1
)

if exist "%MINGW%\libstdc++-6.dll" (
    copy "%MINGW%\libstdc++-6.dll" "%BINDIR%\" >nul 2>&1
    echo %G%  + libstdc++-6.dll%X%
    set /a COPIED+=1
)

if exist "%MINGW%\libwinpthread-1.dll" (
    copy "%MINGW%\libwinpthread-1.dll" "%BINDIR%\" >nul 2>&1
    echo %G%  + libwinpthread-1.dll%X%
    set /a COPIED+=1
)

REM SQLite (do MinGW)
echo.
echo %Y%SQLite:%X%
if exist "%MINGW%\libsqlite3-0.dll" (
    copy "%MINGW%\libsqlite3-0.dll" "%BINDIR%\" >nul 2>&1
    echo %G%  + libsqlite3-0.dll%X%
    set /a COPIED+=1
)

REM Bibliotecas de imagem (do MinGW)
echo.
echo %Y%Bibliotecas de imagem:%X%
if exist "%MINGW%\zlib1.dll" (
    copy "%MINGW%\zlib1.dll" "%BINDIR%\" >nul 2>&1
    echo %G%  + zlib1.dll%X%
    set /a COPIED+=1
)

if exist "%MINGW%\libpng16-16.dll" (
    copy "%MINGW%\libpng16-16.dll" "%BINDIR%\" >nul 2>&1
    echo %G%  + libpng16-16.dll%X%
    set /a COPIED+=1
)

if exist "%MINGW%\libjpeg-8.dll" (
    copy "%MINGW%\libjpeg-8.dll" "%BINDIR%\" >nul 2>&1
    echo %G%  + libjpeg-8.dll%X%
    set /a COPIED+=1
)

if exist "%MINGW%\libtiff-5.dll" (
    copy "%MINGW%\libtiff-5.dll" "%BINDIR%\" >nul 2>&1
    echo %G%  + libtiff-5.dll%X%
    set /a COPIED+=1
)

if exist "%MINGW%\libexpat-1.dll" (
    copy "%MINGW%\libexpat-1.dll" "%BINDIR%\" >nul 2>&1
    echo %G%  + libexpat-1.dll%X%
    set /a COPIED+=1
)

if exist "%MINGW%\liblzma-5.dll" (
    copy "%MINGW%\liblzma-5.dll" "%BINDIR%\" >nul 2>&1
    echo %G%  + liblzma-5.dll%X%
    set /a COPIED+=1
)

if exist "%MINGW%\libzstd.dll" (
    copy "%MINGW%\libzstd.dll" "%BINDIR%\" >nul 2>&1
    echo %G%  + libzstd.dll%X%
    set /a COPIED+=1
)

if exist "%MINGW%\libwebp-7.dll" (
    copy "%MINGW%\libwebp-7.dll" "%BINDIR%\" >nul 2>&1
    echo %G%  + libwebp-7.dll%X%
    set /a COPIED+=1
)

if exist "%MINGW%\libbz2-1.dll" (
    copy "%MINGW%\libbz2-1.dll" "%BINDIR%\" >nul 2>&1
    echo %G%  + libbz2-1.dll%X%
    set /a COPIED+=1
)

if exist "%MINGW%\libiconv-2.dll" (
    copy "%MINGW%\libiconv-2.dll" "%BINDIR%\" >nul 2>&1
    echo %G%  + libiconv-2.dll%X%
    set /a COPIED+=1
)

echo.
echo %G%Total de DLLs copiadas: %COPIED%%X%

REM Criar README
(
echo ================================================================================
echo                      Kaiser Family Budgets v1.0
echo                    Sistema de Gestao Financeira Familiar
echo ================================================================================
echo.
echo Build: %MODE%
echo Data: %date% %time%
echo.
echo COMO EXECUTAR:
echo   Execute: FamilyBudgets.exe
echo.
echo ESTRUTURA:
echo   FamilyBudgets.exe ............ Executavel principal
echo   *.dll ........................ Bibliotecas necessarias
echo   resources\icons\ ............. Icones da interface
echo   resources\database\ .......... Schema do banco
echo   data\ ........................ Dados do usuario (criado automaticamente)
echo.
echo BACKUP:
echo   Para backup, copie: data\familybudgets.db
echo.
echo Desenvolvido por Isac Soares Barreto - 2025
) > "%BINDIR%\README.txt"

REM Criar run.bat
(
echo @echo off
echo title Kaiser Family Budgets - %MODE%
echo start "" FamilyBudgets.exe
) > "%BINDIR%\run.bat"

echo.
echo %G%=====================================================================%X%
echo %G%              Deploy %MODE% concluido com sucesso!                  %X%
echo %G%=====================================================================%X%
echo.

exit /b 0

REM ====================================================================
:fim
echo.
echo %G%=====================================================================%X%
echo %G%                    DEPLOY FINALIZADO!                              %X%
echo %G%=====================================================================%X%
echo.

set /p TESTAR="Deseja testar o programa? (S/N): "
if /i "%TESTAR%"=="S" (
    if exist "bin\Release\FamilyBudgets.exe" (
        echo Abrindo versao Release...
        start "" "bin\Release\FamilyBudgets.exe"
    ) else if exist "bin\Debug\FamilyBudgets.exe" (
        echo Abrindo versao Debug...
        start "" "bin\Debug\FamilyBudgets.exe"
    )
)

pause
exit /b 0