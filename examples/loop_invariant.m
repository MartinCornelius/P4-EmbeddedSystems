/* Example program for the loop invariant optimization */

setup()
{
    int8 n = 37; // Arbitrary value
}

mainloop()
{
    uint32 i = 1; // For outer loop

    while (i <= 10)
    {
        uint16 L = i * (n + 2);
        uint32 j = 1; // For inner loop
        while (j <= 10)
        { 
            uint16 R = L * 10 * j; // Interesting calculation
            j = j + 1; // Increment inner loop
        }
        i = i + 1; // Increment outer loop
    }
}