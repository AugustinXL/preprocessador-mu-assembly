@echo off
setlocal

echo ========================================
echo TESTES DE INTEGRACAO - PRE-PROCESSADOR
echo ========================================

gcc main.c preprocessador.c f_quebras.c -o main.exe
if errorlevel 1 (
    echo.
    echo ERRO: o projeto ainda nao esta compilando.
    echo Isso pode acontecer enquanto os membros 1, 3 e 4 ainda estao integrando as funcoes.
    exit /b 1
)

set FALHAS=0

call :teste crlf
call :teste lf
call :teste sem_quebra
call :teste vazio
call :teste linhas_vazias

echo.
echo ========================================
if "%FALHAS%"=="0" (
    echo RESULTADO: TODOS OS TESTES PASSARAM
    exit /b 0
) else (
    echo RESULTADO: %FALHAS% TESTE^(S^) FALHARAM
    exit /b 1
)

:teste
set NOME=%~1
main.exe "testes_membro2\%NOME%.asm" "testes_membro2\%NOME%.saida.pre" >nul
if errorlevel 1 (
    echo [FALHOU] %NOME% - executavel retornou erro
    set /a FALHAS+=1
    goto :eof
)

fc /b "testes_membro2\%NOME%.saida.pre" "testes_membro2\%NOME%.pre" >nul
if errorlevel 1 (
    echo [FALHOU] %NOME% - saida diferente do esperado
    set /a FALHAS+=1
) else (
    echo [OK] %NOME%
)

goto :eof
