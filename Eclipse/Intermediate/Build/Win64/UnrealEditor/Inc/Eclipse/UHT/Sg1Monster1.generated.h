// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Sg1Monster1.h"

#ifdef ECLIPSE_Sg1Monster1_generated_h
#error "Sg1Monster1.generated.h already included, missing '#pragma once' in Sg1Monster1.h"
#endif
#define ECLIPSE_Sg1Monster1_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

class APawn;

// ********** Begin Class ASg1Monster1 *************************************************************
#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_Sg1Monster1_h_21_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execOnHearNoise); \
	DECLARE_FUNCTION(execOnSeePawn);


ECLIPSE_API UClass* Z_Construct_UClass_ASg1Monster1_NoRegister();

#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_Sg1Monster1_h_21_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesASg1Monster1(); \
	friend struct Z_Construct_UClass_ASg1Monster1_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend ECLIPSE_API UClass* Z_Construct_UClass_ASg1Monster1_NoRegister(); \
public: \
	DECLARE_CLASS2(ASg1Monster1, ACharacter, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/Eclipse"), Z_Construct_UClass_ASg1Monster1_NoRegister) \
	DECLARE_SERIALIZER(ASg1Monster1)


#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_Sg1Monster1_h_21_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	ASg1Monster1(ASg1Monster1&&) = delete; \
	ASg1Monster1(const ASg1Monster1&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ASg1Monster1); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ASg1Monster1); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(ASg1Monster1) \
	NO_API virtual ~ASg1Monster1();


#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_Sg1Monster1_h_18_PROLOG
#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_Sg1Monster1_h_21_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_Sg1Monster1_h_21_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_Sg1Monster1_h_21_INCLASS_NO_PURE_DECLS \
	FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_Sg1Monster1_h_21_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class ASg1Monster1;

// ********** End Class ASg1Monster1 ***************************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_Sg1Monster1_h

// ********** Begin Enum EMonsterState *************************************************************
#define FOREACH_ENUM_EMONSTERSTATE(op) \
	op(EMonsterState::Patrol) \
	op(EMonsterState::Chase) \
	op(EMonsterState::Attack) \
	op(EMonsterState::Dead) 

enum class EMonsterState : uint8;
template<> struct TIsUEnumClass<EMonsterState> { enum { Value = true }; };
template<> ECLIPSE_API UEnum* StaticEnum<EMonsterState>();
// ********** End Enum EMonsterState ***************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
