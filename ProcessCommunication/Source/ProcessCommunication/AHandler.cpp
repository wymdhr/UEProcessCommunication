// Fill out your copyright notice in the Description page of Project Settings.


#include "AHandler.h"
#include "Framework/Application/SlateApplication.h"


// Sets default values
AAHandler::AAHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAHandler::BeginPlay()
{
	Super::BeginPlay();
	if (FWindowsApplication* Application = (FWindowsApplication*)FSlateApplication::Get().GetPlatformApplication().Get())
	{
		Application->AddMessageHandler(Handler);
	}
}

void AAHandler::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (FWindowsApplication* Application = (FWindowsApplication*)FSlateApplication::Get().GetPlatformApplication().Get())
	{
		Application->RemoveMessageHandler(Handler);
	}
}

// Called every frame
void AAHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool FExampleHandler::ProcessMessage(HWND Hwnd, uint32 Message, WPARAM WParam, LPARAM LParam, int32& OutResult)
{
	if (Message == WM_COPYDATA)
	{
		COPYDATASTRUCT* CopyData = (COPYDATASTRUCT*)LParam;
		if (CopyData->dwData == (WM_USER + 100))//Sender发来的消息
		{
			FString CustomMessage = (char*)CopyData->lpData;
			GEngine->AddOnScreenDebugMessage((uint64)-1, 2.0f, FColor::Emerald, FString::Printf(TEXT("Message = %s"), *CustomMessage));
		}
	}
	return false;
}
