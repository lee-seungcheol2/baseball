#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CHGameModeBase.generated.h"


class ACHPlayerController;

UCLASS()
class CHATX_API ACHGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void BeginPlay() override;

public:
	void ResetGame();

	void JudgeGame(ACHPlayerController* InChattingPlayerController, int InStrikeCount);
	void PrintChatMessageString(ACHPlayerController* InChattingPlayerController, const FString& InChatMessageString);
	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);
	void IncreaseGuessCount(ACHPlayerController* InChattingPlayerController);
protected:
	FString SecretNumberString;

	TArray<TObjectPtr<ACHPlayerController>> AllPlayerControllers;
};
