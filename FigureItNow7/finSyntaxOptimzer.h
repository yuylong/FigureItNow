#ifndef FINSYNTAXOPTIMZER_H
#define FINSYNTAXOPTIMZER_H

#include "finErrorCode.h"
#include "finLexNode.h"
#include "finSyntaxTree.h"

class finSyntaxOptimzer
{


public:
    finSyntaxOptimzer();

    finErrorCode checkAndOptimize(finSyntaxTree *syntree);

private:
    finErrorCode optimizeSyntaxNode(finSyntaxNode *synnode);
};

#endif // FINSYNTAXOPTIMZER_H
