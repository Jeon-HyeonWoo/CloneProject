
#include "GameFeaturesAction_WorldActionBase.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeaturesAction_WorldActionBase)

void UGameFeaturesAction_WorldActionBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	//World를 Loop
	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		//앞서, ExperienceManagercomponent에서 GameFeatureAction을 활성화 하면서, Context에 World를 넣어줌
		// 이를 통해 적용할 대상인지 판단
		if (Context.ShouldApplyToWorldContext(WorldContext))
		{
			//WorldActionBase의 Interface인 AddToWorld호출
			AddToWorld(WorldContext, Context);
		}
	}
}
