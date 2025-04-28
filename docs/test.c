sv sub(gf o, gf a, gf b)
{
    int i;
    for (i = 0; i < 16; i++)
        o[i] = a[i] - b[i];  // Element-wise subtraction
}