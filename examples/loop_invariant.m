/* Example program for the loop invariant optimization */

setup()
{
    int16 n = 37; // Arbitrary value
}

mainloop()
{
    uint16 i = 1; // For outer loop

    while (i <= 10)
    {
        uint16 L = i * (n + 2);
        uint16 j = 1; // For inner loop
        while (j <= 10)
        { 
            uint16 t = L * 10;
            uint16 R = L * 10 * j; // Interesting calculation
            j = j + 1; // Increment inner loop
            print(R);
        }
        i = i + 1; // Increment outer loop
    }
}