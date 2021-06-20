// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SoundsBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EXWORLDTEST_API USoundsBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
    static void SetUnfocusedVolumeMuted(bool IsMuted);
};
