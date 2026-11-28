// MDC por subtracoes sucessivas (algoritmo de Euclides sem %)
function mdc(a, b) {
    while (a != b) {
        if (a > b) {
            a = a - b;
        } else {
            b = b - a;
        }
    }
    return a;
}

function mmc(a, b) {
    return a * b / mdc(a, b);
}

console.log("mdc(48, 18) =", mdc(48, 18));
console.log("mdc(17, 5) =", mdc(17, 5));
console.log("mdc(100, 75) =", mdc(100, 75));
console.log("mmc(4, 6) =", mmc(4, 6));
console.log("mmc(21, 6) =", mmc(21, 6));
