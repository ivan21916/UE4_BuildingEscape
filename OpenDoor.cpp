#include "OpenDoor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/AudioComponent.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw;

	FindPressurePlate();

	FindAudioComponent();
	// ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
	
}

void UOpenDoor::FindAudioComponent(){
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if(!AudioComponent){
		UE_LOG(LogTemp, Error, TEXT("%s missing audio component!"), *GetOwner()->GetName());
	}
}

void UOpenDoor::FindPressurePlate(){
	if(!PressurePlate){ // prevent Unreal crashing
		UE_LOG(LogTemp, Error, TEXT("%s has the open door component on it, but no pressureplate	set"), *GetOwner()->GetName());
	}
}
// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// if(PressurePlate && PressurePlate->IsOverlappingActor(ActorThatOpens)){
	// 	OpenDoor(DeltaTime);
	// 	DoorLastOpened = GetWorld()->GetTimeSeconds();
	// }
	if(TotalMassOfActors() > MassToOpenDoor){
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();		
	}
	else{
		if(GetWorld()->GetTimeSeconds() - DoorLastOpened >= DoorCloseDelay){
			CloseDoor(DeltaTime);
		}

	}

}

void UOpenDoor::OpenDoor(float DeltaTime){

	// UE_LOG(LogTemp, Warning, TEXT("%s"), *GetOwner()->GetActorRotation().ToString());
	// UE_LOG(LogTemp, Warning, TEXT("Yaw is %f"), GetOwner()->GetActorRotation().Yaw);
	
	CurrentYaw = FMath::Lerp(CurrentYaw, OpenAngle, DeltaTime*DoorOpenSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	CloseDoorSound = false;
	if(!AudioComponent){
		return;
	}
	if(!OpenDoorSound){
		AudioComponent->Play();
		OpenDoorSound = true;
	}


	// float CurrentYaw = GetOwner()->GetActorRotation().Yaw;
	// FRotator OpenDoor(0, OpenAngle, 0);

	// OpenDoor.Yaw = FMath::Lerp(CurrentYaw, OpenAngle, 0.02f);
	// // OpenDoor.Yaw = FMath::FInterpConstantTo(CurrentYaw, OpenAngle, DeltaTime, 45); //Linear interpolation
	// // OpenDoor.Yaw = FMath::FInterpTo(CurrentYaw, OpenAngle, DeltaTime, 2);
	// GetOwner()->SetActorRotation(OpenDoor);

	// FRotator CurrentRotation = 	GetOwner()->GetActorRotation();
	// CurrentRotation.Yaw = 0;
}

void UOpenDoor::CloseDoor(float DeltaTime){

	CurrentYaw = FMath::Lerp(CurrentYaw, InitialYaw, DeltaTime*DoorCloseSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	OpenDoorSound = false;
	if(!AudioComponent){
		return;
	}
	if(!CloseDoorSound){
		AudioComponent->Play();
		CloseDoorSound = true;
	}

}

float UOpenDoor::TotalMassOfActors() const{
	float TotalMass = 0.f;

	TArray<AActor*> OverlappingActors;
	if(!PressurePlate){
		return TotalMass;
	}
	PressurePlate->GetOverlappingActors(OverlappingActors);

	for(AActor* Actor : OverlappingActors){
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("%s is on the pressureplate!"), *Actor->GetName());
	}

	return TotalMass;
}