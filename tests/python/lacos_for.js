let soma = 0;
for (let i = 1; i <= 5; i = i + 1) {
    soma = soma + i;
}

let pares = 0;
for (let a = 0, b = 6; a < b; a = a + 1) {
    pares = pares + 1;
}

let k = 0;
for (; k < 4; k = k + 1) ;

let passos = 0;
for (let m = 0; m < 3;) {
    passos = passos + 2;
    m = m + 1;
}

for (; false;) {}

function soma_grade(linhas, colunas) {
    let total = 0;
    for (let lin = 0; lin < linhas; lin = lin + 1) {
        for (let col = 0; col < colunas; col = col + 1) {
            total = total + 1;
        }
    }
    return total;
}

function conta_ate(alvo) {
    for (let p = 0;; p = p + 1) {
        if (p == alvo) {
            return p;
        }
    }
}

console.log(soma, pares, k, passos, soma_grade(3, 4), conta_ate(5));
