// Contagem com variaveis de varios tipos e console.log de cada um
const nome = "Mini-JS";
let versao = 1.5;
let ativo = true;
let indefinido;
console.log("compilador:", nome, "versao", versao, "ativo:", ativo, "valor:", indefinido);

// Pares e impares de 1 a 20, alternando um booleano (o % esta fora do escopo)
let pares = 0;
let impares = 0;
let eh_impar = true;
for (let i = 1; i <= 20; i = i + 1) {
    if (eh_impar) {
        impares = impares + 1;
    } else {
        pares = pares + 1;
    }
    eh_impar = !eh_impar;
}
console.log("pares:", pares, "impares:", impares, "total:", pares + impares);

// Divisoes exatas e com casa decimal, e um numero negativo
console.log(10 / 2, 7 / 2, 1 / 4, -3 * 2);
console.log("vazio:", "", "fim");
console.log(pares > impares, pares == impares, !ativo);
console.log();
