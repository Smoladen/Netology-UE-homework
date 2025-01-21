// LeaveMeAlone Game by Netologya. All rights Reserved


#include "Player/LMAPlayerController.h"

ALMAPlayerController::ALMAPlayerController() {

}

void ALMAPlayerController::BeginPlay() {
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}