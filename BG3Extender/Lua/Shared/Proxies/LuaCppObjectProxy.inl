#include <Lua/Shared/Proxies/LuaCppObjectProxy.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(lua)

struct CppObjectProxyHelpers
{
	static int Next(lua_State* L, GenericPropertyMap const& pm, void* object, LifetimeHandle const& lifetime, FixedString const& key)
	{
		if (!key) {
			if (!pm.Properties.empty()) {
				StackCheck _(L, 2);
				auto it = pm.Properties.begin();
				push(L, it->first);
				if (pm.GetRawProperty(L, lifetime, object, it->first) != PropertyOperationResult::Success) {
					push(L, nullptr);
				}

				return 2;
			}
		} else {
			auto it = pm.Properties.find(key);
			if (it != pm.Properties.end()) {
				++it;
				if (it != pm.Properties.end()) {
					StackCheck _(L, 2);
					push(L, it->first);
					if (pm.GetRawProperty(L, lifetime, object, it->first) != PropertyOperationResult::Success) {
						push(L, nullptr);
					}

					return 2;
				}
			}
		}

		return 0;
	}
};

CppMetatableManager::CppMetatableManager()
{
	metatables_.resize((int)MetatableTag::Max + 1);
}

void CppMetatableManager::RegisterMetatable(MetatableTag tag, CMetatable* mt)
{
	assert(tag <= MetatableTag::Max);
	metatables_[(int)tag] = mt;
}

CMetatable* CppMetatableManager::GetMetatable(MetatableTag tag)
{
	assert(tag <= MetatableTag::Max);
	return metatables_[(int)tag];
}

CppMetatableManager& CppMetatableManager::FromLua(lua_State* L)
{
	return State::FromLua(L)->GetMetatableManager();
}


void* ObjectProxy::GetRaw(lua_State* L, int index, GenericPropertyMap const& pm)
{
	CppObjectMetadata meta;
	lua_get_cppobject(L, index, meta);

	if (pm.RegistryIndex != meta.PropertyMapTag) {
		luaL_error(L, "Argument %d: Expected object of type '%s', got '%s'", index,
			pm.Name.GetString(), LuaGetPropertyMap(meta.PropertyMapTag).Name.GetString());
	}

	if (!meta.Lifetime.IsAlive(L)) {
		luaL_error(L, "Attempted to fetch '%s' whose lifetime has expired", pm.Name.GetString());
	}

	return meta.Ptr;
}

GenericPropertyMap& LightObjectProxyByRefMetatable::GetPropertyMap(CppObjectMetadata const& meta)
{
	assert(meta.MetatableTag == MetaTag);
	return LuaGetPropertyMap(meta.PropertyMapTag);
}


void* LightObjectProxyByRefMetatable::TryGetGeneric(lua_State* L, int index, int propertyMapIndex)
{
	CppObjectMetadata meta;
	if (lua_try_get_cppobject(L, index, MetaTag, meta)) {
		auto& pm = LuaGetPropertyMap(meta.PropertyMapTag);
		if (pm.IsA(meta.PropertyMapTag) && meta.Lifetime.IsAlive(L)) {
			return meta.Ptr;
		}
	}

	return nullptr;
}

void* LightObjectProxyByRefMetatable::GetGeneric(lua_State* L, int index, int propertyMapIndex)
{
	CppObjectMetadata meta;
	if (lua_try_get_cppobject(L, index, meta)) {
		auto& pm = LuaGetPropertyMap(meta.PropertyMapTag);
		if (pm.IsA(propertyMapIndex)) {
			if (!meta.Lifetime.IsAlive(L)) {
				luaL_error(L, "Attempted to fetch '%s' whose lifetime has expired", GetTypeName(L, meta));
				return 0;
			}

			return meta.Ptr;
		} else {
			luaL_error(L, "Argument %d: Expected object of type '%s', got '%s'", index,
				LuaGetPropertyMap(propertyMapIndex).Name.GetString(),
				pm.Name.GetString());
			return nullptr;
		}
	} else {
		luaL_error(L, "Argument %d: Expected object of type '%s', got '%s'", index,
			LuaGetPropertyMap(propertyMapIndex).Name.GetString(),
			lua_typename(L, lua_type(L, index)));
		return nullptr;
	}
}

int LightObjectProxyByRefMetatable::Index(lua_State* L, CppObjectMetadata& self)
{
	auto pm = gExtender->GetPropertyMapManager().GetPropertyMap(self.PropertyMapTag);
	auto prop = get<FixedString>(L, 2);
	auto result = pm->GetRawProperty(L, self.Lifetime, self.Ptr, prop);
	switch (result) {
	case PropertyOperationResult::Success:
		break;

	case PropertyOperationResult::NoSuchProperty:
		luaL_error(L, "Property does not exist: %s::%s - property does not exist", GetTypeName(L, self), prop.GetString());
		push(L, nullptr);
		break;

	case PropertyOperationResult::Unknown:
	default:
		luaL_error(L, "Cannot get property %s::%s - unknown error", GetTypeName(L, self), prop.GetString());
		push(L, nullptr);
		break;
	}

	return 1;
}

int LightObjectProxyByRefMetatable::NewIndex(lua_State* L, CppObjectMetadata& self)
{
	auto pm = gExtender->GetPropertyMapManager().GetPropertyMap(self.PropertyMapTag);
	auto prop = get<FixedString>(L, 2);
	auto result = pm->SetRawProperty(L, self.Ptr, prop, 3);
	switch (result) {
	case PropertyOperationResult::Success:
		break;

	case PropertyOperationResult::NoSuchProperty:
		luaL_error(L, "Cannot set property %s::%s - property does not exist", GetTypeName(L, self), prop.GetString());
		break;

	case PropertyOperationResult::ReadOnly:
		luaL_error(L, "Cannot set property %s::%s - property is read-only", GetTypeName(L, self), prop.GetString());
		break;

	case PropertyOperationResult::UnsupportedType:
		luaL_error(L, "Cannot set property %s::%s - cannot write properties of this type", GetTypeName(L, self), prop.GetString());
		break;

	case PropertyOperationResult::Unknown:
	default:
		luaL_error(L, "Cannot set property %s::%s - unknown error", GetTypeName(L, self), prop.GetString());
		break;
	}

	return 0;
}

int LightObjectProxyByRefMetatable::ToString(lua_State* L, CppObjectMetadata& self)
{
	char entityName[200];
	if (self.Lifetime.IsAlive(L)) {
		_snprintf_s(entityName, std::size(entityName) - 1, "%s (%p)", GetTypeName(L, self), self.Ptr);
	} else {
		_snprintf_s(entityName, std::size(entityName) - 1, "%s (%p, DEAD REFERENCE)", GetTypeName(L, self), self.Ptr);
	}

	push(L, entityName);
	return 1;
}

bool LightObjectProxyByRefMetatable::IsEqual(lua_State* L, CppObjectMetadata& self, CppObjectMetadata& other)
{
	return self.Ptr == other.Ptr && self.PropertyMapTag == other.PropertyMapTag;
}

int LightObjectProxyByRefMetatable::Next(lua_State* L, CppObjectMetadata& self)
{
	auto pm = gExtender->GetPropertyMapManager().GetPropertyMap(self.PropertyMapTag);
	if (lua_type(L, 2) == LUA_TNIL) {
		return CppObjectProxyHelpers::Next(L, *pm, self.Ptr, self.Lifetime, FixedString{});
	} else {
		auto key = get<FixedString>(L, 2);
		return CppObjectProxyHelpers::Next(L, *pm, self.Ptr, self.Lifetime, key);
	}
}

char const* LightObjectProxyByRefMetatable::GetTypeName(lua_State* L, CppObjectMetadata& self)
{
	auto pm = gExtender->GetPropertyMapManager().GetPropertyMap(self.PropertyMapTag);
	return pm->Name.GetString();
}

END_NS()
