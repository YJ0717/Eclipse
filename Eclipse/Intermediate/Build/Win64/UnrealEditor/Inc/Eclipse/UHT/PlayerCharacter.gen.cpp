// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Eclipse/PlayerCharacter.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodePlayerCharacter() {}

// ********** Begin Cross Module References ********************************************************
ECLIPSE_API UClass* Z_Construct_UClass_APlayerCharacter();
ECLIPSE_API UClass* Z_Construct_UClass_APlayerCharacter_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_ACharacter();
ENGINE_API UClass* Z_Construct_UClass_UCameraComponent_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_USpringArmComponent_NoRegister();
ENHANCEDINPUT_API UClass* Z_Construct_UClass_UInputAction_NoRegister();
ENHANCEDINPUT_API UClass* Z_Construct_UClass_UInputMappingContext_NoRegister();
UPackage* Z_Construct_UPackage__Script_Eclipse();
// ********** End Cross Module References **********************************************************

// ********** Begin Class APlayerCharacter *********************************************************
void APlayerCharacter::StaticRegisterNativesAPlayerCharacter()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_APlayerCharacter;
UClass* APlayerCharacter::GetPrivateStaticClass()
{
	using TClass = APlayerCharacter;
	if (!Z_Registration_Info_UClass_APlayerCharacter.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("PlayerCharacter"),
			Z_Registration_Info_UClass_APlayerCharacter.InnerSingleton,
			StaticRegisterNativesAPlayerCharacter,
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
	return Z_Registration_Info_UClass_APlayerCharacter.InnerSingleton;
}
UClass* Z_Construct_UClass_APlayerCharacter_NoRegister()
{
	return APlayerCharacter::GetPrivateStaticClass();
}
struct Z_Construct_UClass_APlayerCharacter_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "PlayerCharacter.h" },
		{ "ModuleRelativePath", "PlayerCharacter.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DefaultMappingContext_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Input" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// \xec\x9e\x85\xeb\xa0\xa5 \xeb\xa7\xa4\xed\x95\x91 \xec\xbb\xa8\xed\x85\x8d\xec\x8a\xa4\xed\x8a\xb8\n" },
#endif
		{ "ModuleRelativePath", "PlayerCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "\xec\x9e\x85\xeb\xa0\xa5 \xeb\xa7\xa4\xed\x95\x91 \xec\xbb\xa8\xed\x85\x8d\xec\x8a\xa4\xed\x8a\xb8" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_JumpAction_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Input" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// \xec\xa0\x90\xed\x94\x84 \xec\x9e\x85\xeb\xa0\xa5 \xec\x95\xa1\xec\x85\x98\n" },
#endif
		{ "ModuleRelativePath", "PlayerCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "\xec\xa0\x90\xed\x94\x84 \xec\x9e\x85\xeb\xa0\xa5 \xec\x95\xa1\xec\x85\x98" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MoveAction_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Input" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// \xec\x9d\xb4\xeb\x8f\x99 \xec\x9e\x85\xeb\xa0\xa5 \xec\x95\xa1\xec\x85\x98\n" },
#endif
		{ "ModuleRelativePath", "PlayerCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "\xec\x9d\xb4\xeb\x8f\x99 \xec\x9e\x85\xeb\xa0\xa5 \xec\x95\xa1\xec\x85\x98" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LookAction_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Input" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// \xeb\x91\x98\xeb\x9f\xac\xeb\xb3\xb4\xea\xb8\xb0 \xec\x9e\x85\xeb\xa0\xa5 \xec\x95\xa1\xec\x85\x98\n" },
#endif
		{ "ModuleRelativePath", "PlayerCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "\xeb\x91\x98\xeb\x9f\xac\xeb\xb3\xb4\xea\xb8\xb0 \xec\x9e\x85\xeb\xa0\xa5 \xec\x95\xa1\xec\x85\x98" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CameraBoom_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Camera" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// \xec\xba\x90\xeb\xa6\xad\xed\x84\xb0 \xeb\x92\xa4\xec\x97\x90 \xec\xb9\xb4\xeb\xa9\x94\xeb\x9d\xbc\xeb\xa5\xbc \xeb\xb0\xb0\xec\xb9\x98\xed\x95\x98\xeb\x8a\x94 \xec\xb9\xb4\xeb\xa9\x94\xeb\x9d\xbc \xeb\xb6\x90\n" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "PlayerCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "\xec\xba\x90\xeb\xa6\xad\xed\x84\xb0 \xeb\x92\xa4\xec\x97\x90 \xec\xb9\xb4\xeb\xa9\x94\xeb\x9d\xbc\xeb\xa5\xbc \xeb\xb0\xb0\xec\xb9\x98\xed\x95\x98\xeb\x8a\x94 \xec\xb9\xb4\xeb\xa9\x94\xeb\x9d\xbc \xeb\xb6\x90" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_FollowCamera_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Camera" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// \xeb\x94\xb0\xeb\x9d\xbc\xea\xb0\x80\xeb\x8a\x94 \xec\xb9\xb4\xeb\xa9\x94\xeb\x9d\xbc\n" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "PlayerCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "\xeb\x94\xb0\xeb\x9d\xbc\xea\xb0\x80\xeb\x8a\x94 \xec\xb9\xb4\xeb\xa9\x94\xeb\x9d\xbc" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_DefaultMappingContext;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_JumpAction;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_MoveAction;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_LookAction;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_CameraBoom;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_FollowCamera;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<APlayerCharacter>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APlayerCharacter_Statics::NewProp_DefaultMappingContext = { "DefaultMappingContext", nullptr, (EPropertyFlags)0x0020080000000015, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(APlayerCharacter, DefaultMappingContext), Z_Construct_UClass_UInputMappingContext_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DefaultMappingContext_MetaData), NewProp_DefaultMappingContext_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APlayerCharacter_Statics::NewProp_JumpAction = { "JumpAction", nullptr, (EPropertyFlags)0x0020080000000015, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(APlayerCharacter, JumpAction), Z_Construct_UClass_UInputAction_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_JumpAction_MetaData), NewProp_JumpAction_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APlayerCharacter_Statics::NewProp_MoveAction = { "MoveAction", nullptr, (EPropertyFlags)0x0020080000000015, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(APlayerCharacter, MoveAction), Z_Construct_UClass_UInputAction_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MoveAction_MetaData), NewProp_MoveAction_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APlayerCharacter_Statics::NewProp_LookAction = { "LookAction", nullptr, (EPropertyFlags)0x0020080000000015, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(APlayerCharacter, LookAction), Z_Construct_UClass_UInputAction_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LookAction_MetaData), NewProp_LookAction_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APlayerCharacter_Statics::NewProp_CameraBoom = { "CameraBoom", nullptr, (EPropertyFlags)0x00200800000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(APlayerCharacter, CameraBoom), Z_Construct_UClass_USpringArmComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CameraBoom_MetaData), NewProp_CameraBoom_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_APlayerCharacter_Statics::NewProp_FollowCamera = { "FollowCamera", nullptr, (EPropertyFlags)0x00200800000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(APlayerCharacter, FollowCamera), Z_Construct_UClass_UCameraComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_FollowCamera_MetaData), NewProp_FollowCamera_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_APlayerCharacter_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APlayerCharacter_Statics::NewProp_DefaultMappingContext,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APlayerCharacter_Statics::NewProp_JumpAction,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APlayerCharacter_Statics::NewProp_MoveAction,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APlayerCharacter_Statics::NewProp_LookAction,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APlayerCharacter_Statics::NewProp_CameraBoom,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_APlayerCharacter_Statics::NewProp_FollowCamera,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_APlayerCharacter_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_APlayerCharacter_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_ACharacter,
	(UObject* (*)())Z_Construct_UPackage__Script_Eclipse,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_APlayerCharacter_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_APlayerCharacter_Statics::ClassParams = {
	&APlayerCharacter::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_APlayerCharacter_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_APlayerCharacter_Statics::PropPointers),
	0,
	0x009001A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_APlayerCharacter_Statics::Class_MetaDataParams), Z_Construct_UClass_APlayerCharacter_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_APlayerCharacter()
{
	if (!Z_Registration_Info_UClass_APlayerCharacter.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_APlayerCharacter.OuterSingleton, Z_Construct_UClass_APlayerCharacter_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_APlayerCharacter.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR(APlayerCharacter);
APlayerCharacter::~APlayerCharacter() {}
// ********** End Class APlayerCharacter ***********************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_PlayerCharacter_h__Script_Eclipse_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_APlayerCharacter, APlayerCharacter::StaticClass, TEXT("APlayerCharacter"), &Z_Registration_Info_UClass_APlayerCharacter, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(APlayerCharacter), 107766397U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_PlayerCharacter_h__Script_Eclipse_859235412(TEXT("/Script/Eclipse"),
	Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_PlayerCharacter_h__Script_Eclipse_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_PlayerCharacter_h__Script_Eclipse_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
