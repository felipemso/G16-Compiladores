// Parametros, retorno de funcao e variaveis reatribuidas com outro tipo sao desconhecidos e nao geram erro
function soma(x, y) {
    return x + y;
}
let r = soma(1, 2);
let texto = r + "!";

let v = 1;
v = "um";
let w = v * 2;

// Dentro da funcao, contador (let de fora) e desconhecido: concatenar com string nao gera erro
let contador = 0;
function descreve() {
    return "valor: " + contador;
}
