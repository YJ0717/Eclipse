// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Eclipse/EclipseGameMode.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeEclipseGameMode() {}

// ********** Begin Cross Module References ********************************************************
ECLIPSE_API UClass* Z_Construct_UClass_AEclipseGameMode();
ECLIPSE_API UClass* Z_Construct_UClass_AEclipseGameMode_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
UPackage* Z_Construct_UPackage__Script_Eclipse();
// ********** End Cross Module References **********************************************************

// ********** Begin Class AEclipseGameMode *********************************************************
void AEclipseGameMode::StaticRegisterNativesAEclipseGameMode()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_AEclipseGameMode;
UClass* AEclipseGameMode::GetPrivateStaticClass()
{
	using TClass = AEclipseGameMode;
	if (!Z_Registration_Info_UClass_AEclipseGameMode.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("EclipseGameMode"),
			Z_Registration_Info_UClass_AEclipseGameMode.InnerSingleton,
			StaticRegisterNativesAEclipseGameMode,
			sizeof(TClass),
			alignof(TClass),
			TClass::StaticClassFlags,
			TClass::StaticClassCastFlags(),
			TClass::StaticConfigName(),
			(UClass::ClassConstructorType)InternalConstructor<TClass>,
			(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
			UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
			&TClass::Super::StaticClass,
			&TClass::WithinClass::StaticClass
		);
	}
	return Z_Registration_Info_UClass_AEclipseGameMode.InnerSingleton;
}
UClass* Z_Construct_UClass_AEclipseGameMode_NoRegister()
{
	return AEclipseGameMode::GetPrivateStaticClass();
}
struct Z_Construct_UClass_AEclipseGameMode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n *  Simple GameMode for a third person game\n */" },
#endif
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering HLOD WorldPartition DataLayers Transformation" },
		{ "IncludePath", "EclipseGameMode.h" },
		{ "ModuleRelativePath", "EclipseGameMode.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Simple GameMode for a third person game" },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AEclipseGameMode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_AEclipseGameMode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AGameModeBase,
	(UObject* (*)())Z_Construct_UPackage__Script_Eclipse,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AEclipseGameMode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AEclipseGameMode_Statics::ClassParams = {
	&AEclipseGameMode::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x008003ADu,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AEclipseGameMode_Statics::Class_MetaDataParams), Z_Construct_UClass_AEclipseGameMode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AEclipseGameMode()
{
	if (!Z_Registration_Info_UClass_AEclipseGameMode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AEclipseGameMode.OuterSingleton, Z_Construct_UClass_AEclipseGameMode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AEclipseGameMode.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AEclipseGameMode);
AEclipseGameMode::~AEclipseGameMode() {}
// ********** End Class AEclipseGameMode ***********************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseGameMode_h__Script_Eclipse_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AEclipseGameMode, AEclipseGameMode::StaticClass, TEXT("AEclipseGameMode"), &Z_Registration_Info_UClass_AEclipseGameMode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AEclipseGameMode), 2148044076U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseGameMode_h__Script_Eclipse_1534710566(TEXT("/Script/Eclipse"),
	Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseGameMode_h__Script_Eclipse_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseGameMode_h__Script_Eclipse_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
