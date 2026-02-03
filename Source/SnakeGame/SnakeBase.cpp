// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeBase.h"
#include "SnakeElementBase.h"
#include "Interactable.h"

// Sets default values
ASnakeBase::ASnakeBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ElementSize = 100.f;
	MovementSpeed = 10.f;
	TickSpeed = 0.5f;
	LastMoveDirection = EMovementDirection::LEFT;
}

// Called when the game starts or when spawned
void ASnakeBase::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickInterval(TickSpeed);
	AddSnakeElement(4);
}

// Called every frame
void ASnakeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Move();
}

void ASnakeBase::AddSnakeElement(int ElementsNum)
{
	int MakeVectorX = 0;
	int MakeVectorY = 0;
	switch (LastMoveDirection)
	{
	case EMovementDirection::UP:
		MakeVectorX = -1;
		break;
	case EMovementDirection::DOWN:
		MakeVectorX = 1;
		break;
	case EMovementDirection::RIGHT:
		MakeVectorY = -1;
		break;
	case EMovementDirection::LEFT:
		MakeVectorY = 1;
		break;
	}

	for (int i = 0; i < ElementsNum; ++i) {
		FVector NewLocation((MakeVectorX * SnakeElements.Num() * ElementSize), (MakeVectorY * SnakeElements.Num() * ElementSize), 0);
		FTransform NewTransform(NewLocation);
		ASnakeElementBase* NewSnakeElement = GetWorld()->SpawnActor<ASnakeElementBase>(SnakeElementClass, NewTransform);
		NewSnakeElement->SnakeOwner = this;
		int32 ElementIndex = SnakeElements.Add(NewSnakeElement);
		if (ElementIndex == 0) {
			NewSnakeElement->SetFirstElementType();
		}
	}
}

void ASnakeBase::Move()
{
	FVector MovementVector(FVector::ZeroVector);
	switch (LastMoveDirection)
	{
	case EMovementDirection::UP:
		MovementVector.X += MovementSpeed;
		break;
	case EMovementDirection::DOWN:
		MovementVector.X -= MovementSpeed;
		break;
	case EMovementDirection::RIGHT:
		MovementVector.Y += MovementSpeed;
		break;
	case EMovementDirection::LEFT:
		MovementVector.Y -= MovementSpeed;
		break;
	}

	SnakeElements[0]->ToggleCollision();

	for (int i = SnakeElements.Num() - 1; i > 0; i--) {
		auto CurrentElement = SnakeElements[i];
		auto PrevElement = SnakeElements[i-1];
		FVector PrevLocation = PrevElement->GetActorLocation();
		CurrentElement->SetActorLocation(PrevLocation);
	}

	SnakeElements[0]->AddActorWorldOffset(MovementVector);
	SnakeElements[0]->ToggleCollision();
}

void ASnakeBase::SnakeElementOverlap(ASnakeElementBase* OverlappedElement, AActor* Other)
{
	if (IsValid(OverlappedElement))
	{
		int32 ElemIndex;
		SnakeElements.Find(OverlappedElement, ElemIndex);
		bool bIsFirst = ElemIndex == 0;
		IInteractable* InteractableInterface = Cast<IInteractable>(Other);
		if (InteractableInterface)
		{
			InteractableInterface->Interact(this, bIsFirst);
		}
	}
}


