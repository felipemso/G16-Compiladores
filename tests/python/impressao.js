let inteiro = 42, negativo = -7, real = 3.5, divisao = 10 / 4, exata = 9 / 3;
let texto = "ola", vazio = '';
let ligado = true, desligado = false;
let indefinido;

function nada() {
    return;
}

console.log(inteiro, negativo, real, divisao, exata);
console.log(texto, vazio, texto + " mundo");
console.log(ligado, desligado, inteiro > real, !ligado);
console.log(indefinido, nada());
console.log(0.1 + 0.2, 1000000 * 1000000, 2.5 * 2);
console.log();
console.log("total:", inteiro + real, "ok:", ligado && inteiro == 42);
