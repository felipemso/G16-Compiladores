// Erro sintatico na linha 2: expressao ausente apos '='
// O parser deve reportar o erro, sincronizar no ';' e continuar processando
// os comandos validos das linhas seguintes.
let a = 1;
let b = ;
let c = 2;
console.log(c);
