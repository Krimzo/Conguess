#include "conguess.h"


int main( int argc, char** argv )
{
    Conguess conguess{};
    while ( conguess.update() );
    return 0;
}
