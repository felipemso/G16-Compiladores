#!/usr/bin/env bash
# ==============================================================================
# Script Runner de Testes Automatizados (Bash) - Compilador Mini-JS
# ==============================================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BASE_DIR="$(dirname "$SCRIPT_DIR")"

# Localiza o binario do compilador
MINIJS=""
for candidate in "$BASE_DIR/minijs" "$BASE_DIR/minijs.exe" "./minijs" "./minijs.exe"; do
    if [ -x "$candidate" ] || [ -f "$candidate" ]; then
        MINIJS="$candidate"
        break
    fi
done

if [ -z "$MINIJS" ]; then
    echo -e "\033[0;31m[ERRO] Executavel 'minijs' nao foi encontrado. Execute 'make' antes de rodar os testes.\033[0m"
    exit 1
fi

# Localiza o runner de testes unitarios em memoria (se compilado)
TEST_UNIT=""
for candidate in "$BASE_DIR/test_unit" "$BASE_DIR/test_unit.exe" "./test_unit" "./test_unit.exe"; do
    if [ -x "$candidate" ] || [ -f "$candidate" ]; then
        TEST_UNIT="$candidate"
        break
    fi
done

TOTAL=0
PASSARAM=0
FALHARAM=0

echo -e "\033[0;36m============================================================\033[0m"
echo -e "\033[0;36m  INICIANDO SUITE DE TESTES AUTOMATIZADOS - MINI-JS        \033[0m"
echo -e "\033[0;36m============================================================\033[0m"
echo "Executavel do compilador: $MINIJS"
echo ""

# 1. Execucao de Testes Unitarios em Memoria
if [ -n "$TEST_UNIT" ]; then
    "$TEST_UNIT"
    if [ $? -eq 0 ]; then
        TOTAL=$((TOTAL + 1))
        PASSARAM=$((PASSARAM + 1))
    else
        TOTAL=$((TOTAL + 1))
        FALHARAM=$((FALHARAM + 1))
    fi
    echo ""
fi

# Funcao auxiliar parametrizada para iteracao sobre suites
run_suite() {
    local dir="$1"
    local title="$2"
    local expect_zero="$3"

    echo -e "\033[0;33m--- $title ---\033[0m"
    if [ ! -d "$dir" ]; then
        return
    fi

    for arquivo in "$dir"/*.js; do
        [ -e "$arquivo" ] || continue
        TOTAL=$((TOTAL + 1))
        local nome
        nome=$(basename "$arquivo")
        "$MINIJS" "$arquivo" > /dev/null 2>&1
        local code=$?

        if [ "$expect_zero" -eq 1 ]; then
            if [ $code -eq 0 ]; then
                PASSARAM=$((PASSARAM + 1))
                echo -e "  \033[0;32m[PASSOU]\033[0m $nome"
            else
                FALHARAM=$((FALHARAM + 1))
                echo -e "  \033[0;31m[FALHOU]\033[0m $nome (esperava codigo 0, obteve $code)"
            fi
        else
            if [ $code -ne 0 ]; then
                PASSARAM=$((PASSARAM + 1))
                echo -e "  \033[0;32m[PASSOU]\033[0m $nome (falha sintatica esperada, codigo: $code)"
            else
                FALHARAM=$((FALHARAM + 1))
                echo -e "  \033[0;31m[FALHOU]\033[0m $nome (esperava codigo != 0, mas retornou 0)"
            fi
        fi
    done
}

# 2. Casos de Teste Validos
run_suite "$SCRIPT_DIR/validos" "Executando Casos de Teste Validos" 1

# 3. Casos de Teste Invalidos
echo ""
run_suite "$SCRIPT_DIR/invalidos" "Executando Casos de Teste Invalidos (Falha Esperada)" 0

# 4. Sumario Final
echo ""
echo -e "\033[0;36m============================================================\033[0m"
echo -e "\033[0;36m                   SUMARIO DOS TESTES                      \033[0m"
echo -e "\033[0;36m============================================================\033[0m"
echo "  Total de testes executados: $TOTAL"
echo -e "  Testes que passaram:       \033[0;32m$PASSARAM\033[0m"
if [ $FALHARAM -gt 0 ]; then
    echo -e "  Testes que falharam:       \033[0;31m$FALHARAM\033[0m"
    echo -e "\033[0;31m============================================================\033[0m"
    exit 1
else
    echo -e "  Testes que falharam:       \033[0;32m0\033[0m"
    echo -e "\033[0;32m============================================================\033[0m"
    exit 0
fi
