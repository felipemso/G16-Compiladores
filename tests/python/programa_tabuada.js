// Tabuada do 1 ao 5 com for aninhado
for (let i = 1; i <= 5; i = i + 1) {
    console.log("tabuada do", i);
    for (let j = 1; j <= 10; j = j + 1) {
        console.log(" ", i, "x", j, "=", i * j);
    }
}
