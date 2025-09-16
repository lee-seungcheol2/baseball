#include "PlayerController/CHPlayerController.h"
#include "EngineUtils.h"
#include "ChatX.h"
#include "Gamemode/CHGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Player/CHPlayerState.h"
#include "UI/CHChatInput.h"

ACHPlayerController::ACHPlayerController()
{
	bReplicates = true;
}

void ACHPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController()==false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UCHChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}
	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void ACHPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
	
}

void ACHPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;
	//PrintChatMessageString(ChatMessageString);
	if (IsLocalController() == true)
	{
		// ServerRPCPrintChatMessageString(InChatMessageString);
		ACHPlayerState* CXPS = GetPlayerState<ACHPlayerState>();
		if (IsValid(CXPS) == true)
		{
			//FString CombinedMessageString = CXPS->PlayerNameString + TEXT(": ") + InChatMessageString;
			FString CombinedMessageString = CXPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;
			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void ACHPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	//UKismetSystemLibrary::PrintString(this, ChatMessageString, true, true, FLinearColor::Red, 5.0f);
	// FString NetModeString = ChatXFunctionLibrary::GetNetModeString(this);
	// FString CombinedMessageString = FString::Printf(TEXT("%s: %s"), *NetModeString, *InChatMessageString);
	// ChatXFunctionLibrary::MyPrintString(this, CombinedMessageString, 10.f);
	ChatXFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void ACHPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ACHPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	// for (TActorIterator<ACHPlayerController> It(GetWorld()); It; ++It)
	// {
	// 	ACHPlayerController* CXPlayerController = *It;
	// 	if (IsValid(CXPlayerController) == true)
	// 	{
	// 		CXPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
	// 	}
	// }
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		ACHGameModeBase* CXGM = Cast<ACHGameModeBase>(GM);
		if (IsValid(CXGM) == true)
		{
			CXGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}

