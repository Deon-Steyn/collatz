#include <stdio.h>      // printf
#include <stdlib.h>     // strtol, exit
#include <string.h>     // strcmp
#include <iostream>     // cout
#include <locale>       // isdigit
#include <limits>       // std::numeric_limits

//----------------------------------------------------------------------------
// Forward declarations
//----------------------------------------------------------------------------

unsigned int getArguments( int argc, char** argv );

//----------------------------------------------------------------------------
// Calculate length of Collatz sequence for given number
//----------------------------------------------------------------------------

int collatz( unsigned long long n )
{
    int steps=1;
    while( n > 1 )
    {
        steps++;

        if( n % 2 )
            n = n*3 +1;
        else
            n = n/2;
    }

    return steps;
}

//----------------------------------------------------------------------------
//
// Display number with longest Collatz sequence below given starting number
//
//      Parameter: starting number (limited to maximum integer size)
//
//----------------------------------------------------------------------------

int main( int argc, char ** argv )
{
    // Validate and convert parameter (upper limit of integers to search)
    unsigned int arg_limit = getArguments( argc, argv );
    
    unsigned int numWithLongestSequence =arg_limit;
    unsigned short longestSequence =1;
    unsigned short steps=0;

    if( arg_limit < 3 )
    {
        std::cout << (arg_limit-1) << std::endl;
        exit (EXIT_SUCCESS);
    }

    for( unsigned int n=2; n < arg_limit; n++ )
    {
        steps = collatz( n );

        if( steps > longestSequence )
        {
            longestSequence = steps;
            numWithLongestSequence = n;
        }
    }


    //std::cout << "\n Number with longest Collatz sequence: " << numWithLongestSequence << std::endl << std::endl;
	std::cout << numWithLongestSequence << std::endl;

    exit (EXIT_SUCCESS);
}

//-----------------------------------------------------------------------------
// Validate and return parameter
//-----------------------------------------------------------------------------
unsigned int getArguments( int argc, char** argv )
{
    if( argc != 2 )
    {
        std::cerr << "\n Error: Please supply a single numeric parameter.\n" << std::endl;
        exit( EXIT_FAILURE );
    }

    for( int ch=0; argv[1][ch] !='\0'; ch++ )
    {
        if( !isdigit( argv[1][ch] ) )
        {
            std::cerr << "\n Error: Parameter contains non numeric characters.\n" << std::endl;
            exit( EXIT_FAILURE );
        }
    }

    unsigned long param = strtoul( argv[1], '\0', 10 );
   
    if( param == 0 )
    {
        std::cerr << "\n Error: \"" << argv[1] << "\" is not valid integer.\n" << std::endl;
        exit( EXIT_FAILURE );
    }

    if( param > std::numeric_limits<int>::max() )
    {
        std::cerr << "\n Error: Max supported integer: " << std::numeric_limits<int>::max() << std::endl << std::endl;
        exit( EXIT_FAILURE );
    }

    return( param );
}
