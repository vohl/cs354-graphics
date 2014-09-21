// This is a test program that demonstrates the use of Vec2.h and Vec3.h

#include "Vec2.h"
#include "Vec3.h"

using std::cout;  // This allows us to use "cout" rather than "std::cout".
using std::endl;  // This allows us to use "endl" rather than "std::endl".


// Here is how to pass a 3D vector to a function in such a way that it cannot
// be modified by the function.

void Foo( const Vec3 &A )
{
    cout << "In Foo, argument = " << A << endl;
}


// Here is how to pass a 3D vector to a function in such a way that it CAN
// be modified by the function.

void Baz( Vec3 &A )
{
    A.z = 0.0;  // Just zero out the last component.
}




int main()
{
    cout << "Testing Vec2..." << endl;
    
    Vec2 X, Y(1,2), Z;
    
    // X and Z can be initialized using operations on other vectors, including
    // 2D vectors that are defined in-line using the Vec2 constructior...
    X = Vec2( -2, 6 );
    Z = 2.0 * Y + 3.0 * X - Vec2( 5, 2.2 );
    
    cout << "Z = " << Z << endl;
    
    // The dot product and even a form of "cross product" is defined for Vec2...
    
    cout << "X dot Y = " << X * Y << endl;
    cout << "Y cross Z = " << (Y ^ Z) << endl;  // This is a SCALAR for 2D vectors!
    cout << "Testing Vec3..." << endl;
    
    Vec3 A, B, C( 1, 2, 3 );
    
    // Initialize 3D vectors A and B using various vector operations, as we did above
    // for 2D vectors.
    
    A = 2.0 * C;
    B = A + Vec3( -2, 4, -1 );
    
    cout << "A = " << A << endl;
    cout << "B = " << B << endl;
    
    // Demonstrate dot and cross products.  Verify that the cross product of two vectors
    // is orthogonal to the original two vectors.
    
    double dot = A * B;
    Vec3 W = A ^ B;   // This is the cross product.
    
    cout << "A dot B = " << dot << endl;
    cout << "A cross B = " << W << endl;
    cout << "A dot W = " << (A * W) << endl;
    cout << "B dot W = " << (B * W) << endl;
    
    // Here is how we access individual components of vectors...
    
    cout << "A.x = " << A.x << endl;
    A.x = 2.0 * A.x - 6.0 * A.y + 100;
    cout << "After changing A.x, A = " << A << endl;
    
    // Calling functions with vectors as arguments...
    
    Foo( A );
    Baz( A );
    
    cout << "After calling Baz with A, A = " << A << endl;
    
    // This is how to normalize a vector...
    
    A = Unit(A);
    cout << "A after normalization = " << A << endl;
    cout << "The length of A is now " << Length(A) << endl;
    
    cout << "done." << endl;
    return 0;
}

