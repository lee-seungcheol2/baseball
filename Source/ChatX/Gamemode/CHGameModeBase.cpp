#include "CHGameModeBase.h"
#include "Game/CHGameStateBase.h"
#include "EngineUtils.h"
#include "Player/CHPlayerState.h"
#include "PlayerController/CHPlayerController.h"

void ACHGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	ACHPlayerController* CXPlayerController = Cast<ACHPlayerController>(NewPlayer);
	if (IsValid(CXPlayerController) == true)
	{
		CXPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
	
		// ACHGameStateBase* CXGameStateBase =  GetGameState<ACHGameStateBase>();
		// if (IsValid(CXGameStateBase) == true)
		// {
		// 	CXGameStateBase->MulticastRPCBroadcastLoginMessage(TEXT("XXXXXXX"));
		// }
		//
		// ACHPlayerController* CXPlayerController = Cast<ACHPlayerController>(NewPlayer);
		// if (IsValid(CXPlayerController) == true)
		// {
		 //	AllPlayerControllers.Add(CXPlayerController);
		// }
		//ACHPlayerController* CXPlayerController = Cast<ACHPlayerController>(NewPlayer);
		//if (IsValid(CXPlayerController) == true)
		
			AllPlayerControllers.Add(CXPlayerController);

			ACHPlayerState* CXPS = CXPlayerController->GetPlayerState<ACHPlayerState>();
			if (IsValid(CXPS) == true)
			{
				CXPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
			}

			ACHGameStateBase* CXGameStateBase =  GetGameState<ACHGameStateBase>();
			if (IsValid(CXGameStateBase) == true)
			{
				CXGameStateBase->MulticastRPCBroadcastLoginMessage(CXPS->PlayerNameString);
			}
		}
	
}

void ACHGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	SecretNumberString = GenerateSecretNumber();
}

void ACHGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& CXPlayerController : AllPlayerControllers)
	{
		ACHPlayerState* CXPS = CXPlayerController->GetPlayerState<ACHPlayerState>();
		if (IsValid(CXPS) == true)
		{
			CXPS->CurrentGuessCount = 0;
		}
	}
}

void ACHGameModeBase::JudgeGame(ACHPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		ACHPlayerState* CXPS = InChattingPlayerController->GetPlayerState<ACHPlayerState>();
		for (const auto& CXPlayerController : AllPlayerControllers)
		{
			if (IsValid(CXPS) == true)
			{
				FString CombinedMessageString = CXPS->PlayerNameString + TEXT(" has won the game.");
				CXPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& CXPlayerController : AllPlayerControllers)
		{
			ACHPlayerState* CXPS = CXPlayerController->GetPlayerState<ACHPlayerState>();
			if (IsValid(CXPS) == true)
			{
				if (CXPS->CurrentGuessCount < CXPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& CXPlayerController : AllPlayerControllers)
			{
				CXPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));

				ResetGame();
			}
		}
	}
}

void ACHGameModeBase::PrintChatMessageString(ACHPlayerController* InChattingPlayerController,
                                             const FString& InChatMessageString)
{
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);
		IncreaseGuessCount(InChattingPlayerController);
		for (TActorIterator<ACHPlayerController> It(GetWorld()); It; ++It)
		{
			ACHPlayerController* CXPlayerController = *It;
			if (IsValid(CXPlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
				CXPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);

				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(InChattingPlayerController, StrikeCount);
			}
		}
	}
	else
	{
		for (TActorIterator<ACHPlayerController> It(GetWorld()); It; ++It)
		{
			ACHPlayerController* CXPlayerController = *It;
			if (IsValid(CXPlayerController) == true)
			{
				CXPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

FString ACHGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });
	
	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ACHGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}
			
			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;
		
	} while (false);	

	return bCanPlay;
}

FString ACHGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else 
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;				
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
	
}

void ACHGameModeBase::IncreaseGuessCount(ACHPlayerController* InChattingPlayerController)
{
	ACHPlayerState* CXPS = InChattingPlayerController->GetPlayerState<ACHPlayerState>();
	if (IsValid(CXPS) == true)
	{
		CXPS->CurrentGuessCount++;
	}
}
