// Fill out your copyright notice in the Description page of Project Settings.


#include "3C/CharacterPawnMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "Math/UnitConversion.h"


// Sets default values for this component's properties
UCharacterPawnMovementComponent::UCharacterPawnMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCharacterPawnMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	CollisionParams = FCollisionQueryParams(FName (TEXT("FeetTrace")), false, GetOwner());
	if (UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(UpdatedComponent))
	{
		FeetShape = FCollisionShape::MakeSphere(Capsule->GetScaledCapsuleRadius() - FeetSkin);
	}
	
}


// Called every frame
void UCharacterPawnMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (DashCooldownTimer > 0.0f)
		DashCooldownTimer -= DeltaTime;

	if (bIsDashing)
	{
		DashTimer -= DeltaTime;
		FVector2D MoveVector2 = DashSpeed * DeltaTime * DashDirection;
		FVector MoveVector = FVector(0, MoveVector2.Y, 0);

		UpdatedComponent->MoveComponent(MoveVector, FQuat::Identity, true);

		if (DashTimer <= 0.0f)
			bIsDashing = false;
	}
	else if (!CurrentDirection.IsNearlyZero())
	{
		if (bCanMove)
		{
					WalkProgress += DeltaTime / AccelTime;
            		FMath::Clamp(WalkProgress, 0, 1);
            
            		CurrentSpeed = SpeedCurve->GetFloatValue(WalkProgress) * VMax;
            
            		FVector2D MoveVector2 = CurrentSpeed * DeltaTime * CurrentDirection;
            		FVector MoveVector = FVector(MoveVector2.X, MoveVector2.Y, 0);
            
            		UpdatedComponent->MoveComponent(MoveVector, FQuat::Identity, true);
		}
	}
}

void UCharacterPawnMovementComponent::JumpInput()
{
	
}

void UCharacterPawnMovementComponent::DashInput()
{
	FVector2D CurrentDashDirection = CurrentDirection;
	bCanMove = false;
	if (!bIsDashing && DashCooldownTimer <= 0.0f)
	{
		bIsDashing = true;
		DashTimer = DashTime;
		DashDirection = CurrentDashDirection;
		DashCooldownTimer = DashCooldown + DashTime;
	}
	bCanMove = true;

}	

void UCharacterPawnMovementComponent::MoveInput(const FVector2D& Direction)
{
	CurrentDirection = Direction.GetSafeNormal();
}

