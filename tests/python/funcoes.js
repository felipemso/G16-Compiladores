let n = 5;

function fatorial(n) {
    if (n <= 1) {
        return 1;
    }
    return n * fatorial(n - 1);
}

function quadrado(x) {
    return x * x;
}

function soma_quadrados(a, b) {
    return quadrado(a) + quadrado(b);
}

function maior(a, b) {
    if (a > b) {
        return a;
    }
    return b;
}

function vazia() {
}

function sem_valor(x) {
    if (x) {
        return;
    }
    x = !x;
}

let f = fatorial(n);
let s = soma_quadrados(n, 2) - maior(f, 3);
vazia();
sem_valor(false);
console.log(n, f, s, fatorial(0), maior(quadrado(3), fatorial(3)));
