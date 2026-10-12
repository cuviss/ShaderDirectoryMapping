// Copyright tdxh. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/DeveloperSettings.h"
#include "ShaderDirectoryMappingSettings.generated.h"

UENUM(BlueprintType)
enum class EShaderPathType : uint8
{
	ProjectRelative,
	Absolute
};

USTRUCT(BlueprintType)
struct FShaderDirectoryMapping
{
	GENERATED_BODY()

	// Start with /
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shader", DisplayName= "Virtual Path ( Start with / )")
	FString VirtualPath;

	// Don't Start with /
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shader", DisplayName= "Target Path (Don't Start with / )")
	FString TargetPath; 

	// Choose Relative path or absolute path 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shader")
	EShaderPathType TargetPathType = EShaderPathType::ProjectRelative;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shader")
	bool bEnabled = true;
	
	FString GetRealPath() const;
};

UCLASS(config=Game, defaultconfig, meta=(DisplayName="Shader Directory Mapping"))
class SHADERDIRECTORYMAPPING_API UShaderDirectoryMappingSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UShaderDirectoryMappingSettings(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Default", meta=(ToolTip="Enable default project Shaders directory"))
	bool bUseDefaultProjectShaders = true;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Default", meta=(EditCondition="bUseDefaultProjectShaders", ToolTip="Virtual path default"))
	FString DefaultVirtualPath = "/Project";

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Default", meta=(EditCondition="bUseDefaultProjectShaders", ToolTip="Target path default"))
	FString DefaultTargetPath = "Shaders";

	// Choose Relative path or absolute path 
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Default", meta=(EditCondition="bUseDefaultProjectShaders", ToolTip="Target path Type"))
	EShaderPathType TargetPathType = EShaderPathType::ProjectRelative;

	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category="Custom", meta=(ToolTip="Custom shader directory mappings"))
	TArray<FShaderDirectoryMapping> CustomShaderMappings;
	
	bool ValidatePaths(FString& OutErrorMessage) const;
};