// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "EclipsePlayerController.h"

#ifdef ECLIPSE_EclipsePlayerController_generated_h
#error "EclipsePlayerController.generated.h already included, missing '#pragma once' in EclipsePlayerController.h"
#endif
#define ECLIPSE_EclipsePlayerController_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class AEclipsePlayerController *************************************************
ECLIPSE_API UClass* Z_Construct_UClass_AEclipsePlayerController_NoRegister();

#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipsePlayerController_h_18_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAEclipsePlayerController(); \
	friend struct Z_Construct_UClass_AEclipsePlayerController_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend ECLIPSE_API UClass* Z_Construct_UClass_AEclipsePlayerController_NoRegister(); \
public: \
	DECLARE_CLASS2(AEclipsePlayerController, APlayerController, COMPILED_IN_FLAGS(CLASS_Abstract | CLASS_Config), CASTCLASS_None, TEXT("/Script/Eclipse"), Z_Construct_UClass_AEclipsePlayerController_NoRegister) \
	DECLARE_SERIALIZER(AEclipsePlayerController)


#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipsePlayerController_h_18_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AEclipsePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	AEclipsePlayerController(AEclipsePlayerController&&) = delete; \
	AEclipsePlayerController(const AEclipsePlayerController&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AEclipsePlayerController); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AEclipsePlayerController); \
	DEFINE_ABSTRACT_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AEclipsePlayerController) \
	NO_API virtual ~AEclipsePlayerController();


#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipsePlayerController_h_15_PROLOG
#define FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipsePlayerController_h_18_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipsePlayerController_h_18_INCLASS_NO_PURE_DECLS \
	FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipsePlayerController_h_18_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class AEclipsePlayerController;

// ********** End Class AEclipsePlayerController ***************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_Admin_Desktop_Eclipse_Eclipse_Source_Eclipse_EclipsePlayerController_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
