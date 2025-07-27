// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Eclipse/Sg1Monster1.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeSg1Monster1() {}

// ********** Begin Cross Module References ********************************************************
AIMODULE_API UClass* Z_Construct_UClass_UPawnSensingComponent_NoRegister();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector();
ECLIPSE_API UClass* Z_Construct_UClass_APlayerCharacter_NoRegister();
ECLIPSE_API UClass* Z_Construct_UClass_ASg1Monster1();
ECLIPSE_API UClass* Z_Construct_UClass_ASg1Monster1_NoRegister();
ECLIPSE_API UEnum* Z_Construct_UEnum_Eclipse_EMonsterState();
ENGINE_API UClass* Z_Construct_UClass_ACharacter();
ENGINE_API UClass* Z_Construct_UClass_APawn_NoRegister();
UPackage* Z_Construct_UPackage__Script_Eclipse();
// ********** End Cross Module References **********************************************************

// ********** Begin Enum EMonsterState *************************************************************
static FEnumRegistrationInfo Z_Registration_Info_UEnum_EMonsterState;
static UEnum* EMonsterState_StaticEnum()
{
	if (!Z_Registration_Info_UEnum_EMonsterState.OuterSingleton)
	{
		Z_Registration_Info_UEnum_EMonsterState.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_Eclipse_EMonsterState, (UObject*)Z_Construct_UPackage__Script_Eclipse(), TEXT("EMonsterState"));
	}
	return Z_Registration_Info_UEnum_EMonsterState.OuterSingleton;
}
template<> ECLIPSE_API UEnum* StaticEnum<EMonsterState>()
{
	return EMonsterState_StaticEnum();
}
struct Z_Construct_UEnum_Eclipse_EMonsterState_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
		{ "Attack.Name", "EMonsterState::Attack" },
		{ "BlueprintType", "true" },
		{ "Chase.Name", "EMonsterState::Chase" },
		{ "Dead.Name", "EMonsterState::Dead" },
		{ "ModuleRelativePath", "Sg1Monster1.h" },
		{ "Patrol.Name", "EMonsterState::Patrol" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EMonsterState::Patrol", (int64)EMonsterState::Patrol },
		{ "EMonsterState::Chase", (int64)EMonsterState::Chase },
		{ "EMonsterState::Attack", (int64)EMonsterState::Attack },
		{ "EMonsterState::Dead", (int64)EMonsterState::Dead },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
};
const UECodeGen_Private::FEnumParams Z_Construct_UEnum_Eclipse_EMonsterState_Statics::EnumParams = {
	(UObject*(*)())Z_Construct_UPackage__Script_Eclipse,
	nullptr,
	"EMonsterState",
	"EMonsterState",
	Z_Construct_UEnum_Eclipse_EMonsterState_Statics::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(Z_Construct_UEnum_Eclipse_EMonsterState_Statics::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_Eclipse_EMonsterState_Statics::Enum_MetaDataParams), Z_Construct_UEnum_Eclipse_EMonsterState_Statics::Enum_MetaDataParams)
};
UEnum* Z_Construct_UEnum_Eclipse_EMonsterState()
{
	if (!Z_Registration_Info_UEnum_EMonsterState.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EMonsterState.InnerSingleton, Z_Construct_UEnum_Eclipse_EMonsterState_Statics::EnumParams);
	}
	return Z_Registration_Info_UEnum_EMonsterState.InnerSingleton;
}
// ********** End Enum EMonsterState ***************************************************************

// ********** Begin Class ASg1Monster1 Function OnHearNoise ****************************************
struct Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics
{
	struct Sg1Monster1_eventOnHearNoise_Parms
	{
		APawn* PawnInstigator;
		FVector Location;
		float Volume;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Sg1Monster1.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Location_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PawnInstigator;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Location;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Volume;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics::NewProp_PawnInstigator = { "PawnInstigator", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(Sg1Monster1_eventOnHearNoise_Parms, PawnInstigator), Z_Construct_UClass_APawn_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics::NewProp_Location = { "Location", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(Sg1Monster1_eventOnHearNoise_Parms, Location), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Location_MetaData), NewProp_Location_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics::NewProp_Volume = { "Volume", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(Sg1Monster1_eventOnHearNoise_Parms, Volume), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics::NewProp_PawnInstigator,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics::NewProp_Location,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics::NewProp_Volume,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASg1Monster1, nullptr, "OnHearNoise", Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics::PropPointers), sizeof(Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics::Sg1Monster1_eventOnHearNoise_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00C40401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics::Sg1Monster1_eventOnHearNoise_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_ASg1Monster1_OnHearNoise()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASg1Monster1_OnHearNoise_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASg1Monster1::execOnHearNoise)
{
	P_GET_OBJECT(APawn,Z_Param_PawnInstigator);
	P_GET_STRUCT_REF(FVector,Z_Param_Out_Location);
	P_GET_PROPERTY(FFloatProperty,Z_Param_Volume);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnHearNoise(Z_Param_PawnInstigator,Z_Param_Out_Location,Z_Param_Volume);
	P_NATIVE_END;
}
// ********** End Class ASg1Monster1 Function OnHearNoise ******************************************

// ********** Begin Class ASg1Monster1 Function OnSeePawn ******************************************
struct Z_Construct_UFunction_ASg1Monster1_OnSeePawn_Statics
{
	struct Sg1Monster1_eventOnSeePawn_Parms
	{
		APawn* Pawn;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Sg1Monster1.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Pawn;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_ASg1Monster1_OnSeePawn_Statics::NewProp_Pawn = { "Pawn", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(Sg1Monster1_eventOnSeePawn_Parms, Pawn), Z_Construct_UClass_APawn_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_ASg1Monster1_OnSeePawn_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASg1Monster1_OnSeePawn_Statics::NewProp_Pawn,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_ASg1Monster1_OnSeePawn_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASg1Monster1_OnSeePawn_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASg1Monster1, nullptr, "OnSeePawn", Z_Construct_UFunction_ASg1Monster1_OnSeePawn_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_ASg1Monster1_OnSeePawn_Statics::PropPointers), sizeof(Z_Construct_UFunction_ASg1Monster1_OnSeePawn_Statics::Sg1Monster1_eventOnSeePawn_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00040401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASg1Monster1_OnSeePawn_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASg1Monster1_OnSeePawn_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_ASg1Monster1_OnSeePawn_Statics::Sg1Monster1_eventOnSeePawn_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_ASg1Monster1_OnSeePawn()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASg1Monster1_OnSeePawn_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASg1Monster1::execOnSeePawn)
{
	P_GET_OBJECT(APawn,Z_Param_Pawn);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnSeePawn(Z_Param_Pawn);
	P_NATIVE_END;
}
// ********** End Class ASg1Monster1 Function OnSeePawn ********************************************

// ********** Begin Class ASg1Monster1 *************************************************************
void ASg1Monster1::StaticRegisterNativesASg1Monster1()
{
	UClass* Class = ASg1Monster1::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "OnHearNoise", &ASg1Monster1::execOnHearNoise },
		{ "OnSeePawn", &ASg1Monster1::execOnSeePawn },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
FClassRegistrationInfo Z_Registration_Info_UClass_ASg1Monster1;
UClass* ASg1Monster1::GetPrivateStaticClass()
{
	using TClass = ASg1Monster1;
	if (!Z_Registration_Info_UClass_ASg1Monster1.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("Sg1Monster1"),
			Z_Registration_Info_UClass_ASg1Monster1.InnerSingleton,
			StaticRegisterNativesASg1Monster1,
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
	return Z_Registration_Info_UClass_ASg1Monster1.InnerSingleton;
}
UClass* Z_Construct_UClass_ASg1Monster1_NoRegister()
{
	return ASg1Monster1::GetPrivateStaticClass();
}
struct Z_Construct_UClass_ASg1Monster1_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "Sg1Monster1.h" },
		{ "ModuleRelativePath", "Sg1Monster1.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PawnSensingComp_MetaData[] = {
		{ "Category", "AI" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Sg1Monster1.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CurrentState_MetaData[] = {
		{ "Category", "State" },
		{ "ModuleRelativePath", "Sg1Monster1.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AttackRange_MetaData[] = {
		{ "Category", "State" },
		{ "ModuleRelativePath", "Sg1Monster1.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ChaseSpeed_MetaData[] = {
		{ "Category", "State" },
		{ "ModuleRelativePath", "Sg1Monster1.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PatrolSpeed_MetaData[] = {
		{ "Category", "State" },
		{ "ModuleRelativePath", "Sg1Monster1.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PatrolRadius_MetaData[] = {
		{ "Category", "AI|Patrol" },
		{ "ModuleRelativePath", "Sg1Monster1.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PatrolWaitTime_MetaData[] = {
		{ "Category", "AI|Patrol" },
		{ "ModuleRelativePath", "Sg1Monster1.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_HomeLocation_MetaData[] = {
		{ "Category", "AI" },
		{ "ModuleRelativePath", "Sg1Monster1.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PatrolTargetLocation_MetaData[] = {
		{ "Category", "AI" },
		{ "ModuleRelativePath", "Sg1Monster1.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TargetPawn_MetaData[] = {
		{ "Category", "AI" },
		{ "ModuleRelativePath", "Sg1Monster1.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ForgetTime_MetaData[] = {
		{ "Category", "AI" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// \xed\x94\x8c\xeb\xa0\x88\xec\x9d\xb4\xec\x96\xb4\xeb\xa5\xbc \xec\x9e\x8a\xec\x96\xb4\xeb\xb2\x84\xeb\xa6\xac\xeb\x8a\x94 \xeb\x8d\xb0 \xea\xb1\xb8\xeb\xa6\xac\xeb\x8a\x94 \xec\x8b\x9c\xea\xb0\x84\xec\x9e\x85\xeb\x8b\x88\xeb\x8b\xa4.\n" },
#endif
		{ "ModuleRelativePath", "Sg1Monster1.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "\xed\x94\x8c\xeb\xa0\x88\xec\x9d\xb4\xec\x96\xb4\xeb\xa5\xbc \xec\x9e\x8a\xec\x96\xb4\xeb\xb2\x84\xeb\xa6\xac\xeb\x8a\x94 \xeb\x8d\xb0 \xea\xb1\xb8\xeb\xa6\xac\xeb\x8a\x94 \xec\x8b\x9c\xea\xb0\x84\xec\x9e\x85\xeb\x8b\x88\xeb\x8b\xa4." },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PawnSensingComp;
	static const UECodeGen_Private::FBytePropertyParams NewProp_CurrentState_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_CurrentState;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_AttackRange;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_ChaseSpeed;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_PatrolSpeed;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_PatrolRadius;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_PatrolWaitTime;
	static const UECodeGen_Private::FStructPropertyParams NewProp_HomeLocation;
	static const UECodeGen_Private::FStructPropertyParams NewProp_PatrolTargetLocation;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_TargetPawn;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_ForgetTime;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_ASg1Monster1_OnHearNoise, "OnHearNoise" }, // 3437796279
		{ &Z_Construct_UFunction_ASg1Monster1_OnSeePawn, "OnSeePawn" }, // 3338802821
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASg1Monster1>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASg1Monster1_Statics::NewProp_PawnSensingComp = { "PawnSensingComp", nullptr, (EPropertyFlags)0x00100000000a0009, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASg1Monster1, PawnSensingComp), Z_Construct_UClass_UPawnSensingComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PawnSensingComp_MetaData), NewProp_PawnSensingComp_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_ASg1Monster1_Statics::NewProp_CurrentState_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_ASg1Monster1_Statics::NewProp_CurrentState = { "CurrentState", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASg1Monster1, CurrentState), Z_Construct_UEnum_Eclipse_EMonsterState, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CurrentState_MetaData), NewProp_CurrentState_MetaData) }; // 2430945731
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_ASg1Monster1_Statics::NewProp_AttackRange = { "AttackRange", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASg1Monster1, AttackRange), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AttackRange_MetaData), NewProp_AttackRange_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_ASg1Monster1_Statics::NewProp_ChaseSpeed = { "ChaseSpeed", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASg1Monster1, ChaseSpeed), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ChaseSpeed_MetaData), NewProp_ChaseSpeed_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_ASg1Monster1_Statics::NewProp_PatrolSpeed = { "PatrolSpeed", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASg1Monster1, PatrolSpeed), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PatrolSpeed_MetaData), NewProp_PatrolSpeed_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_ASg1Monster1_Statics::NewProp_PatrolRadius = { "PatrolRadius", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASg1Monster1, PatrolRadius), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PatrolRadius_MetaData), NewProp_PatrolRadius_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_ASg1Monster1_Statics::NewProp_PatrolWaitTime = { "PatrolWaitTime", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASg1Monster1, PatrolWaitTime), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PatrolWaitTime_MetaData), NewProp_PatrolWaitTime_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_ASg1Monster1_Statics::NewProp_HomeLocation = { "HomeLocation", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASg1Monster1, HomeLocation), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_HomeLocation_MetaData), NewProp_HomeLocation_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_ASg1Monster1_Statics::NewProp_PatrolTargetLocation = { "PatrolTargetLocation", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASg1Monster1, PatrolTargetLocation), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PatrolTargetLocation_MetaData), NewProp_PatrolTargetLocation_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASg1Monster1_Statics::NewProp_TargetPawn = { "TargetPawn", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASg1Monster1, TargetPawn), Z_Construct_UClass_APlayerCharacter_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TargetPawn_MetaData), NewProp_TargetPawn_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_ASg1Monster1_Statics::NewProp_ForgetTime = { "ForgetTime", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASg1Monster1, ForgetTime), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ForgetTime_MetaData), NewProp_ForgetTime_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_ASg1Monster1_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASg1Monster1_Statics::NewProp_PawnSensingComp,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASg1Monster1_Statics::NewProp_CurrentState_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASg1Monster1_Statics::NewProp_CurrentState,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASg1Monster1_Statics::NewProp_AttackRange,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASg1Monster1_Statics::NewProp_ChaseSpeed,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASg1Monster1_Statics::NewProp_PatrolSpeed,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASg1Monster1_Statics::NewProp_PatrolRadius,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASg1Monster1_Statics::NewProp_PatrolWaitTime,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASg1Monster1_Statics::NewProp_HomeLocation,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASg1Monster1_Statics::NewProp_PatrolTargetLocation,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASg1Monster1_Statics::NewProp_TargetPawn,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASg1Monster1_Statics::NewProp_ForgetTime,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ASg1Monster1_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_ASg1Monster1_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_ACharacter,
	(UObject* (*)())Z_Construct_UPackage__Script_Eclipse,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ASg1Monster1_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ASg1Monster1_Statics::ClassParams = {
	&ASg1Monster1::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_ASg1Monster1_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_ASg1Monster1_Statics::PropPointers),
	0,
	0x009001A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ASg1Monster1_Statics::Class_MetaDataParams), Z_Construct_UClass_ASg1Monster1_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ASg1Monster1()
{
	if (!Z_Registration_Info_UClass_ASg1Monster1.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ASg1Monster1.OuterSingleton, Z_Construct_UClass_ASg1Monster1_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ASg1Monster1.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ASg1Monster1);
ASg1Monster1::~ASg1Monster1() {}
// ********** End Class ASg1Monster1 ***************************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_Sg1Monster1_h__Script_Eclipse_Statics
{
	static constexpr FEnumRegisterCompiledInInfo EnumInfo[] = {
		{ EMonsterState_StaticEnum, TEXT("EMonsterState"), &Z_Registration_Info_UEnum_EMonsterState, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 2430945731U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ASg1Monster1, ASg1Monster1::StaticClass, TEXT("ASg1Monster1"), &Z_Registration_Info_UClass_ASg1Monster1, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ASg1Monster1), 597664487U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_Sg1Monster1_h__Script_Eclipse_3949218544(TEXT("/Script/Eclipse"),
	Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_Sg1Monster1_h__Script_Eclipse_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_Sg1Monster1_h__Script_Eclipse_Statics::ClassInfo),
	nullptr, 0,
	Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_Sg1Monster1_h__Script_Eclipse_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_Sg1Monster1_h__Script_Eclipse_Statics::EnumInfo));
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
