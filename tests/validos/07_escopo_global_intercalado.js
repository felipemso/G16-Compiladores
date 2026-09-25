// Blocos intercalados com comandos no escopo global
let a = 1;
{ { let b = 2; } }
console.log(a);
{
    a = a + 1;
    { ; }
}
;
let c = a;
