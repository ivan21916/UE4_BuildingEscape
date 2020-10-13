// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Grabber.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();

	SetupInputComponent();


}

void UGrabber::SetupInputComponent(){
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	if(InputComponent){
		// UE_LOG(LogTemp, Warning, TEXT("Input Component found on %s!"), *GetOwner()->GetName());
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	// else{
	// 	UE_LOG(LogTemp, Warning, TEXT("Input Component not found!"), *GetOwner()->GetName());
	// }

}

void UGrabber::FindPhysicsHandle(){
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if(!PhysicsHandle){
		UE_LOG(LogTemp, Warning, TEXT("No physics handle component found on %s!"), *GetOwner()->GetName());
	}
}

void UGrabber::Grab(){
	// UE_LOG(LogTemp, Warning, TEXT("Grabber Pressed"));

	FHitResult HitResult = GetFirstPhysicsBodyReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();

	if(ActorHit){
		if(!PhysicsHandle){
			return;
		}
		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			GetPlayerReach()
		);
	}
}


void UGrabber::Release(){
	// UE_LOG(LogTemp, Warning, TEXT("Grabber Released"));
	if(!PhysicsHandle){
		return;
	}
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DrawDebugLine(
		GetWorld(),
		GetPlayerWorldPos(),
		GetPlayerReach(),
		FColor(0, 255, 0),
		false,
		0.f,
		0,
		5.f
	);

	if(PhysicsHandle->GrabbedComponent){
		PhysicsHandle->SetTargetLocation(GetPlayerReach());
	}
}

FHitResult UGrabber::GetFirstPhysicsBodyReach() const{

	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		Hit,
		GetPlayerWorldPos(),
		GetPlayerReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);
	// AActor* ActorHit = Hit.GetActor();
	
	// if(ActorHit){
	// 	UE_LOG(LogTemp, Warning, TEXT("Line has hit %s"), *(ActorHit->GetName()));
	// } 

	return Hit;
}

FVector UGrabber::GetPlayerWorldPos() const{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);
	// FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
	// UE_LOG(LogTemp, Warning, TEXT("Location is: %s, Rotation is %s"), *PlayerViewPointLocation.ToString(), *PlayerViewPointRotation.ToString());
	return PlayerViewPointLocation;
}

FVector UGrabber::GetPlayerReach() const{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);
	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}