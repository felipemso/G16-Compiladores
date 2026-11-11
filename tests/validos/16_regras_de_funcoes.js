// Recursao, chamadas na ordem certa e parametro com nome de global alterado dentro da funcao
function fatorial(n) {
    if (n <= 1) {
        return 1;
    }
    return n * fatorial(n - 1);
}

function dobro(x) {
    return x * 2;
}

function quadruplo(x) {
    return dobro(dobro(x));
}

let valor = 3;
function incrementa(valor) {
    valor = valor + 1;
    return valor;
}

let r = fatorial(5) + quadruplo(valor) + incrementa(valor);
console.log(r);
