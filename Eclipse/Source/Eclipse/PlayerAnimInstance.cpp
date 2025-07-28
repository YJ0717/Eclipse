#include "PlayerAnimInstance.h"
#include "PlayerCharacter.h"
#include "GameFramework/Character.h"

void UPlayerAnimInstance::AnimNotify_DetachWeapon()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	if (PlayerCharacter)
	{
		PlayerCharacter->GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	}
}