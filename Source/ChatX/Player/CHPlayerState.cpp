#include "Player/CHPlayerState.h"
#include "Net/UnrealNetwork.h"


ACHPlayerState::ACHPlayerState()
: PlayerNameString(TEXT("None"))
, CurrentGuessCount(0)
, MaxGuessCount(3)
{
	bReplicates = true;
}

FString ACHPlayerState::GetPlayerInfoString()
{
	FString PlayerInfoString = PlayerNameString + TEXT("(") + FString::FromInt(CurrentGuessCount) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT(")");
	return PlayerInfoString;
}

void ACHPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);
}
