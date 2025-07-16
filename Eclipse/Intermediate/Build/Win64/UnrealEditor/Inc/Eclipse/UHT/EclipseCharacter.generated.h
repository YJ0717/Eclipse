// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "EclipseCharacter.h"

#ifdef ECLIPSE_EclipseCharacter_generated_h
#error "EclipseCharacter.generated.h already included, missing '#pragma once' in EclipseCharacter.h"
#endif
#define ECLIPSE_EclipseCharacter_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class AEclipseCharacter ********************************************************
#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseCharacter_h_24_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execDoJumpEnd); \
	DECLARE_FUNCTION(execDoJumpStart); \
	DECLARE_FUNCTION(execDoLook); \
	DECLARE_FUNCTION(execDoMove);


ECLIPSE_API UClass* Z_Construct_UClass_AEclipseCharacter_NoRegister();

#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseCharacter_h_24_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAEclipseCharacter(); \
	friend struct Z_Construct_UClass_AEclipseCharacter_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend ECLIPSE_API UClass* Z_Construct_UClass_AEclipseCharacter_NoRegister(); \
public: \
	DECLARE_CLASS2(AEclipseCharacter, ACharacter, COMPILED_IN_FLAGS(CLASS_Abstract | CLASS_Config), CASTCLASS_None, TEXT("/Script/Eclipse"), Z_Construct_UClass_AEclipseCharacter_NoRegister) \
	DECLARE_SERIALIZER(AEclipseCharacter)


#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseCharacter_h_24_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	AEclipseCharacter(AEclipseCharacter&&) = delete; \
	AEclipseCharacter(const AEclipseCharacter&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AEclipseCharacter); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AEclipseCharacter); \
	DEFINE_ABSTRACT_DEFAULT_CONSTRUCTOR_CALL(AEclipseCharacter) \
	NO_API virtual ~AEclipseCharacter();


#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseCharacter_h_21_PROLOG
#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseCharacter_h_24_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseCharacter_h_24_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseCharacter_h_24_INCLASS_NO_PURE_DECLS \
	FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseCharacter_h_24_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class AEclipseCharacter;

// ********** End Class AEclipseCharacter **********************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseCharacter_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
