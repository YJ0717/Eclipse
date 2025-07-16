// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeEclipse_init() {}
	ECLIPSE_API UFunction* Z_Construct_UDelegateFunction_Eclipse_OnEnemyDied__DelegateSignature();
	static FPackageRegistrationInfo Z_Registration_Info_UPackage__Script_Eclipse;
	FORCENOINLINE UPackage* Z_Construct_UPackage__Script_Eclipse()
	{
		if (!Z_Registration_Info_UPackage__Script_Eclipse.OuterSingleton)
		{
			static UObject* (*const SingletonFuncArray[])() = {
				(UObject* (*)())Z_Construct_UDelegateFunction_Eclipse_OnEnemyDied__DelegateSignature,
			};
			static const UECodeGen_Private::FPackageParams PackageParams = {
				"/Script/Eclipse",
				SingletonFuncArray,
				UE_ARRAY_COUNT(SingletonFuncArray),
				PKG_CompiledIn | 0x00000000,
				0x31ABF5BF,
				0xE424A527,
				METADATA_PARAMS(0, nullptr)
			};
			UECodeGen_Private::ConstructUPackage(Z_Registration_Info_UPackage__Script_Eclipse.OuterSingleton, PackageParams);
		}
		return Z_Registration_Info_UPackage__Script_Eclipse.OuterSingleton;
	}
	static FRegisterCompiledInInfo Z_CompiledInDeferPackage_UPackage__Script_Eclipse(Z_Construct_UPackage__Script_Eclipse, TEXT("/Script/Eclipse"), Z_Registration_Info_UPackage__Script_Eclipse, CONSTRUCT_RELOAD_VERSION_INFO(FPackageReloadVersionInfo, 0x31ABF5BF, 0xE424A527));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
