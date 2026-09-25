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

# Funcao auxiliar para execucao parametrizada de suites de testes
function Invoke-TestSuite {
    param (
        [string]$Dir,
        [string]$Titulo,
        [bool]$ExpectSuccess
    )

    Write-Host "--- $Titulo ---" -ForegroundColor Yellow
    if (-not (Test-Path $Dir)) {
        Write-Host "  [AVISO] Diretorio nao encontrado: $Dir" -ForegroundColor DarkYellow
        return
    }

    $arquivos = Get-ChildItem -Path $Dir -Filter "*.js" | Sort-Object Name
    foreach ($arquivo in $arquivos) {
        $script:Total++
        $output = & $script:Minijs $arquivo.FullName 2>&1
        $code = $LASTEXITCODE

        $passou = ($code -eq 0) -eq $ExpectSuccess
        if ($passou) {
            $script:Passaram++
            if ($ExpectSuccess) {
                Write-Host "  [PASSOU] $($arquivo.Name)" -ForegroundColor Green
            } else {
                Write-Host "  [PASSOU] $($arquivo.Name) (falha sintatica esperada, codigo: $code)" -ForegroundColor Green
            }
        } else {
            $script:Falharam++
            if ($ExpectSuccess) {
                Write-Host "  [FALHOU] $($arquivo.Name) (esperava codigo 0, obteve $code)" -ForegroundColor Red
            } else {
                Write-Host "  [FALHOU] $($arquivo.Name) (esperava codigo != 0, mas retornou 0)" -ForegroundColor Red
            }
        }
    }
}

# 2. Casos de Teste Validos
Invoke-TestSuite -Dir (Join-Path $ScriptDir "validos") -Titulo "Executando Casos de Teste Validos" -ExpectSuccess $true

# 3. Casos de Teste Invalidos
Write-Host ""
Invoke-TestSuite -Dir (Join-Path $ScriptDir "invalidos") -Titulo "Executando Casos de Teste Invalidos (Falha Esperada)" -ExpectSuccess $false

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
