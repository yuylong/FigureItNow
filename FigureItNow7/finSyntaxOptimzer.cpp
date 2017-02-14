#include "finSyntaxOptimzer.h"

finSyntaxOptimzer::finSyntaxOptimzer()
{

}

finErrorCode finSyntaxOptimzer::checkAndOptimize(finSyntaxTree *syntree)
{
    if ( syntree == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

