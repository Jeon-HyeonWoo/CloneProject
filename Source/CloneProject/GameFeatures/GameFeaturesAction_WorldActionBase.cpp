
#include "GameFeaturesAction_WorldActionBase.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeaturesAction_WorldActionBase)

void UGameFeaturesAction_WorldActionBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	//World�� Loop
	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		//�ռ�, ExperienceManagercomponent���� GameFeatureAction�� Ȱ��ȭ �ϸ鼭, Context�� World�� �־���
		// �̸� ���� ������ ������� �Ǵ�
		if (Context.ShouldApplyToWorldContext(WorldContext))
		{
			//WorldActionBase�� Interface�� AddToWorldȣ��
			AddToWorld(WorldContext, Context);
		}
	}
}
