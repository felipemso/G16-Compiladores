// Dois for seguidos com let i: cada i fica no escopo do proprio for
let soma = 0;
for (let i = 0; i < 3; i = i + 1) {
    soma = soma + i;
}
for (let i = 0; i < 3; i = i + 1) {
    soma = soma - i;
}
