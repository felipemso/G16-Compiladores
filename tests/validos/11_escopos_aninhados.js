// Escopos aninhados validos: blocos irmaos reaproveitam nomes e blocos internos enxergam os externos
let total = 0;
const passo = 2;
{
    let parcial = total + passo;
    {
        let dobro = parcial * 2;
        total = total + dobro;
    }
    {
        let dobro = parcial * 3;
        total = total + dobro;
    }
}
{
    let parcial = 1;
    total = total + parcial;
}
var resultado = total;
