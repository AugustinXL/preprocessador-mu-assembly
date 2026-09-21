@echo off
REM ==========================================================================
REM  testes.bat - pre-processador uAssembly
REM
REM  Executar na RAIZ do projeto (junto de main.c):   testes.bat
REM
REM  Compila o projeto e roda o pre-processador em todos os .asm da pasta
REM  testes\, exibindo o resultado de cada um.
REM
REM    testes\<nome>.asm   entrada
REM    testes\<nome>.pre   saida gerada pelo programa
REM
REM  Nao ha comparacao com gabarito - a conferencia e visual.
REM ==========================================================================

setlocal

echo ========================================
echo PRE-PROCESSADOR - SAIDAS GERADAS
echo ========================================

gcc main.c preprocessador.c lexico.c tabela_simbolos.c -o main.exe -Wall -Wextra
if errorlevel 1 (
    echo.
    echo ERRO: o projeto ainda nao esta compilando.
    echo Verifique se todas as funcoes foram integradas em preprocessador.c.
    exit /b 1
)

if not exist "testes" (
    echo ERRO: pasta "testes" nao encontrada.
    exit /b 1
)

set TOTAL=0
set ERROS=0

for %%F in (testes\*.asm) do call :processar "%%~nF"

echo.
echo ========================================
echo %TOTAL% arquivo^(s^) processado^(s^), %ERROS% com erro.
echo Saidas gravadas em testes\
exit /b 0

REM --------------------------------------------------------------------------
REM  :processar <nome sem extensao>
REM --------------------------------------------------------------------------
:processar
set NOME=%~1
set /a TOTAL+=1

echo.
echo ==================================================
echo  %NOME%.asm
echo ==================================================

main.exe "testes\%NOME%.asm" "testes\%NOME%.pre" "testes\%NOME%.lex" >nul
if errorlevel 1 (
    echo [ERRO] o executavel retornou codigo de erro.
    set /a ERROS+=1
    goto :eof
)

type "testes\%NOME%.pre"
goto :eof