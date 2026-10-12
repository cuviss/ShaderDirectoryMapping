// Copyright tdxh. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogShaderDirectoryMapping, Log, All);

class FShaderDirectoryMappingModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static void ApplyShaderMappings();
};
