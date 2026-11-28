// Maior de tres numeros com if/else encadeado
function maior(a, b, c) {
    if (a >= b && a >= c) {
        return a;
    } else if (b >= c) {
        return b;
    } else {
        return c;
    }
}

function descreve(a, b, c) {
    let m = maior(a, b, c);
    if (m == a && m == b && m == c) {
        console.log(a, b, c, "-> todos iguais a", m);
    } else {
        console.log(a, b, c, "-> maior:", m);
    }
}

descreve(3, 7, 5);
descreve(9, 2, 4);
descreve(1, 6, 8);
descreve(-1, -5, -3);
descreve(2.5, 2.25, 2);
descreve(4, 4, 4);
