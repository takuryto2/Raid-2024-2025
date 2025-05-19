// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Raid_2024_2025/Private/DefaultPlayer.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeDefaultPlayer() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_APawn();
RAID_2024_2025_API UClass* Z_Construct_UClass_ADefaultPlayer();
RAID_2024_2025_API UClass* Z_Construct_UClass_ADefaultPlayer_NoRegister();
UPackage* Z_Construct_UPackage__Script_Raid_2024_2025();
// End Cross Module References

// Begin Class ADefaultPlayer
void ADefaultPlayer::StaticRegisterNativesADefaultPlayer()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ADefaultPlayer);
UClass* Z_Construct_UClass_ADefaultPlayer_NoRegister()
{
	return ADefaultPlayer::StaticClass();
}
struct Z_Construct_UClass_ADefaultPlayer_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "DefaultPlayer.h" },
		{ "ModuleRelativePath", "Private/DefaultPlayer.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ADefaultPlayer>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_ADefaultPlayer_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_APawn,
	(UObject* (*)())Z_Construct_UPackage__Script_Raid_2024_2025,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ADefaultPlayer_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ADefaultPlayer_Statics::ClassParams = {
	&ADefaultPlayer::StaticClass,
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
	0x008000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ADefaultPlayer_Statics::Class_MetaDataParams), Z_Construct_UClass_ADefaultPlayer_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ADefaultPlayer()
{
	if (!Z_Registration_Info_UClass_ADefaultPlayer.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ADefaultPlayer.OuterSingleton, Z_Construct_UClass_ADefaultPlayer_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ADefaultPlayer.OuterSingleton;
}
template<> RAID_2024_2025_API UClass* StaticClass<ADefaultPlayer>()
{
	return ADefaultPlayer::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ADefaultPlayer);
ADefaultPlayer::~ADefaultPlayer() {}
// End Class ADefaultPlayer

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_louis_Documents_GitHub_Raid_2024_2025_Raid_2024_2025_Source_Raid_2024_2025_Private_DefaultPlayer_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ADefaultPlayer, ADefaultPlayer::StaticClass, TEXT("ADefaultPlayer"), &Z_Registration_Info_UClass_ADefaultPlayer, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ADefaultPlayer), 743304577U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_louis_Documents_GitHub_Raid_2024_2025_Raid_2024_2025_Source_Raid_2024_2025_Private_DefaultPlayer_h_189293297(TEXT("/Script/Raid_2024_2025"),
	Z_CompiledInDeferFile_FID_Users_louis_Documents_GitHub_Raid_2024_2025_Raid_2024_2025_Source_Raid_2024_2025_Private_DefaultPlayer_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_louis_Documents_GitHub_Raid_2024_2025_Raid_2024_2025_Source_Raid_2024_2025_Private_DefaultPlayer_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
