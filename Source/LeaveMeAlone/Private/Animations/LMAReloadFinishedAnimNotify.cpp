// LeaveMeAlone Game by Netologya. All rights Reserved


#include "Animations/LMAReloadFinishedAnimNotify.h"

void ULMAReloadFinishedAnimNotify::Notify(USkeletalMeshComponent*MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	OnNotifyReloadFinished.Broadcast(MeshComp);

}