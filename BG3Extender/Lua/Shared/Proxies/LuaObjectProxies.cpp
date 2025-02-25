// Shared container for all property lists

#include <stdafx.h>

#include <Lua/Shared/Proxies/PropertyMapDependencies.h>
#include <Lua/Shared/Proxies/PolymorphicPush.inl>

BEGIN_NS(lua)

void CopyRawProperties(GenericPropertyMap const& base, GenericPropertyMap& child)
{
	for (auto const& prop : base.Properties) {
		child.AddRawProperty(prop.first.GetString(), prop.second.Get, prop.second.Set, 
			prop.second.Serialize, prop.second.Offset, prop.second.Flag);
	}
	
	for (auto const& prop : base.Validators) {
		child.AddRawValidator(prop.Name.GetString(), prop.Validate, prop.Offset, prop.Flag);
	}

	for (auto const& parent : base.Parents) {
		child.Parents.push_back(parent);
	}

	for (auto const& parent : base.ParentRegistryIndices) {
		child.ParentRegistryIndices.push_back(parent);
	}

	child.Parents.push_back(base.Name);
	child.ParentRegistryIndices.push_back(base.RegistryIndex);
}

template <class T>
void AddBitmaskProperty(GenericPropertyMap& pm, std::size_t offset)
{
	for (auto const& label : EnumInfo<T>::Store->Values) {
		pm.AddRawProperty(label.Key.GetString(),
			&(GenericGetOffsetBitmaskFlag<std::underlying_type_t<T>>),
			&(GenericSetOffsetBitmaskFlag<std::underlying_type_t<T>>),
			nullptr,
			nullptr,
			offset,
			(uint64_t)label.Value
		);
	}
}

template <class TCls, class TEnum>
void AddBitmaskProperty(LuaPropertyMap<TCls>& pm, std::size_t offset,
	typename LuaPropertyMap<TCls>::PropertyAccessors::Getter* getter,
	typename LuaPropertyMap<TCls>::PropertyAccessors::Setter* setter)
{
	for (auto const& label : EnumInfo<TEnum>::Store->Values) {
		pm.AddProperty(label.Key.GetString(),
			getter,
			setter,
			nullptr,
			nullptr,
			offset,
			(uint64_t)label.Value
		);
	}
}


// Lua property map and object proxy template specialization declarations

#define BEGIN_CLS(name) LuaPropertyMap<name> StaticLuaPropertyMap<name>::PropertyMap;
#define BEGIN_CLS_TN(name, typeName) LuaPropertyMap<name> StaticLuaPropertyMap<name>::PropertyMap;

#define END_CLS()
#define INHERIT(base)
#define P(prop)
#define P_RO(prop)
#define P_BITMASK(prop)
#define P_BITMASK_GETTER_SETTER(prop, getter, setter)
#define PN(name, prop)
#define PN_RO(name, prop)
#define P_GETTER(prop, fun)
#define P_GETTER_SETTER(prop, getter, setter)
#define P_FUN(prop, fun)
#define P_FALLBACK(getter, setter)

#include <GameDefinitions/PropertyMaps/AllPropertyMaps.inl>

#undef BEGIN_CLS
#undef BEGIN_CLS_TN
#undef END_CLS
#undef INHERIT
#undef P
#undef P_RO
#undef P_BITMASK
#undef P_BITMASK_GETTER_SETTER
#undef PN
#undef PN_RO
#undef P_GETTER
#undef P_GETTER_SETTER
#undef P_FUN
#undef P_FALLBACK

// Property registrations

	void InitObjectProxyPropertyMaps()
	{
		static bool initialized{ false };
		if (initialized) return;

#define GENERATING_PROPMAP

#define BEGIN_CLS(cls) ([]() { \
	using PM = StaticLuaPropertyMap<cls>; \
	auto& pm = StaticLuaPropertyMap<cls>::PropertyMap; \
	pm.Init(gExtender->GetPropertyMapManager().RegisterPropertyMap(&pm)); \
	pm.BindConstructors(); \
	pm.Name = FixedString(#cls);

#define BEGIN_CLS_TN(cls, typeName) ([]() { \
	using PM = StaticLuaPropertyMap<cls>; \
	auto& pm = StaticLuaPropertyMap<cls>::PropertyMap; \
	pm.Init(gExtender->GetPropertyMapManager().RegisterPropertyMap(&pm)); \
	pm.BindConstructors(); \
	pm.Name = FixedString(#typeName);

#define END_CLS() pm.Finish(); \
	})();

#define INHERIT(base) { \
		auto& basePm = StaticLuaPropertyMap<base>::PropertyMap; \
		InheritProperties(basePm, pm); \
	}

#define P(prop) \
	pm.AddRawProperty(#prop, \
		&(GenericGetOffsetProperty<decltype(PM::ObjectType::prop)>), \
		&(GenericSetOffsetProperty<decltype(PM::ObjectType::prop)>), \
		&(GenericValidateOffsetProperty<decltype(PM::ObjectType::prop)>), \
		&(GenericSerializeOffsetProperty<decltype(PM::ObjectType::prop)>), \
		offsetof(PM::ObjectType, prop) \
	);

#define P_RO(prop) \
	pm.AddRawProperty(#prop, \
		&(GenericGetOffsetProperty<decltype(PM::ObjectType::prop)>), \
		&GenericSetReadOnlyProperty, \
		&(GenericValidateOffsetProperty<decltype(PM::ObjectType::prop)>), \
		&(GenericSerializeOffsetProperty<decltype(PM::ObjectType::prop)>), \
		offsetof(PM::ObjectType, prop) \
	);

#define P_BITMASK(prop) AddBitmaskProperty<decltype(PM::ObjectType::prop)>(pm, offsetof(PM::ObjectType, prop));

#define P_BITMASK_GETTER_SETTER(prop, getter, setter) \
	AddBitmaskProperty<PM::ObjectType, decltype(PM::ObjectType::prop)>(pm, offsetof(PM::ObjectType, prop), \
		[](lua_State* L, LifetimeHandle const& lifetime, PM::ObjectType* obj, std::size_t offset, uint64_t flag) { \
			CallFlagGetter(L, obj, &PM::ObjectType::getter, (decltype(PM::ObjectType::prop))flag); \
			return PropertyOperationResult::Success; \
		}, \
		[](lua_State* L, LifetimeHandle const& lifetime, PM::ObjectType* obj, int index, std::size_t offset, uint64_t flag) { \
			CallFlagSetter(L, obj, index, &PM::ObjectType::setter, (decltype(PM::ObjectType::prop))flag); \
			return PropertyOperationResult::Success; \
		});

#define PN(name, prop) \
	pm.AddRawProperty(#name, \
		&(GenericGetOffsetProperty<decltype(PM::ObjectType::prop)>), \
		&(GenericSetOffsetProperty<decltype(PM::ObjectType::prop)>), \
		&(GenericValidateOffsetProperty<decltype(PM::ObjectType::prop)>), \
		&(GenericSerializeOffsetProperty<decltype(PM::ObjectType::prop)>), \
		offsetof(PM::ObjectType, prop) \
	);

#define PN_RO(name, prop) \
	pm.AddRawProperty(#name, \
		&(GenericGetOffsetProperty<decltype(PM::ObjectType::prop)>), \
		&GenericSetReadOnlyProperty, \
		&(GenericValidateOffsetProperty<decltype(PM::ObjectType::prop)>), \
		&(GenericSerializeOffsetProperty<decltype(PM::ObjectType::prop)>), \
		offsetof(PM::ObjectType, prop) \
	);

#define P_GETTER(name, fun) \
	pm.AddProperty(#name, \
		[](lua_State* L, LifetimeHandle const& lifetime, PM::ObjectType* obj, std::size_t offset, uint64_t flag) { \
			CallGetter(L, obj, &PM::ObjectType::fun); \
			return PropertyOperationResult::Success; \
		}, \
		(PM::TPropertyMap::PropertyAccessors::Setter*)&GenericSetNonWriteableProperty, nullptr, nullptr, 0, 0 \
	);

#define P_GETTER_SETTER(prop, getter, setter) \
	pm.AddProperty(#prop, \
		[](lua_State* L, LifetimeHandle const& lifetime, PM::ObjectType* obj, std::size_t offset, uint64_t flag) { \
			CallGetter(L, obj, &PM::ObjectType::getter); \
			return PropertyOperationResult::Success; \
		}, \
		[](lua_State* L, LifetimeHandle const& lifetime, PM::ObjectType* obj, int index, std::size_t offset, uint64_t flag) { \
			CallSetter(L, obj, index, &PM::ObjectType::setter); \
			return PropertyOperationResult::Success; \
		}, \
		nullptr, nullptr, 0, 0 \
	);

// FIXME - avoid generating a separate push function for each closure
#define P_FUN(name, fun) \
	pm.AddProperty(#name, \
		[](lua_State* L, LifetimeHandle const& lifetime, PM::ObjectType* obj, std::size_t offset, uint64_t flag) { \
			lua_pushcfunction(L, [](lua_State* L) -> int { \
				return CallMethod(L, &PM::ObjectType::fun); \
			}); \
			return PropertyOperationResult::Success; \
		}, \
		(PM::TPropertyMap::PropertyAccessors::Setter*)&GenericSetNonWriteableProperty, \
		nullptr, \
		(PM::TPropertyMap::PropertyAccessors::Serializer*)&GenericNullSerializeProperty, \
		0, 0 \
	);

#define P_FALLBACK(getter, setter) pm.SetFallback(getter, setter);

#include <GameDefinitions/PropertyMaps/AllPropertyMaps.inl>

#undef GENERATING_PROPMAP
#undef BEGIN_CLS
#undef BEGIN_CLS_TN
#undef END_CLS
#undef INHERIT
#undef P
#undef P_RO
#undef P_BITMASK
#undef P_BITMASK_GETTER_SETTER
#undef PN
#undef PN_RO
#undef P_GETTER
#undef P_GETTER_SETTER
#undef P_FUN
#undef P_FALLBACK

		initialized = true;
	}
END_NS()
