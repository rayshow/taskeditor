
#include"Expression/TaskSystemExpressionComment.h"


UTaskSystemExpressionComment::UTaskSystemExpressionComment(FObjectInitializer const& Initializer)
	:UTaskSystemExpression(Initializer) {}

void UTaskSystemExpressionComment::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}