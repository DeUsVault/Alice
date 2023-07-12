// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class ALICE_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACasing();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CasingMesh;

	UPROPERTY(EditAnywhere)
	float CasingEjectionImpulse = 10.f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> CasingSound;

	bool bSoundPlayed = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
