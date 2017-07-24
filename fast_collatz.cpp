#include <stdio.h>      // printf
#include <stdlib.h>     // strtol, exit
#include <string.h>     // strcmp
#include <iostream>     // cout
#include <locale>       // isdigit
#include <limits>       // std::numeric_limits
#include <pthread.h>
#include <unistd.h>     // sysconf(_SC_NPROCESSORS_ONLN);


//----------------------------------------------------------------------------
// Forward declarations
//----------------------------------------------------------------------------

unsigned int getArguments( int argc, char** argv );
unsigned int allocateArrayMemory( unsigned int size, unsigned short ** array );
void *calcCollatz( void *arg );

// create thread argument struct for thr_func() */
typedef struct thread_args
{
    unsigned int lowerLimit;
    unsigned int upperLimit;
} thread_args_t;

//----------------------------------------------------------------------------
// Golbal variables, access by thread
//----------------------------------------------------------------------------

// Array of previous calculations (only odd numbers)
unsigned short * lookup;
unsigned int maxLookupNumber;

//----------------------------------------------------------------------------
// Golbal variables, access by thread
//----------------------------------------------------------------------------

unsigned int shared_longestSequence;
unsigned int shared_numWithLongestSequence;
pthread_mutex_t lock_x;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;


//----------------------------------------------------------------------------
//
// Display number with longest Collatz sequence below given starting number
//
//      Parameter: starting number (limited to maximum integer size)
//
//      Optimizations:
//          1) Stored previously calculated numbers (+90% faster)
//          2) Store only odd numbers (half memory, +15% faster)
//          3) Apply two steps to odd numbers, since 1st (n3+1) is always even
//          4) Multi-thread (overide system with env var NUM_THREADS)
//
//----------------------------------------------------------------------------

int main( int argc, char ** argv )
{
    // Get arguments
    unsigned int arg_upper_limit = getArguments( argc, argv );

#ifdef DEBUG
    std::cout << "\nGiven " << arg_upper_limit << std::endl << std::endl;
#endif

    if( arg_upper_limit < 3 )
    {
        std::cout << (arg_upper_limit-1) << std::endl;
        exit (EXIT_SUCCESS);
    }

    // Allocate memory
    maxLookupNumber = 2 * allocateArrayMemory( arg_upper_limit, &lookup );

    // Determine num threads (specified by system config / overide: env var NUM_THREADS
    int NUM_THREADS = sysconf( _SC_NPROCESSORS_ONLN );
    if( const char* env_p = getenv( "NUM_THREADS" ) )
        NUM_THREADS = strtol( env_p, '\0', 10 );

    // Sanity check number of threads
    if( NUM_THREADS < 1 ||
        NUM_THREADS > arg_upper_limit )
        NUM_THREADS = 1;

    //-------------------------------------------------------------------------
    // Create thread work distribution (number range)
    //  NOTE: excludes user upper limit itself (as per requirements)
    //-------------------------------------------------------------------------

    thread_args_t thread_arg[NUM_THREADS];
    unsigned int threadWorkSize = arg_upper_limit / NUM_THREADS;
    for( int i = 0; i < NUM_THREADS; i++ )
    {
        thread_arg[i].lowerLimit = i * threadWorkSize;
        thread_arg[i].upperLimit = threadWorkSize + thread_arg[i].lowerLimit - 1;
    }

    // Correct last thread's upper limit to account for rounding 
    thread_arg[NUM_THREADS-1].upperLimit = arg_upper_limit -1;

#ifdef DEBUG
    for( int i = 0; i < NUM_THREADS; i++ )
        std::cout << "\nThread [" << i << "]" << thread_arg[i].lowerLimit << "-" << thread_arg[i].upperLimit << std::endl;
#endif
    //-------------------------------------------------------------------------
    // Run threads, block until complete
    //-------------------------------------------------------------------------

    pthread_t thr[NUM_THREADS];
    for( int i = 0; i < NUM_THREADS; i++ )
    {
        int retcode = pthread_create( &thr[i], NULL, calcCollatz, &thread_arg[i] );
        if( retcode )
        {
            std::cerr << "\n Error starting thread, error code: %d" << retcode << std::endl;
            exit( EXIT_FAILURE );
        }
    }

    // Block until all threads complete
    for( int i = 0; i < NUM_THREADS; i++ )
    {
        pthread_join( thr[i], NULL );
    }

    //std::cout << "\n Number with longest Collatz sequence: " << shared_numWithLongestSequence << std::endl << std::endl;
    std::cout << shared_numWithLongestSequence << std::endl;

    free( lookup );
	
    exit( EXIT_SUCCESS );
}

//-----------------------------------------------------------------------------
// Array only stores odd numbers
//   - this halves memory requirements without impacting performance
//   - marginal performance improvement (malloc time)
//-----------------------------------------------------------------------------
unsigned int allocateArrayMemory(
                    const unsigned int arg_size,
                    unsigned short **  arg_array )
{
    // not using 0 element so add 1 to size.
    unsigned int allocatedSize = arg_size + 1;

    // prevent truncation on odd numbers
    if( allocatedSize % 2 )           
        ++allocatedSize;

    // Starts with ideal size and halfs until able to allocate
    //  - calloc is faster than new
    do
    {
        allocatedSize /= 2;
        *arg_array =(unsigned short*) calloc( sizeof(unsigned short), allocatedSize );
    }
    while( !*arg_array );

    if( !*arg_array )
    {
        std::cerr << "\n Unable to allocate memory." << std::endl;
        exit( EXIT_FAILURE );
    }

    return( allocatedSize );
}

//-----------------------------------------------------------------------------
// Thread calculates number with longest Collatz sequence from upper to lower limit
//-----------------------------------------------------------------------------
void *calcCollatz( void *arg )
{
    thread_args_t *threadArgs = (thread_args_t *)arg;

    unsigned int longestSequence = 1;
    unsigned int numWithLongestSequence = 1;
    unsigned int        idx;
    unsigned short      steps=0;
    unsigned long long  n;       // intermediary value can exceed starting number

    // Parameter (upperLimit) is excluded as per specification
    unsigned int checkNumber = threadArgs->lowerLimit;
    while( checkNumber <= threadArgs->upperLimit )
    {
#ifdef DEBUG
    std::cout << "\n[" << checkNumber << "]";
#endif

        n = checkNumber;
        steps = 1;

        while( n > 1 )
        {
#ifdef DEBUG
    std::cout << " " << n;
#endif
            if( n % 2 )     // Odd number
            {
                // if( n < checkNumber && n < maxLookupNumber )
                // {
                //     idx = (n+1) / 2;    // array only stores odd numbers, e.g. 7 stored at pos 4
                //     if( lookup[idx] != 0 )
                //     {
                //         steps += lookup[idx];
                //         break;
                //     }
                // }
                // Optimization: (n*3 +1) always even number, so immediately apply n/2
                // n = (n*3 +1) / 2;
                // steps +=2;
                n = (n*3 +1);
                steps ++;

            }
            else    // Even number
            {
                n /= 2;
                steps++;
            }
        }
#ifdef DEBUG
    std::cout << " (" << steps << ")";
#endif
        // Only store uneven numbers in array
        if( checkNumber % 2 )
            lookup[ (checkNumber+1) / 2 ] = steps;

        if( steps > longestSequence )
        {
            longestSequence = steps;
            numWithLongestSequence = checkNumber;
        }

        checkNumber++;
    }

    // Update shared max values
    pthread_mutex_lock( &mutex1 );

    if( shared_longestSequence < longestSequence )
    {
        shared_longestSequence        = longestSequence;
        shared_numWithLongestSequence = numWithLongestSequence;
    }

    pthread_mutex_unlock( &mutex1 );

    pthread_exit(NULL);
}


//-----------------------------------------------------------------------------
// Validate and return parameter
//-----------------------------------------------------------------------------
unsigned int getArguments( int argc, char** argv )
{
    if( argc != 2 )
    {
        std::cout << "\n Error: Please supply a single numeric parameter.\n" << std::endl;
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
        std::cerr << "\n Error: \"" << argv[1] << "\"is not valid integer.\n" << std::endl;
        exit( EXIT_FAILURE );
    }

    if( param > std::numeric_limits<int>::max() )
    {
        std::cerr << "\n Error: Max supported integer: " << std::numeric_limits<int>::max() << std::endl << std::endl;
        exit( EXIT_FAILURE );
    }

    return( param );
}
