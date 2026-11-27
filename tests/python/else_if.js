let c1 = false, c2 = false, c3 = true;
let r = 0;

if (c1) {
    r = 1;
} else if (c2) {
    ;
} else if (c3) {
    r = 3;
} else {
    r = 4;
}

if (c1) {
    r = 10;
} else {
    if (c2) {
        r = 20;
    } else {
        if (c3) {
            r = 30;
        }
    }
}
