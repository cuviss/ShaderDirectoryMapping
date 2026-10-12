// Copyright tdxh. All Rights Reserved.

#pragma once

#include "ShaderDirectoryMappingSettings.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"

FString FShaderDirectoryMapping::GetRealPath() const
{
	if (TargetPathType == EShaderPathType::ProjectRelative)
	{
		return FPaths::Combine(FPaths::ProjectDir(), TargetPath);
	}
	return TargetPath;
}

UShaderDirectoryMappingSettings::UShaderDirectoryMappingSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 
}

bool UShaderDirectoryMappingSettings::ValidatePaths(FString& OutErrorMessage) const
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	TArray<FShaderDirectoryMapping> MappingToBeChecked;
	
	if (bUseDefaultProjectShaders)
	{
		FShaderDirectoryMapping DefaultProjectShaders;

		DefaultProjectShaders.bEnabled = true;
		DefaultProjectShaders.VirtualPath = DefaultVirtualPath;
		DefaultProjectShaders.TargetPath = DefaultTargetPath;
		DefaultProjectShaders.TargetPathType = EShaderPathType::ProjectRelative;

		MappingToBeChecked.Emplace(DefaultProjectShaders);
	}

	MappingToBeChecked.Append(CustomShaderMappings);

	for (const FShaderDirectoryMapping& Mapping : MappingToBeChecked)
	{
		if (Mapping.bEnabled)
		{
			const FString RealPath = Mapping.GetRealPath();
			if (!PlatformFile.DirectoryExists(*RealPath))
			{
#if WITH_EDITOR
				OutErrorMessage = FString::Printf(TEXT("Shader directory does not exist: %s (Virtual: %s)"), *RealPath, *Mapping.VirtualPath);
#endif
				return false;
			}
		}
	}

	return true;
}