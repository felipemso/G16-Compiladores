// Fatorial recursivo de 0 a 10
function fatorial(n) {
    if (n <= 1) {
        return 1;
    }
    return n * fatorial(n - 1);
}

for (let i = 0; i <= 10; i = i + 1) {
    console.log("fatorial de", i, "=", fatorial(i));
}
