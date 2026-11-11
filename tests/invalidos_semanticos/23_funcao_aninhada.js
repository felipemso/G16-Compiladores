// Funcao declarada dentro de outra funcao (sem funcoes aninhadas)
function externa() {
    function interna() {
        return 1;
    }
    return 2;
}
