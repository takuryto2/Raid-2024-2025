// Fill out your copyright notice in the Description page of Project Settings.

#include "3C/CharacterPawnMovementComponent.h"

#include "MaterialHLSLTree.h"
#include "Components/CapsuleComponent.h"
#include "Math/UnitConversion.h"

// Sets default values for this component's properties
UCharacterPawnMovementComponent::UCharacterPawnMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UCharacterPawnMovementComponent::BeginPlay()
{
    Super::BeginPlay();

    CollisionParams = FCollisionQueryParams(FName(TEXT("FeetTrace")), false, GetOwner());
    if (UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(UpdatedComponent))
    {
        FeetShape = FCollisionShape::MakeSphere(Capsule->GetScaledCapsuleRadius() - FeetSkin);
        halfHeight = Capsule->GetScaledCapsuleHalfHeight_WithoutHemisphere();
        CapsuleStep = Capsule->GetScaledCapsuleHalfHeight() - Capsule->GetScaledCapsuleHalfHeight_WithoutHemisphere();

        StepMult = 1.0f;
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
        FVector MoveVector = FVector(0, MoveVector2.Y, MoveVector2.X);

        UpdatedComponent->MoveComponent(MoveVector, FQuat::Identity, true);

        if (DashTimer <= 0.0f)
            bIsDashing = false;
    }
    else if (!CurrentDirection.IsNearlyZero())
    {
        if (bCanMove)
        {
            WalkProgress += DeltaTime / AccelTime;
            WalkProgress = FMath::Clamp(WalkProgress, 0.f, 1.f);

            CurrentSpeed = SpeedCurve->GetFloatValue(WalkProgress) * VMax;

            FVector2D MoveVector2 = CurrentSpeed * DeltaTime * CurrentDirection;
            FVector MoveVector = FVector(0, MoveVector2.Y, MoveVector2.X) ;
            
            UpdatedComponent->MoveComponent(MoveVector, FQuat::Identity, true);

            // Collide and slide async trace
            PerformSlideAsyncTrace(MoveVector);
        }
    }
}


void UCharacterPawnMovementComponent::JumpInput()
{
    
}

void UCharacterPawnMovementComponent::DashInput()
{
    bCanMove = false;
    if (!bIsDashing && DashCooldownTimer <= 0.0f)
    {
        bIsDashing = true;
        DashTimer = DashTime;
        if (CurrentDirection.IsZero())
        {
            DashDirection = FVector2D(0, 1);
        }
        else
        {
            DashDirection = CurrentDirection;
        }
        DashCooldownTimer = DashCooldown + DashTime;
    }
    MoveInput(FVector2D(0, 0));
    bCanMove = true;
}

void UCharacterPawnMovementComponent::MoveInput(const FVector2D& Direction)
{
    CurrentDirection = Direction.GetSafeNormal();
}

void UCharacterPawnMovementComponent::PerformSlideAsyncTrace(const FVector& MoveVector)
{
    if (!GetWorld() || !UpdatedComponent)
        return;

    
    // FVector origin;
    // FVector boxExtent;

    // GetOwner()->GetActorBounds(false, origin, boxExtent);
    
    const FVector Start = UpdatedComponent->GetComponentLocation() + GetOwner()->GetActorUpVector() * -halfHeight;
    const FVector End = FVector(Start.X, Start.Y,Start.Z - CapsuleStep * StepMult) ;

    FQuat Rotation = UpdatedComponent->GetComponentQuat();

    FTraceDelegate LocalDelegate;
    LocalDelegate.BindUObject(this, &UCharacterPawnMovementComponent::OnAsyncTraceResult);

    GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, TEXT("PerformSlideAsyncTrace called"));
    DrawDebugSphere(GetWorld(), Start, FeetShape.GetSphereRadius(),16, FColor::Blue);
    DrawDebugSphere(GetWorld(), End, FeetShape.GetSphereRadius(),16, FColor::Red);
    GetWorld()->AsyncSweepByProfile(
        EAsyncTraceType::Single,
        Start,
        End,
        FQuat::Identity,
        UCollisionProfile::Pawn_ProfileName,
        FeetShape,
        CollisionParams,
        &LocalDelegate
    );
}

void UCharacterPawnMovementComponent::OnAsyncTraceResult(const FTraceHandle& Handle, FTraceDatum& Datum)
{
    // GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, TEXT("OnAsyncTraceResult called"));
    //
    // GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red,
    //     FString::Printf(TEXT("Async Trace Result: %d hits"), Datum.OutHits.Num()));

    

    if(!GetWorld()->GetWorld()->QueryTraceData(Handle, Datum))
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red,
    FString::Printf(TEXT("No Hit")));
        return;
    }

    if (!Datum.OutHits.Num())
    {
                GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red,
        FString::Printf(TEXT("No OutHits In Datum")));
                return;
    }
    
    const FHitResult& Hit = Datum.OutHits[0];

    UE_LOG(LogTemp, Warning, TEXT("%s"), *Hit.GetActor()->GetName());

#if WITH_EDITOR
    DrawDebugSphere(GetWorld(), Hit.Location, FeetShape.GetSphereRadius(),16, FColor::Green);
#endif
        // Datum.OutHits[0];
    if (!UpdatedComponent)
        return;

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
        FString::Printf(TEXT("Hit: %s at %s"), *Hit.GetActor()->GetName(), *Hit.Location.ToString()));
    
    // Calcul du slide : on glisse le long de la surface percutée
    // FVector Normal = Hit.Normal;
    // FVector DistanceAbs = FVector::VectorPlaneProject(Hit.TraceEnd - Hit.TraceStart, Normal).GetAbs();
    // FVector Deflected = FVector::VectorPlaneProject(Hit.TraceEnd - Hit.TraceStart, Normal).GetSafeNormal();
    //
    // FVector NewLocation = (Hit.Location + Deflected) * (DistanceAbs).Size();
    // UpdatedComponent->SetWorldLocation(NewLocation);
}

