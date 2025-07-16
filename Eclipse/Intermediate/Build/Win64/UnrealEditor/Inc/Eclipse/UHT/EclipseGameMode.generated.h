// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "EclipseGameMode.h"

#ifdef ECLIPSE_EclipseGameMode_generated_h
#error "EclipseGameMode.generated.h already included, missing '#pragma once' in EclipseGameMode.h"
#endif
#define ECLIPSE_EclipseGameMode_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class AEclipseGameMode *********************************************************
ECLIPSE_API UClass* Z_Construct_UClass_AEclipseGameMode_NoRegister();

#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseGameMode_h_15_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAEclipseGameMode(); \
	friend struct Z_Construct_UClass_AEclipseGameMode_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend ECLIPSE_API UClass* Z_Construct_UClass_AEclipseGameMode_NoRegister(); \
public: \
	DECLARE_CLASS2(AEclipseGameMode, AGameModeBase, COMPILED_IN_FLAGS(CLASS_Abstract | CLASS_Transient | CLASS_Config), CASTCLASS_None, TEXT("/Script/Eclipse"), Z_Construct_UClass_AEclipseGameMode_NoRegister) \
	DECLARE_SERIALIZER(AEclipseGameMode)


#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseGameMode_h_15_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	AEclipseGameMode(AEclipseGameMode&&) = delete; \
	AEclipseGameMode(const AEclipseGameMode&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AEclipseGameMode); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AEclipseGameMode); \
	DEFINE_ABSTRACT_DEFAULT_CONSTRUCTOR_CALL(AEclipseGameMode) \
	NO_API virtual ~AEclipseGameMode();


#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseGameMode_h_12_PROLOG
#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseGameMode_h_15_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseGameMode_h_15_INCLASS_NO_PURE_DECLS \
	FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseGameMode_h_15_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class AEclipseGameMode;

// ********** End Class AEclipseGameMode ***********************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipseGameMode_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
