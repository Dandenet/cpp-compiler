// single line comment

/* Multy
 * line
 * comment
*/

class MyClass {
    int x = 12345;
    double y = 0.1e+12;

    void Fun()
    {
       x = x + y;

       while (x >= y || y <= x && x > y && y < x)
       {
           x = x - y;
           y = y / x;
           y = y % x;
           y = y == x;
       }
    }
};

void main()
{
    MyClass a;

    a.fun();
}

/*
* Cooment
*/
