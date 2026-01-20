// Copyright tdxh. All Rights Reserved.

#pragma once

#include "ShaderDirectoryMapping.h"

#include "ShaderDirectoryMappingSettings.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"
#include "Misc/MessageDialog.h"
#include "HAL/PlatformFilemanager.h"

#define LOCTEXT_NAMESPACE "FShaderDirectoryMappingModule"

DEFINE_LOG_CATEGORY(LogShaderDirectoryMapping);

void LogExistedMapping()
{
	const auto& ExistingMappings = AllShaderSourceDirectoryMappings();
	int Count = 0;
	for (const auto& Mapping : ExistingMappings)
	{
		UE_LOG(LogShaderDirectoryMapping, Log,
			TEXT("Existed Mapping show [%d]: %s -> %s"),
			  Count, *Mapping.Key, *Mapping.Value);
		++Count;
	}
}

void FShaderDirectoryMappingModule::StartupModule()
{
	UE_LOG(LogShaderDirectoryMapping, Log, TEXT("ShaderDirectoryMapping Plugin starting up"));
	LogExistedMapping();
	ApplyShaderMappings();
	UE_LOG(LogShaderDirectoryMapping, Log, TEXT("ShaderDirectoryMapping Plugin finished running"));
}

void FShaderDirectoryMappingModule::ShutdownModule()
{
	UE_LOG(LogShaderDirectoryMapping, Log, TEXT("ShaderDirectoryMapping Plugin shutting down, you need to restart Unreal Editor"));
}

TOptional<FShaderDirectoryMapping> GetDefaultShaderDirectoryMapping()
{
	const UShaderDirectoryMappingSettings* Settings = GetDefault<UShaderDirectoryMappingSettings>();

	if (!Settings->bUseDefaultProjectShaders) return TOptional<FShaderDirectoryMapping>();

	FShaderDirectoryMapping DefaultProjectShaders;

	DefaultProjectShaders.bEnabled = true;
	DefaultProjectShaders.VirtualPath = Settings->DefaultVirtualPath;
	DefaultProjectShaders.TargetPath = Settings->DefaultTargetPath;
	DefaultProjectShaders.TargetPathType = Settings->TargetPathType;

	return DefaultProjectShaders;
}

bool ShouldUseDefaultProjectShaders()
{
	return  GetDefault<UShaderDirectoryMappingSettings>()->bUseDefaultProjectShaders;
}

void CheckAndRemoveExistedInputMapping(TArray<FShaderDirectoryMapping>& MappingToBeApplied)
{
	const auto& ExistingMappings = AllShaderSourceDirectoryMappings();
	
	TArray<FShaderDirectoryMapping> ValidMappings;

	int FirstIndexExceptDefault = 0;
	
	// check the default one
	if (ShouldUseDefaultProjectShaders() && MappingToBeApplied.Num() > 0)
	{
		FirstIndexExceptDefault = 1;
		
		auto& DefaultMapping = MappingToBeApplied[0];
		if (const FString* ExistingRealPath = ExistingMappings.Find(DefaultMapping.VirtualPath))
		{
			const FString ExistingFullPath = FPaths::ConvertRelativePathToFull(*ExistingRealPath);
			const FString MappingFullPath  = FPaths::ConvertRelativePathToFull(DefaultMapping.GetRealPath());
			
			if (ExistingFullPath == MappingFullPath)
			{
				UE_LOG(LogShaderDirectoryMapping, Log,
				TEXT("Mapping[0] skipped: VirtualPath '%s' already mapped to '%s' "),
				  *DefaultMapping.VirtualPath, **ExistingRealPath);
			}
			else
			{
				UE_LOG(LogShaderDirectoryMapping, Error,
				TEXT("Default mapping conflict: VirtualPath '%s' already mapped to '%s', "
		 "but new target is '%s'"),
				*DefaultMapping.VirtualPath, **ExistingRealPath, *DefaultMapping.GetRealPath());
			}
			// we don't need to add it 
		}
		else
		{
			ValidMappings.Add(DefaultMapping);
		}
	}

	for (int32 Index = FirstIndexExceptDefault; Index < MappingToBeApplied.Num(); ++Index)
	{
		const FShaderDirectoryMapping& Mapping = MappingToBeApplied[Index];

		if (const FString* ExistingRealPath = ExistingMappings.Find(Mapping.VirtualPath))
		{
			UE_LOG(LogShaderDirectoryMapping, Error,
				TEXT("Mapping[%d] conflict: VirtualPath '%s' already mapped to '%s' (new target: '%s')"),
				Index, *Mapping.VirtualPath, **ExistingRealPath, *Mapping.GetRealPath());
			// we don't need to add it 
			continue;
		}

		ValidMappings.Add(Mapping);
	}

	MappingToBeApplied = MoveTemp(ValidMappings);
}

void FShaderDirectoryMappingModule::ApplyShaderMappings()
{
	const UShaderDirectoryMappingSettings* Settings = GetDefault<UShaderDirectoryMappingSettings>();
    
	// Validate Paths
	FString ErrorMessage;
	if (!Settings->ValidatePaths(ErrorMessage))
	{
#if WITH_EDITOR
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ErrorMessage));
		UE_LOG(LogShaderDirectoryMapping, Error, TEXT("%s"), *ErrorMessage);
#endif
		return;
	}

	// apply the Shaders path
	TArray<FShaderDirectoryMapping> MappingToBeApplied;

	auto DefaultProjectShadersOpt = GetDefaultShaderDirectoryMapping();
	if (DefaultProjectShadersOpt.IsSet())
	{
		MappingToBeApplied.Emplace(DefaultProjectShadersOpt.GetValue());
	}

	MappingToBeApplied.Append(Settings->CustomShaderMappings);

	// Check the inputs, if any InputMapping exists, will show warning and the remove it.
	CheckAndRemoveExistedInputMapping(MappingToBeApplied);
	
	for (const FShaderDirectoryMapping& Mapping : MappingToBeApplied)
	{
		if (Mapping.bEnabled)
		{
			FString RealPath = Mapping.GetRealPath();
			AddShaderSourceDirectoryMapping(Mapping.VirtualPath, RealPath);
			UE_LOG(LogShaderDirectoryMapping, Log, TEXT("Mapped shader directory: %s -> %s"), 
				*Mapping.VirtualPath, *RealPath);
		}
	}
}
 

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FShaderDirectoryMappingModule, ShaderDirectoryMapping)