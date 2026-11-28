let i = 0;
let soma = 0;
while (i < 5) {
    soma = soma + i;
    i = i + 1;
}

let cont = 0;
let x = 0;
while (x < 3) {
    let y = 0;
    while (y < 2) {
        cont = cont + 1;
        y = y + 1;
    }
    x = x + 1;
}

while (false) {}
while (false) ;

function potencia(base, exp) {
    let res = 1;
    let n = exp;
    while (n > 0) {
        res = res * base;
        n = n - 1;
    }
    return res;
}

console.log(i, soma, cont, potencia(2, 5), potencia(3, 0));
