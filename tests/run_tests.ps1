# ==============================================================================
# Script Runner de Testes Automatizados - Compilador Mini-JS
# ==============================================================================

$ErrorActionPreference = "Continue"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$BaseDir = Split-Path -Parent $ScriptDir

# Localiza o executavel principal do compilador
$Minijs = $null
$MinijsCandidates = @(
    (Join-Path $BaseDir "minijs.exe"),
    (Join-Path $BaseDir "minijs"),
    (Join-Path (Get-Location) "minijs.exe"),
    (Join-Path (Get-Location) "minijs")
)

foreach ($candidate in $MinijsCandidates) {
    if (Test-Path $candidate) {
        $Minijs = (Resolve-Path $candidate).Path
        break
    }
}

if (-not $Minijs) {
    Write-Host "[ERRO] Executavel 'minijs' nao foi encontrado. Execute 'make' antes de rodar os testes." -ForegroundColor Red
    exit 1
}

# Localiza o executavel de testes unitarios em memoria (se compilado)
$TestUnit = $null
$TestUnitCandidates = @(
    (Join-Path $BaseDir "test_unit.exe"),
    (Join-Path $BaseDir "test_unit"),
    (Join-Path (Get-Location) "test_unit.exe"),
    (Join-Path (Get-Location) "test_unit")
)

foreach ($candidate in $TestUnitCandidates) {
    if (Test-Path $candidate) {
        $TestUnit = (Resolve-Path $candidate).Path
        break
    }
}

$Total = 0
$Passaram = 0
$Falharam = 0

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "  INICIANDO SUITE DE TESTES AUTOMATIZADOS - MINI-JS        " -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "Executavel do compilador: $Minijs`n"

# 1. Execucao de Testes Unitarios em Memoria (se disponivel)
if ($TestUnit) {
    & $TestUnit
    if ($LASTEXITCODE -eq 0) {
        $Total++
        $Passaram++
    } else {
        $Total++
        $Falharam++
    }
    Write-Host ""
}

# 2. Casos de Teste Validos
Write-Host "--- Executando Casos de Teste Validos ---" -ForegroundColor Yellow
$ValidosDir = Join-Path $ScriptDir "validos"
if (Test-Path $ValidosDir) {
    $ArquivosValidos = Get-ChildItem -Path $ValidosDir -Filter "*.js" | Sort-Object Name
    foreach ($arquivo in $ArquivosValidos) {
        $Total++
        $output = & $Minijs $arquivo.FullName 2>&1
        $code = $LASTEXITCODE

        if ($code -eq 0) {
            $Passaram++
            Write-Host "  [PASSOU] $($arquivo.Name)" -ForegroundColor Green
        } else {
            $Falharam++
            Write-Host "  [FALHOU] $($arquivo.Name) (esperava codigo 0, obteve $code)" -ForegroundColor Red
            if ($output) {
                Write-Host "           Saida: $($output -join ' ')" -ForegroundColor DarkGray
            }
        }
    }
} else {
    Write-Host "  [AVISO] Diretorio de casos validos nao encontrado: $ValidosDir" -ForegroundColor DarkYellow
}

# 3. Casos de Teste Invalidos
Write-Host "`n--- Executando Casos de Teste Invalidos (Falha Esperada) ---" -ForegroundColor Yellow
$InvalidosDir = Join-Path $ScriptDir "invalidos"
if (Test-Path $InvalidosDir) {
    $ArquivosInvalidos = Get-ChildItem -Path $InvalidosDir -Filter "*.js" | Sort-Object Name
    foreach ($arquivo in $ArquivosInvalidos) {
        $Total++
        $output = & $Minijs $arquivo.FullName 2>&1
        $code = $LASTEXITCODE

        if ($code -ne 0) {
            $Passaram++
            Write-Host "  [PASSOU] $($arquivo.Name) (falha sintatica esperada, codigo: $code)" -ForegroundColor Green
        } else {
            $Falharam++
            Write-Host "  [FALHOU] $($arquivo.Name) (esperava codigo de erro != 0, mas retornou 0)" -ForegroundColor Red
        }
    }
} else {
    Write-Host "  [AVISO] Diretorio de casos invalidos nao encontrado: $InvalidosDir" -ForegroundColor DarkYellow
}

# 4. Sumario Final
Write-Host "`n============================================================" -ForegroundColor Cyan
Write-Host "                   SUMARIO DOS TESTES                      " -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "  Total de testes executados: $Total"
Write-Host "  Testes que passaram:       $Passaram" -ForegroundColor Green
if ($Falharam -gt 0) {
    Write-Host "  Testes que falharam:       $Falharam" -ForegroundColor Red
    Write-Host "============================================================" -ForegroundColor Red
    exit 1
} else {
    Write-Host "  Testes que falharam:       0" -ForegroundColor Green
    Write-Host "============================================================" -ForegroundColor Green
    exit 0
}
