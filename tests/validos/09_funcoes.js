// Declaracao, retorno e chamada de funcoes (T07)
function soma(a, b) {
    return a + b;
}

function dobro(x) {
    return soma(x, x);
}

function saudacao() {
    console.log("ola");
    return;
}

let total = soma(2, 3) * 2;
let ok = dobro(total) > 10 && soma(1, 1) == 2;
saudacao();
console.log("total:", total, ok);
