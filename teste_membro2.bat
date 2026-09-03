@echo off
setlocal

echo ========================================
echo TESTES UNITARIOS - MEMBRO 2
echo ========================================

if not exist teste_membro2.exe (
    echo Compilando teste_membro2.exe...
    gcc teste_membro2.c f_quebras.c -o teste_membro2.exe
    if errorlevel 1 (
        echo ERRO: falha na compilacao.
        exit /b 1
    )
)

teste_membro2.exe
if errorlevel 1 (
    echo.
    echo RESULTADO: FALHOU
    exit /b 1
)

echo.
echo RESULTADO: TODOS OS TESTES PASSARAM
exit /b 0
