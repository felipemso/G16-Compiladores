let inteiro = 10, real = 3.14, sem_decimal = 10.0;
let texto = "ola" + ' mundo';
let ligado = true, desligado = false;

let soma = inteiro + real * 2;
let divisao = inteiro / 4 - 1;
let negativo = -inteiro;
let maior = inteiro > real;
let igual = inteiro == 10;
let diferente = texto != "ola";
let ambos = ligado && desligado;
let algum = ligado || desligado;
let nenhum = !ligado;
let faixa = inteiro >= 0 && inteiro <= 100;

if (inteiro > 5 && !desligado) {
    soma = soma - 1;
} else if (texto == "") {
    soma = 0;
}
