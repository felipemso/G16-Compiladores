// Sombreamento proibido: variavel local com o nome de uma global (so parametro pode)
let x = 1;
function f() {
    let x = 2;
    return x;
}
