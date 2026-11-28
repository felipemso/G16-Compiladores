// Os 15 primeiros termos de Fibonacci com while
let anterior = 0;
let atual = 1;
let contador = 0;
while (contador < 15) {
    console.log(contador, anterior);
    let proximo = anterior + atual;
    anterior = atual;
    atual = proximo;
    contador = contador + 1;
}
console.log("termo seguinte:", anterior);
