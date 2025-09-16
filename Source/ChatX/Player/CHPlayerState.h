// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CHPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CHATX_API ACHPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ACHPlayerState();
	FString GetPlayerInfoString();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
public:
	UPROPERTY(Replicated)
	FString PlayerNameString;
	UPROPERTY(Replicated)
	int32 CurrentGuessCount;
	UPROPERTY(Replicated)
	int32 MaxGuessCount;
};
