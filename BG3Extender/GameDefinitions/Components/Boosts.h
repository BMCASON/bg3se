#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Misc.h>

BEGIN_SE()

struct BoostConditionComponent : public BaseComponent
{
	static constexpr ExtComponentType ComponentType = ExtComponentType::BoostCondition;
	static constexpr auto EngineClass = "eoc::BoostConditionComponent";

	int ConditionFlags;
	uint8_t field_1C;
};

struct BoostsContainerComponent : public BaseComponent
{
	static constexpr ExtComponentType ComponentType = ExtComponentType::BoostsContainer;
	static constexpr auto EngineClass = "eoc::BoostsContainerComponent";

	uint64_t field_0;
	MultiHashMap<BoostType, Array<EntityHandle>> Boosts;
};

struct BoostInfoComponent : public BaseComponent
{
	static constexpr ExtComponentType ComponentType = ExtComponentType::BoostInfo;
	static constexpr auto EngineClass = "eoc::BoostInfoComponent";

	EntityHandle BoostEntity;
	BoostType Type;
	Guid field_10;
	uint8_t field_20;
	BoostCause Cause;
	EntityHandle Owner;
	BoostParameters Params;
	Guid field_80;
};

DEFN_BOOST(ArmorClass, AC, {
	int32_t AC;
})

DEFN_BOOST(Ability, Ability, {
	AbilityId Ability;
	int32_t Value;
	int32_t field_8;
	int8_t field_C;
})

DEFN_BOOST(RollBonus, RollBonus, {
	stats::RollType RollType;
	StatsExpressionParam Amount;
	AbilityId Ability;
	SkillId Skill;
})

DEFN_BOOST(Advantage, Advantage, {
	AdvantageTypeId AdvantageType;
	AdvantageBoostType Type;
	AbilityId Ability;
	SkillId Skill;
	Array<Guid> Tags;
})

DEFN_BOOST(ActionResourceValue, ActionResource, {
	Guid ResourceUUID;
	int Amount2;
	double Amount;
	DiceSizeId DiceSize;
})

DEFN_BOOST(CriticalHit, CriticalHit, {
	CriticalHitBoostFlags Flags;
	uint8_t field_1;
	int Value;
})

DEFN_BOOST(AbilityFailedSavingThrow, AbilityFailedSavingThrow, {
	AbilityId Ability;
})

DEFN_BOOST(Resistance, Resistance, {
	DamageType DamageType;
	ResistanceBoostFlags ResistanceFlags;
	bool IsResistantToAll;
})

DEFN_BOOST(WeaponDamageResistance, WeaponDamageResistance, {
	Array<DamageType> DamageTypes;
})

DEFN_BOOST(ProficiencyBonusOverride, ProficiencyBonusOverride, {
	StatsExpressionParam Value;
})

DEFN_BOOST(JumpMaxDistanceMultiplier, JumpMaxDistanceMultiplier, {
	float Amount;
})

DEFN_BOOST(HalveWeaponDamage, HalveWeaponDamage, {
	AbilityId Ability;
})

DEFN_BOOST(UnlockSpell, UnlockSpell, {
	FixedString SpellId;
	AbilityId Ability;
	Guid SomeUUID;
	SpellChildSelectionType SpellChildSelection;
	SpellCooldownType CooldownType;
})

DEFN_BOOST(SourceAdvantage, SourceAllyAdvantageOnAttack, {
	SourceAdvantageType Type;
})

DEFN_BOOST(ProficiencyBonus, ProficiencyBonus, {
	ProficiencyBonusBoostType Type;
	AbilityId Ability;
	SkillId Skill;
})

DEFN_BOOST(Proficiency, Proficiency, {
	ProficiencyGroupFlags Flags;
})

struct IncreaseMaxHPComponent : public BaseComponent
{
	static constexpr ExtComponentType ComponentType = ExtComponentType::IncreaseMaxHPBoost;
	static constexpr auto EngineClass = "eoc::IncreaseMaxHPBoostComponent";

	std::variant<int32_t, StatsExpressionParam> Amount;
	int32_t field_30;
};

DEFN_BOOST(ActionResourceBlock, ActionResourceBlock, {
	Guid ResourceUUID;
	int IntParam;
})

DEFN_BOOST(StatusImmunity, StatusImmunity, {
	FixedString StatusID;
	Array<Guid> UnknownUUIDs;
})

struct UseBoostsComponent : public BaseComponent
{
	static constexpr ExtComponentType ComponentType = ExtComponentType::UseBoosts;
	static constexpr auto EngineClass = "eoc::UseBoostsComponent";

	Array<BoostParameters> Boosts;
};

DEFN_BOOST(CannotHarmCauseEntity, CannotHarmCauseEntity, {
	FixedString Type;
})

DEFN_BOOST(TemporaryHP, TemporaryHP, {
	StatsExpressionParam HP;
})

DEFN_BOOST(Weight, Weight, {
	int Amount;
})

DEFN_BOOST(WeightCategory, WeightCategory, {
	int Amount;
})

DEFN_BOOST(FactionOverride, FactionOverride, {
	Guid Faction;
	uint8_t field_10;
})

DEFN_BOOST(ActionResourceMultiplier, ActionResourceMultiplier, {
	Guid ResourceUUID;
	int IntParam2;
	int IntParam;
	DiceSizeId DiceSize;
})

DEFN_BOOST(Initiative, Initiative, {
	int Amount;
})

DEFN_BOOST(DarkvisionRange, DarkvisionRange, {
	float Range;
})

DEFN_BOOST(DarkvisionRangeMin, DarkvisionRangeMin, {
	float Range;
})

DEFN_BOOST(DarkvisionRangeOverride, DarkvisionRangeOverride, {
	float Range;
})

DEFN_BOOST(AddTag, Tag, {
	Guid Tag;
})

DEFN_BOOST(IgnoreDamageThresholdMin, IgnoreDamageThreshold, {
	DamageType DamageType;
	bool All;
	uint16_t Amount;
})

DEFN_BOOST(Skill, Skill, {
	SkillId Skill;
	StatsExpressionParam Amount;
})

DEFN_BOOST(WeaponDamage, WeaponDamage, {
	DamageType DamageType;
	StatsExpressionParam Amount;
	bool field_30;
})

DEFN_BOOST(NullifyAbility, NullifyAbilityScore, {
	AbilityId Ability;
})

DEFN_BOOST(Reroll, Reroll, {
	stats::RollType RollType;
	int8_t field_1;
	bool field_2;
})

DEFN_BOOST(DownedStatus, DownedStatus, {
	FixedString StatusId;
	int32_t field_4;
})

DEFN_BOOST(WeaponEnchantment, WeaponEnchantment, {
	int Value;
})

DEFN_BOOST(GuaranteedChanceRollOutcome, GuaranteedChanceRollOutcome, {
	bool field_0;
})

// FIXME - update mapping from stats!
DEFN_BOOST(Attribute, Attribute, {
	AttributeFlags AttributeFlags;
})

DEFN_BOOST(GameplayLight, GameplayLight, {
	int field_0;
	bool field_4;
	int field_8;
	uint8_t field_C;
})

DEFN_BOOST(DualWielding, DualWielding, {
	bool DualWielding;
})

DEFN_BOOST(Savant, Savant, {
	SpellSchoolId SpellSchool;
})

DEFN_BOOST(MinimumRollResult, MinimumRollResult, {
	stats::RollType RollType;
	int8_t Result;
})

DEFN_BOOST(CharacterWeaponDamage, CharacterWeaponDamage, {
	StatsExpressionParam Amount;
	DamageType DamageType;
})

DEFN_BOOST(ProjectileDeflect, ProjectileDeflect, {
	ProjectileTypeIds ProjectileTypes;
})

DEFN_BOOST(AbilityOverrideMinimum, AbilityOverrideMinimum, {
	AbilityId Ability;
	int Amount;
})

DEFN_BOOST(ACOverrideFormula, ACOverrideFormula, {
	int32_t AC;
	bool field_4;
	Array<AbilityId> AddAbilityModifiers;
})

DEFN_BOOST(FallDamageMultiplier, FallDamageMultiplier, {
	float Amount;
})

DEFN_BOOST(ActiveCharacterLight, ActiveCharacterLight, {
	FixedString LightUUID;
})

DEFN_BOOST(WeaponAttackTypeOverride, WeaponAttackTypeOverride, {
	SpellAttackType AttackType;
})

DEFN_BOOST(WeaponDamageDieOverride, WeaponDamageDieOverride, {
	DiceValues Roll;
})

DEFN_BOOST(CarryCapacityMultiplier, CarryCapacityMultiplier, {
	float Multiplier;
})

DEFN_BOOST(WeaponProperty, WeaponProperty, {
	WeaponFlags Properties;
})

DEFN_BOOST(WeaponAttackRollAbilityOverride, WeaponAttackRollAbilityOverride, {
	AbilityId Ability;
})

DEFN_BOOST(SightRangeAdditive, SightRangeAdditive, {
	float Range;
})

DEFN_BOOST(SightRangeMinimum, SightRangeMinimum, {
	float Range;
})

DEFN_BOOST(SightRangeMaximum, SightRangeMaximum, {
	float Range;
})

DEFN_BOOST(SightRangeOverride, SightRangeOverride, {
	float Range;
})

DEFN_BOOST(MovementSpeedLimit, MovementSpeedLimit, {
	uint8_t MovementType;
})

DEFN_BOOST(UnlockSpellVariant, UnlockSpellVariant, {
	STDString Spell;
	Array<spell::Modification::Variant> Modifications;
})

DEFN_BOOST(DetectCrimesBlock, DetectDisturbancesBlock, {
	bool field_0;
})

struct BlockAbilityModifierFromACComponent : public BaseComponent
{
	static constexpr ExtComponentType ComponentType = ExtComponentType::BlockAbilityModifierFromACBoost;
	static constexpr auto BoostType = BoostType::BlockAbilityModifierFromAC;
	static constexpr auto EngineClass = "eoc::BlockAbilityModifierFromACComponent";

	AbilityId Ability;
};

DEFN_BOOST(ScaleMultiplier, ScaleMultiplier, {
	float Multiplier;
})

struct DamageReductionBoostComponent : public BaseComponent
{
	static constexpr ExtComponentType ComponentType = ExtComponentType::DamageReductionBoost;
	static constexpr auto BoostType = BoostType::DamageReduction;
	static constexpr auto EngineClass = "eoc::DamageReductionBoostComponent";

	DamageType DamageType;
	std::variant<int32_t, StatsExpressionParam> Amount;
	bool Flat;
	bool Half;
};

DEFN_BOOST(ReduceCriticalAttackThreshold, ReduceCriticalAttackThreshold, {
	int32_t field_0;
	int32_t field_4;
	int64_t field_8;
})

DEFN_BOOST(PhysicalForceRangeBonus, PhysicalForceRangeBonus, {
	float field_0;
	uint8_t field_4;
})

DEFN_BOOST(ObjectSize, ObjectSize, {
	int32_t SizeCategoryAdjustment;
})

DEFN_BOOST(ObjectSizeOverride, ObjectSizeOverride, {
	uint8_t field_0;
})

DEFN_BOOST(AiArchetypeOverride, AiArchetypeOverride, {
	FixedString Archetype;
	int32_t Priority;
})

DEFN_BOOST(ExpertiseBonus, ExpertiseBonus, {
	SkillId Skill;
})

DEFN_BOOST(EntityThrowDamage, EntityThrowDamage, {
	DiceValues Roll;
	uint8_t field_C;
})

DEFN_BOOST(WeaponDamageTypeOverride, WeaponDamageTypeOverride, {
	DamageType DamageType;
})

DEFN_BOOST(MaximizeHealing, MaximizeHealing, {
	HealDirection Direction;
	stats::TargetTypeFlags TargetTypes;
})

DEFN_BOOST(DamageBonus, DamageBonus, {
	StatsExpressionParam Amount;
	DamageType DamageType;
	uint8_t field_31;
})

DEFN_BOOST(AdvanceSpells, AdvanceSpells, {
	FixedString field_0;
	int32_t field_4;
})

DEFN_BOOST(SpellResistance, SpellResistance, {
	ResistanceBoostFlags Resistance;
})

DEFN_BOOST(WeaponAttackRollBonus, WeaponAttackRollBonus, {
	StatsExpressionParam Amount;
})

DEFN_BOOST(SpellSaveDC, SpellSaveDC, {
	int32_t DC;
})

DEFN_BOOST(RedirectDamage, RedirectDamage, {
	int32_t Amount;
	DamageType DamageType1;
	DamageType DamageType2;
	bool field_6;
})

DEFN_BOOST(CanSeeThrough, CanSeeThrough, {
	bool CanSeeThrough;
})

DEFN_BOOST(CanShootThrough, CanShootThrough, {
	bool CanShootThrough;
})

DEFN_BOOST(CanWalkThrough, CanWalkThrough, {
	bool CanWalkThrough;
})

DEFN_BOOST(MonkWeaponDamageDiceOverride, MonkWeaponDamageDiceOverride, {
	FixedString DamageDice;
})

DEFN_BOOST(HorizontalFOVOverride, HorizontalFOVOverride, {
	float FOV;
})

DEFN_BOOST(CharacterUnarmedDamage, CharacterUnarmedDamage, {
	StatsExpressionParam Amount;
	DamageType DamageType;
})

DEFN_BOOST(ActionResourceReplenishTypeOverride, ActionResourceReplenishTypeOverride, {
	Guid ActionResource;
	ResourceReplenishType ReplenishType;
})

DEFN_BOOST(ActionResourcePreventReduction, ActionResourcePreventReduction, {
	Guid ActionResource;
	int32_t Amount;
})

DEFN_BOOST(AttackSpellOverride, AttackSpellOverride, {
	FixedString SpellId;
	SpellAttackTypeOverride AttackType;
})

DEFN_BOOST(Lock, Lock, {
	Guid Lock;
})

DEFN_BOOST(IgnorePointBlankDisadvantage, IgnorePointBlankDisadvantage, {
	WeaponFlags Flags;
})

DEFN_BOOST(CriticalHitExtraDice, CriticalHitExtraDice, {
	uint8_t Amount;
	SpellAttackType AttackType;
})

DEFN_BOOST(DodgeAttackRoll, DodgeAttackRoll, {
	uint8_t field_0;
	int32_t field_4;
	StatusType StatusType;
	stats::StatusGroup StatusGroup;
})

DEFN_BOOST(GameplayObscurity, GameplayObscurity, {
	float Obscurity;
})

DEFN_BOOST(MaximumRollResult, MaximumRollResult, {
	stats::RollType RollType;
	int8_t Result;
})

DEFN_BOOST(UnlockInterrupt, UnlockInterrupt, {
	FixedString Interrupt;
})

DEFN_BOOST(JumpMaxDistanceBonus, JumpMaxDistanceBonus, {
	float DistanceBonus;
})

DEFN_BOOST(ArmorAbilityModifierCapOverride, ArmorAbilityModifierCapOverride, {
	ArmorType ArmorType;
	int32_t Value;
})

DEFN_BOOST(IgnoreResistance, IgnoreResistance, {
	DamageType DamageType;
	ResistanceBoostFlags Flags;
})

DEFN_BOOST(ConcentrationIgnoreDamage, ConcentrationIgnoreDamage, {
	SpellSchoolId SpellSchool;
})

DEFN_BOOST(IgnoreLowGroundPenalty, IgnoreLowGroundPenalty, {
	stats::RollType RollType;
})

DEFN_BOOST(IgnoreSurfaceCover, IgnoreSurfaceCover, {
	SurfaceType SurfaceType;
})

END_SE()
