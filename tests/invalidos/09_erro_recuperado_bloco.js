// Erro sintatico dentro do bloco: falta ';' apos a declaracao,
// forcando a sincronizacao no nivel de bloco via 'error RBRACE'.
let x = 1;
{
    let y = 5
}
console.log(x);
