// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundsBlueprintFunctionLibrary.h"

void USoundsBlueprintFunctionLibrary::SetUnfocusedVolumeMuted(bool IsMuted)
{
    float volume = IsMuted ? 0.f : 1.f;
    GConfig->SetFloat(TEXT("Audio"), TEXT("UnfocusedVolumeMultiplier"), volume, GEngineIni);
    FApp::SetUnfocusedVolumeMultiplier(volume);
}