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
        [bool]$ExpectSuccess,
        # Fase da falha esperada, usada na mensagem
        [string]$Fase = "sintatica",
        # Texto que deve aparecer na saida da falha. Sem ele, um erro de outra
        # fase (ex.: sintaxe num teste semantico) passaria so pelo codigo de saida.
        [string]$Marcador = ""
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
        $texto = ($output | ForEach-Object { "$_" }) -join "`n"

        $passou = ($code -eq 0) -eq $ExpectSuccess
        if ($Marcador -and $code -ne 0 -and -not $texto.Contains($Marcador)) {
            $script:Falharam++
            Write-Host "  [FALHOU] $($arquivo.Name) (falhou com codigo $code, mas sem '$Marcador' na saida)" -ForegroundColor Red
        } elseif ($passou) {
            $script:Passaram++
            if ($ExpectSuccess) {
                Write-Host "  [PASSOU] $($arquivo.Name)" -ForegroundColor Green
            } else {
                Write-Host "  [PASSOU] $($arquivo.Name) (falha $Fase esperada, codigo: $code)" -ForegroundColor Green
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

# 4. Casos de Teste Invalidos Semanticos (sintaxe valida, erro semantico esperado)
Write-Host ""
Invoke-TestSuite -Dir (Join-Path $ScriptDir "invalidos_semanticos") -Titulo "Executando Casos de Teste Invalidos Semanticos (Falha Esperada)" -ExpectSuccess $false -Fase "semantica" -Marcador "Erro semantico"

# 5. Saida da AST e execucao silenciosa sem --ast
Write-Host "`n--- Executando Casos de Teste de AST ---" -ForegroundColor Yellow
$AstDir = Join-Path $ScriptDir "ast"
$AstErros = [System.IO.Path]::GetTempFileName()
$AstCodificacaoOriginal = [Console]::OutputEncoding
try {
    # Os conectores da arvore sao UTF-8, inclusive no Windows PowerShell 5.1.
    [Console]::OutputEncoding = New-Object System.Text.UTF8Encoding($false)
    foreach ($arquivo in (Get-ChildItem -Path $AstDir -Filter "*.js" | Sort-Object Name)) {
        $Total++
        $esperado = [System.IO.Path]::ChangeExtension($arquivo.FullName, ".esperado")
        $output = @(& $Minijs --ast $arquivo.FullName 2> $AstErros)
        $code = $LASTEXITCODE
        # Normaliza apenas CRLF: espacos, ordem e linhas extras continuam significativos.
        $texto = if ($output.Count -gt 0) { ($output -join "`n") + "`n" } else { "" }
        if ((Test-Path $esperado) -and $code -eq 0 -and (Get-Item $AstErros).Length -eq 0) {
            $referencia = [System.IO.File]::ReadAllText($esperado).Replace("`r`n", "`n")
            $passou = $texto -ceq $referencia
        } else {
            $passou = $false
        }
        if ($passou) {
            $Passaram++
            Write-Host "  [PASSOU] $($arquivo.Name) (--ast)" -ForegroundColor Green
        } else {
            $Falharam++
            Write-Host "  [FALHOU] $($arquivo.Name) (--ast)" -ForegroundColor Red
            Get-Content $AstErros
        }

        $Total++
        $output = @(& $Minijs $arquivo.FullName 2> $AstErros)
        $code = $LASTEXITCODE
        if ($code -eq 0 -and $output.Count -eq 0 -and (Get-Item $AstErros).Length -eq 0) {
            $Passaram++
            Write-Host "  [PASSOU] $($arquivo.Name) (sem --ast)" -ForegroundColor Green
        } else {
            $Falharam++
            Write-Host "  [FALHOU] $($arquivo.Name) (sem --ast)" -ForegroundColor Red
        }
    }
} finally {
    [Console]::OutputEncoding = $AstCodificacaoOriginal
    Remove-Item $AstErros
}

# 6. Opcoes de linha de comando: opcao desconhecida e erro, e nao nome de arquivo
Write-Host "`n--- Executando Casos de Teste de Opcoes ---" -ForegroundColor Yellow
$ArquivoValido = Join-Path (Join-Path $ScriptDir "validos") "01_declaracoes.js"
foreach ($opcao in @("--xyz", "-x")) {
    $Total++
    $output = & $Minijs $opcao $ArquivoValido 2>&1
    $code = $LASTEXITCODE
    $texto = ($output | ForEach-Object { "$_" }) -join "`n"
    if ($code -ne 0 -and $texto.Contains("opcao desconhecida")) {
        $Passaram++
        Write-Host "  [PASSOU] opcao $opcao rejeitada (codigo: $code)" -ForegroundColor Green
    } else {
        $Falharam++
        Write-Host "  [FALHOU] opcao $opcao (esperava erro de opcao desconhecida, codigo: $code)" -ForegroundColor Red
    }
}

# 7. Geracao de Python: --python, -o e execucao do .py gerado
Write-Host "`n--- Executando Casos de Teste de Geracao de Python ---" -ForegroundColor Yellow
$Python = $null
foreach ($candidato in @("python3", "python", "py")) {
    # "--version" descarta o atalho da Microsoft Store, que existe no PATH mas nao executa o Python
    if (Get-Command $candidato -ErrorAction SilentlyContinue) {
        & $candidato --version *> $null
        if ($LASTEXITCODE -eq 0) {
            $Python = $candidato
            break
        }
    }
}
$Pulados = 0
if (-not $Python) {
    Write-Host "  [AVISO] Python nao encontrado: os .py gerados nao serao executados" -ForegroundColor DarkYellow
}

# Le a saida de um comando como texto com quebras LF, do mesmo jeito que os arquivos de referencia
function ConvertTo-TextoLf($linhas) {
    if ($linhas.Count -gt 0) { return ($linhas -join "`n") + "`n" }
    return ""
}

$PyDir = Join-Path $ScriptDir "python"
$PyErros = [System.IO.Path]::GetTempFileName()
$PyGravado = [System.IO.Path]::GetTempFileName()
$PyCodificacaoOriginal = [Console]::OutputEncoding
$PyEncodingOriginal = $env:PYTHONIOENCODING
try {
    # No Windows, o Python escreve na codificacao local por padrao; UTF-8 mantem acentos iguais ao .saida
    [Console]::OutputEncoding = New-Object System.Text.UTF8Encoding($false)
    $env:PYTHONIOENCODING = "utf-8"
    foreach ($arquivo in (Get-ChildItem -Path $PyDir -Filter "*.js" | Sort-Object Name)) {
        $base = $arquivo.FullName.Substring(0, $arquivo.FullName.Length - 3)

        # a) O codigo impresso com --python e igual ao .py.esperado
        $Total++
        $gerado = ConvertTo-TextoLf @(& $Minijs --python $arquivo.FullName 2> $PyErros)
        $code = $LASTEXITCODE
        $esperado = "$base.py.esperado"
        if ((Test-Path $esperado) -and $code -eq 0 -and (Get-Item $PyErros).Length -eq 0) {
            $passou = $gerado -ceq [System.IO.File]::ReadAllText($esperado).Replace("`r`n", "`n")
        } else {
            $passou = $false
        }
        if ($passou) {
            $Passaram++
            Write-Host "  [PASSOU] $($arquivo.Name) (--python)" -ForegroundColor Green
        } else {
            $Falharam++
            Write-Host "  [FALHOU] $($arquivo.Name) (--python)" -ForegroundColor Red
            Get-Content $PyErros
        }

        # b) O arquivo gravado com -o e igual ao que sai com --python, e nada vai para a tela
        $Total++
        Remove-Item $PyGravado -ErrorAction SilentlyContinue
        $tela = @(& $Minijs $arquivo.FullName -o $PyGravado 2> $PyErros)
        $code = $LASTEXITCODE
        if ($code -eq 0 -and $tela.Count -eq 0 -and (Get-Item $PyErros).Length -eq 0 -and (Test-Path $PyGravado)) {
            $passou = [System.IO.File]::ReadAllText($PyGravado).Replace("`r`n", "`n") -ceq $gerado
        } else {
            $passou = $false
        }
        if ($passou) {
            $Passaram++
            Write-Host "  [PASSOU] $($arquivo.Name) (-o)" -ForegroundColor Green
        } else {
            $Falharam++
            Write-Host "  [FALHOU] $($arquivo.Name) (-o: arquivo diferente da saida de --python)" -ForegroundColor Red
        }

        # c) O .py gerado roda no Python e imprime o mesmo que o .saida
        if (-not $Python) {
            $Pulados++
            continue
        }
        $Total++
        $execucao = ConvertTo-TextoLf @(& $Python $PyGravado 2> $PyErros)
        $code = $LASTEXITCODE
        $saida = "$base.saida"
        if ((Test-Path $saida) -and $code -eq 0) {
            $passou = $execucao -ceq [System.IO.File]::ReadAllText($saida).Replace("`r`n", "`n")
        } else {
            $passou = $false
        }
        if ($passou) {
            $Passaram++
            Write-Host "  [PASSOU] $($arquivo.Name) (execucao com $Python)" -ForegroundColor Green
        } else {
            $Falharam++
            Write-Host "  [FALHOU] $($arquivo.Name) (execucao com $Python)" -ForegroundColor Red
            Get-Content $PyErros
        }
    }
} finally {
    [Console]::OutputEncoding = $PyCodificacaoOriginal
    $env:PYTHONIOENCODING = $PyEncodingOriginal
    Remove-Item $PyErros, $PyGravado -ErrorAction SilentlyContinue
}

# 8. Sumario Final
Write-Host "`n============================================================" -ForegroundColor Cyan
Write-Host "                   SUMARIO DOS TESTES                      " -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "  Total de testes executados: $Total"
Write-Host "  Testes que passaram:       $Passaram" -ForegroundColor Green
if ($Pulados -gt 0) {
    Write-Host "  Execucoes puladas:         $Pulados (Python nao encontrado)" -ForegroundColor DarkYellow
}
if ($Falharam -gt 0) {
    Write-Host "  Testes que falharam:       $Falharam" -ForegroundColor Red
    Write-Host "============================================================" -ForegroundColor Red
    exit 1
} else {
    Write-Host "  Testes que falharam:       0" -ForegroundColor Green
    Write-Host "============================================================" -ForegroundColor Green
    exit 0
}
