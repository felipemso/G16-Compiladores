// Negacao simples, encadeada e combinada com outros operadores
let a = !true;
let b = !!false;
let c = !(1 < 2);
let d = !a && b || !c;
let e = !false == true;
let f = 1 != 2;
console.log(a, b, c, d, e, f);
