// Soma de 1 a N com for, conferida pela formula de Gauss
function soma_ate(n) {
    let soma = 0;
    for (let i = 1; i <= n; i = i + 1) {
        soma = soma + i;
    }
    return soma;
}

function gauss(n) {
    return n * (n + 1) / 2;
}

const N = 100;
let resultado = soma_ate(N);
console.log("soma de 1 a", N, "=", resultado);
console.log("formula de Gauss:", gauss(N));
console.log("iguais:", resultado == gauss(N));
console.log("soma de 1 a 7 =", soma_ate(7), "e media", soma_ate(7) / 7);
