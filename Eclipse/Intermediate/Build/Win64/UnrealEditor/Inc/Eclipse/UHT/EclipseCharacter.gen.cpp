// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Eclipse/EclipseCharacter.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeEclipseCharacter() {}

// ********** Begin Cross Module References ********************************************************
ECLIPSE_API UClass* Z_Construct_UClass_AEclipseCharacter();
ECLIPSE_API UClass* Z_Construct_UClass_AEclipseCharacter_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_ACharacter();
ENGINE_API UClass* Z_Construct_UClass_UCameraComponent_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_USpringArmComponent_NoRegister();
ENHANCEDINPUT_API UClass* Z_Construct_UClass_UInputAction_NoRegister();
UPackage* Z_Construct_UPackage__Script_Eclipse();
// ********** End Cross Module References **********************************************************

// ********** Begin Class AEclipseCharacter Function DoJumpEnd *************************************
struct Z_Construct_UFunction_AEclipseCharacter_DoJumpEnd_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Input" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Handles jump pressed inputs from either controls or UI interfaces */" },
#endif
		{ "ModuleRelativePath", "EclipseCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Handles jump pressed inputs from either controls or UI interfaces" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AEclipseCharacter_DoJumpEnd_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_AEclipseCharacter, nullptr, "DoJumpEnd", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020400, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_AEclipseCharacter_DoJumpEnd_Statics::Function_MetaDataParams), Z_Construct_UFunction_AEclipseCharacter_DoJumpEnd_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_AEclipseCharacter_DoJumpEnd()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AEclipseCharacter_DoJumpEnd_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(AEclipseCharacter::execDoJumpEnd)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->DoJumpEnd();
	P_NATIVE_END;
}
// ********** End Class AEclipseCharacter Function DoJumpEnd ***************************************

// ********** Begin Class AEclipseCharacter Function DoJumpStart ***********************************
struct Z_Construct_UFunction_AEclipseCharacter_DoJumpStart_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Input" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Handles jump pressed inputs from either controls or UI interfaces */" },
#endif
		{ "ModuleRelativePath", "EclipseCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Handles jump pressed inputs from either controls or UI interfaces" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AEclipseCharacter_DoJumpStart_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_AEclipseCharacter, nullptr, "DoJumpStart", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020400, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_AEclipseCharacter_DoJumpStart_Statics::Function_MetaDataParams), Z_Construct_UFunction_AEclipseCharacter_DoJumpStart_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_AEclipseCharacter_DoJumpStart()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AEclipseCharacter_DoJumpStart_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(AEclipseCharacter::execDoJumpStart)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->DoJumpStart();
	P_NATIVE_END;
}
// ********** End Class AEclipseCharacter Function DoJumpStart *************************************

// ********** Begin Class AEclipseCharacter Function DoLook ****************************************
struct Z_Construct_UFunction_AEclipseCharacter_DoLook_Statics
{
	struct EclipseCharacter_eventDoLook_Parms
	{
		float Yaw;
		float Pitch;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Input" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Handles look inputs from either controls or UI interfaces */" },
#endif
		{ "ModuleRelativePath", "EclipseCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Handles look inputs from either controls or UI interfaces" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Yaw;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Pitch;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_AEclipseCharacter_DoLook_Statics::NewProp_Yaw = { "Yaw", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(EclipseCharacter_eventDoLook_Parms, Yaw), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_AEclipseCharacter_DoLook_Statics::NewProp_Pitch = { "Pitch", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(EclipseCharacter_eventDoLook_Parms, Pitch), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AEclipseCharacter_DoLook_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AEclipseCharacter_DoLook_Statics::NewProp_Yaw,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AEclipseCharacter_DoLook_Statics::NewProp_Pitch,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_AEclipseCharacter_DoLook_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AEclipseCharacter_DoLook_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_AEclipseCharacter, nullptr, "DoLook", Z_Construct_UFunction_AEclipseCharacter_DoLook_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_AEclipseCharacter_DoLook_Statics::PropPointers), sizeof(Z_Construct_UFunction_AEclipseCharacter_DoLook_Statics::EclipseCharacter_eventDoLook_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020400, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_AEclipseCharacter_DoLook_Statics::Function_MetaDataParams), Z_Construct_UFunction_AEclipseCharacter_DoLook_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_AEclipseCharacter_DoLook_Statics::EclipseCharacter_eventDoLook_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_AEclipseCharacter_DoLook()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AEclipseCharacter_DoLook_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(AEclipseCharacter::execDoLook)
{
	P_GET_PROPERTY(FFloatProperty,Z_Param_Yaw);
	P_GET_PROPERTY(FFloatProperty,Z_Param_Pitch);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->DoLook(Z_Param_Yaw,Z_Param_Pitch);
	P_NATIVE_END;
}
// ********** End Class AEclipseCharacter Function DoLook ******************************************

// ********** Begin Class AEclipseCharacter Function DoMove ****************************************
struct Z_Construct_UFunction_AEclipseCharacter_DoMove_Statics
{
	struct EclipseCharacter_eventDoMove_Parms
	{
		float Right;
		float Forward;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Input" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Handles move inputs from either controls or UI interfaces */" },
#endif
		{ "ModuleRelativePath", "EclipseCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Handles move inputs from either controls or UI interfaces" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Right;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Forward;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_AEclipseCharacter_DoMove_Statics::NewProp_Right = { "Right", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(EclipseCharacter_eventDoMove_Parms, Right), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_AEclipseCharacter_DoMove_Statics::NewProp_Forward = { "Forward", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(EclipseCharacter_eventDoMove_Parms, Forward), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AEclipseCharacter_DoMove_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AEclipseCharacter_DoMove_Statics::NewProp_Right,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AEclipseCharacter_DoMove_Statics::NewProp_Forward,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_AEclipseCharacter_DoMove_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AEclipseCharacter_DoMove_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_AEclipseCharacter, nullptr, "DoMove", Z_Construct_UFunction_AEclipseCharacter_DoMove_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_AEclipseCharacter_DoMove_Statics::PropPointers), sizeof(Z_Construct_UFunction_AEclipseCharacter_DoMove_Statics::EclipseCharacter_eventDoMove_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020400, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_AEclipseCharacter_DoMove_Statics::Function_MetaDataParams), Z_Construct_UFunction_AEclipseCharacter_DoMove_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_AEclipseCharacter_DoMove_Statics::EclipseCharacter_eventDoMove_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_AEclipseCharacter_DoMove()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AEclipseCharacter_DoMove_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(AEclipseCharacter::execDoMove)
{
	P_GET_PROPERTY(FFloatProperty,Z_Param_Right);
	P_GET_PROPERTY(FFloatProperty,Z_Param_Forward);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->DoMove(Z_Param_Right,Z_Param_Forward);
	P_NATIVE_END;
}
// ********** End Class AEclipseCharacter Function DoMove ******************************************

// ********** Begin Class AEclipseCharacter ********************************************************
void AEclipseCharacter::StaticRegisterNativesAEclipseCharacter()
{
	UClass* Class = AEclipseCharacter::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "DoJumpEnd", &AEclipseCharacter::execDoJumpEnd },
		{ "DoJumpStart", &AEclipseCharacter::execDoJumpStart },
		{ "DoLook", &AEclipseCharacter::execDoLook },
		{ "DoMove", &AEclipseCharacter::execDoMove },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
FClassRegistrationInfo Z_Registration_Info_UClass_AEclipseCharacter;
UClass* AEclipseCharacter::GetPrivateStaticClass()
{
	using TClass = AEclipseCharacter;
	if (!Z_Registration_Info_UClass_AEclipseCharacter.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("EclipseCharacter"),
			Z_Registration_Info_UClass_AEclipseCharacter.InnerSingleton,
			StaticRegisterNativesAEclipseCharacter,
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
	return Z_Registration_Info_UClass_AEclipseCharacter.InnerSingleton;
}
UClass* Z_Construct_UClass_AEclipseCharacter_NoRegister()
{
	return AEclipseCharacter::GetPrivateStaticClass();
}
struct Z_Construct_UClass_AEclipseCharacter_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n *  A simple player-controllable third person character\n *  Implements a controllable orbiting camera\n */" },
#endif
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "EclipseCharacter.h" },
		{ "ModuleRelativePath", "EclipseCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "A simple player-controllable third person character\nImplements a controllable orbiting camera" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CameraBoom_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Camera" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Camera boom positioning the camera behind the character */" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "EclipseCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Camera boom positioning the camera behind the character" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_FollowCamera_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Camera" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Follow camera */" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "EclipseCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Follow camera" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_JumpAction_MetaData[] = {
		{ "Category", "Input" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Jump Input Action */" },
#endif
		{ "ModuleRelativePath", "EclipseCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Jump Input Action" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MoveAction_MetaData[] = {
		{ "Category", "Input" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Move Input Action */" },
#endif
		{ "ModuleRelativePath", "EclipseCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Move Input Action" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LookAction_MetaData[] = {
		{ "Category", "Input" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Look Input Action */" },
#endif
		{ "ModuleRelativePath", "EclipseCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Look Input Action" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MouseLookAction_MetaData[] = {
		{ "Category", "Input" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Mouse Look Input Action */" },
#endif
		{ "ModuleRelativePath", "EclipseCharacter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Mouse Look Input Action" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_CameraBoom;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_FollowCamera;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_JumpAction;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_MoveAction;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_LookAction;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_MouseLookAction;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_AEclipseCharacter_DoJumpEnd, "DoJumpEnd" }, // 2848870597
		{ &Z_Construct_UFunction_AEclipseCharacter_DoJumpStart, "DoJumpStart" }, // 566847514
		{ &Z_Construct_UFunction_AEclipseCharacter_DoLook, "DoLook" }, // 2818070588
		{ &Z_Construct_UFunction_AEclipseCharacter_DoMove, "DoMove" }, // 3135240782
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AEclipseCharacter>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AEclipseCharacter_Statics::NewProp_CameraBoom = { "CameraBoom", nullptr, (EPropertyFlags)0x00400000000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AEclipseCharacter, CameraBoom), Z_Construct_UClass_USpringArmComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CameraBoom_MetaData), NewProp_CameraBoom_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AEclipseCharacter_Statics::NewProp_FollowCamera = { "FollowCamera", nullptr, (EPropertyFlags)0x00400000000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AEclipseCharacter, FollowCamera), Z_Construct_UClass_UCameraComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_FollowCamera_MetaData), NewProp_FollowCamera_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AEclipseCharacter_Statics::NewProp_JumpAction = { "JumpAction", nullptr, (EPropertyFlags)0x0020080000000015, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AEclipseCharacter, JumpAction), Z_Construct_UClass_UInputAction_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_JumpAction_MetaData), NewProp_JumpAction_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AEclipseCharacter_Statics::NewProp_MoveAction = { "MoveAction", nullptr, (EPropertyFlags)0x0020080000000015, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AEclipseCharacter, MoveAction), Z_Construct_UClass_UInputAction_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MoveAction_MetaData), NewProp_MoveAction_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AEclipseCharacter_Statics::NewProp_LookAction = { "LookAction", nullptr, (EPropertyFlags)0x0020080000000015, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AEclipseCharacter, LookAction), Z_Construct_UClass_UInputAction_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LookAction_MetaData), NewProp_LookAction_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AEclipseCharacter_Statics::NewProp_MouseLookAction = { "MouseLookAction", nullptr, (EPropertyFlags)0x0020080000000015, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AEclipseCharacter, MouseLookAction), Z_Construct_UClass_UInputAction_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MouseLookAction_MetaData), NewProp_MouseLookAction_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AEclipseCharacter_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AEclipseCharacter_Statics::NewProp_CameraBoom,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AEclipseCharacter_Statics::NewProp_FollowCamera,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AEclipseCharacter_Statics::NewProp_JumpAction,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AEclipseCharacter_Statics::NewProp_MoveAction,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AEclipseCharacter_Statics::NewProp_LookAction,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AEclipseCharacter_Statics::NewProp_MouseLookAction,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AEclipseCharacter_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_AEclipseCharacter_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_ACharacter,
	(UObject* (*)())Z_Construct_UPackage__Script_Eclipse,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AEclipseCharacter_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AEclipseCharacter_Statics::ClassParams = {
	&AEclipseCharacter::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_AEclipseCharacter_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_AEclipseCharacter_Statics::PropPointers),
	0,
	0x008001A5u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AEclipseCharacter_Statics::Class_MetaDataParams), Z_Construct_UClass_AEclipseCharacter_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AEclipseCharacter()
{
	if (!Z_Registration_Info_UClass_AEclipseCharacter.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AEclipseCharacter.OuterSingleton, Z_Construct_UClass_AEclipseCharacter_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AEclipseCharacter.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AEclipseCharacter);
AEclipseCharacter::~AEclipseCharacter() {}
// ********** End Class AEclipseCharacter **********************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseCharacter_h__Script_Eclipse_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AEclipseCharacter, AEclipseCharacter::StaticClass, TEXT("AEclipseCharacter"), &Z_Registration_Info_UClass_AEclipseCharacter, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AEclipseCharacter), 966048679U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseCharacter_h__Script_Eclipse_869360647(TEXT("/Script/Eclipse"),
	Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseCharacter_h__Script_Eclipse_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseCharacter_h__Script_Eclipse_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
