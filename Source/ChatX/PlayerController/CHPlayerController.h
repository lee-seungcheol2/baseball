#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CHPlayerController.generated.h"

class UCHChatInput;
class UUserWidget;


UCLASS()
class CHATX_API ACHPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACHPlayerController();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	void SetChatMessageString(const FString& InChatMessageString);
	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);
	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCHChatInput> ChatInputWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UCHChatInput> ChatInputWidgetInstance;

	FString ChatMessageString;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;
};
