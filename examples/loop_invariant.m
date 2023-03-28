/* Example program for the loop invariant optimization */

setup()
{
    int8 n = 37; // Arbitrary value
}

mainloop()
{
    int8 i = 1; // For outer loop

    while (i <= 10)
    {
        int8 L = i * (n + 2);
        int8 j = 1; // For inner loop
        while (j <= 10)
        { 
            int8 R = L * 10 * j; // Interesting calculation
            j = j + 1; // Increment inner loop
        }
        i = i + 1; // Increment outer loop
    }
}