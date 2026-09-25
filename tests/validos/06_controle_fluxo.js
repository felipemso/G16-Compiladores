// Estruturas condicionais e lacos de repeticao
let limite = 10;
let total = 0;

if (limite > 0)
    if (total == 0)
        total = total + 1;
    else
        total = total - 1;

if (total > 0) {
    console.log(total);
} else {
    console.log("zero");
}

while (total < limite) {
    total = total + 1;
}

for (let i = 0; i < 10; i = i + 1) {
    total = total + i;
}

for (; total < 100; ) {
    total = total + 10;
}
