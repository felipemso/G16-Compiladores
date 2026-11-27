let a = true, b = false;
let r = 0;

if (a) {
    let local = 1;
    if (b) {
        r = local;
    } else {
        r = 2;
    }
} else {
    if (b) {
        r = 3;
    }
    r = 4;
}

if (a)
    if (b)
        r = 10;
    else
        r = 20;

if (b) {}
if (a) ; else { ; }
