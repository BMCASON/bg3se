#pragma once

#include <Lua/LuaHelpers.h>
#include <Lua/Shared/LuaLifetime.h>
#include <Lua/Shared/LuaPropertyMapHelpers.h>

namespace bg3se
{
	BY_VAL_ARRAY(uint8_t);
	BY_VAL_ARRAY(int16_t);
	BY_VAL_ARRAY(uint16_t);
	BY_VAL_ARRAY(int32_t);
	BY_VAL_ARRAY(uint32_t);
	BY_VAL_ARRAY(int64_t);
	BY_VAL_ARRAY(uint64_t);
	BY_VAL_ARRAY(float);
	BY_VAL_ARRAY(double);
	BY_VAL_ARRAY(bool);
	BY_VAL_ARRAY(ObjectHandle);
	BY_VAL_ARRAY(EntityHandle);
	BY_VAL_ARRAY(EntityWorldHandle);
	BY_VAL_ARRAY(FixedString);
	BY_VAL_ARRAY(STDString);
	BY_VAL_ARRAY(STDWString);
	BY_VAL_ARRAY(UUID);
}

namespace bg3se::lua
{
	LifetimeHolder GetCurrentLifetime();

	class ArrayProxyImplBase
	{
	public:
		inline virtual ~ArrayProxyImplBase() {};
		virtual char const* GetTypeName() const = 0;
		virtual void* GetRaw() = 0;
		virtual bool GetElement(lua_State* L, unsigned arrayIndex) = 0;
		virtual bool SetElement(lua_State* L, unsigned arrayIndex, int luaIndex) = 0;
		virtual int Next(lua_State* L, int key) = 0;
		virtual unsigned Length() = 0;
		virtual bool IsA(char const* typeName) = 0;
	};


	template <class T>
	struct ArrayProxyHelpers
	{
		static char const* const TypeName;
	};


	template <class T>
	class ArrayProxyByRefImpl : public ArrayProxyImplBase
	{
	public:
		static_assert(!std::is_pointer_v<T>, "ArrayProxyByRefImpl template parameter should not be a pointer type!");

		ArrayProxyByRefImpl(LifetimeHolder const& lifetime, Array<T> * obj)
			: object_(obj), lifetime_(lifetime)
		{}
		
		~ArrayProxyByRefImpl() override
		{}

		T* Get() const
		{
			return object_;
		}

		void* GetRaw() override
		{
			return object_;
		}

		char const* GetTypeName() const override
		{
			return ObjectProxyHelpers<T>::TypeName;
		}

		bool GetElement(lua_State* L, unsigned arrayIndex) override
		{
			if (arrayIndex >= 0 && arrayIndex < (int)object_->Size()) {
				MakeObjectRef(L, lifetime_, &(*object_)[arrayIndex]);
				return true;
			} else {
				return false;
			}
		}

		bool SetElement(lua_State* L, unsigned arrayIndex, int luaIndex) override
		{
			// Appending/swapping elements to by-ref arrays not supported for now
			return false;
		}

		unsigned Length() override
		{
			return object_->Size();
		}

		int Next(lua_State* L, int key) override
		{
			if (key >= -1 && key < (int)object_->Size() - 1) {
				push(L, ++key);
				MakeObjectRef(L, lifetime_, &(*object_)[key]);
				return 2;
			} else {
				return 0;
			}
		}

		bool IsA(char const* typeName) override
		{
			return ObjectProxyHelpers<T>::IsA(typeName);
		}

	private:
		Array<T>* object_;
		LifetimeHolder lifetime_;
	};


	template <class T>
	class ArrayProxyByValImpl : public ArrayProxyImplBase
	{
	public:
		static_assert(!std::is_pointer_v<T>, "ArrayProxyByValImpl template parameter should not be a pointer type!");

		ArrayProxyByValImpl(LifetimeHolder const& lifetime, Array<T> * obj)
			: object_(obj), lifetime_(lifetime)
		{}
		
		~ArrayProxyByValImpl() override
		{}

		T* Get() const
		{
			return object_;
		}

		void* GetRaw() override
		{
			return object_;
		}

		char const* GetTypeName() const override
		{
			return ArrayProxyHelpers<T>::TypeName;
		}

		bool GetElement(lua_State* L, unsigned arrayIndex) override
		{
			if (arrayIndex >= 0 && arrayIndex < (int)object_->Size()) {
				push(L, (*object_)[arrayIndex]);
				return true;
			} else {
				return false;
			}
		}

		bool SetElement(lua_State* L, unsigned arrayIndex, int luaIndex) override
		{
			if (arrayIndex < object_->Size()) {
				lua_pushvalue(L, luaIndex);
				LuaRead(L, (*object_)[arrayIndex]);
				lua_pop(L, 1);
				return true;
			} else if (arrayIndex == object_->Size()) {
				T val;
				lua_pushvalue(L, luaIndex);
				LuaRead(L, val);
				lua_pop(L, 1);

				object_->Add(val);
				return true;
			} else {
				return false;
			}
		}

		unsigned Length() override
		{
			return object_->Size();
		}

		int Next(lua_State* L, int key) override
		{
			if (key >= -1 && key < (int)object_->Size() - 1) {
				push(L, ++key);
				push(L, (*object_)[key]);
				return 2;
			} else {
				return 0;
			}
		}

		bool IsA(char const* typeName) override
		{
			return strcmp(ArrayProxyHelpers<T>::TypeName, typeName) == 0;
		}

	private:
		Array<T>* object_;
		LifetimeHolder lifetime_;
	};


	class ArrayProxy : private Userdata<ArrayProxy>, public Indexable, public NewIndexable,
		public Iterable, public Stringifiable, public Pushable, public GarbageCollected
	{
	public:
		static char const * const MetatableName;

		template <class T>
		inline static ArrayProxyByRefImpl<T>* MakeByRef(lua_State* L, Array<T>* object, LifetimeHolder const& lifetime)
		{
			static_assert(sizeof(ArrayProxyByRefImpl<T>) <= sizeof(impl_), "ArrayProxy implementation object too large!");
			auto self = New(L, lifetime);
			return new (self->impl_) ArrayProxyByRefImpl<T>(lifetime, object);
		}

		template <class T>
		inline static ArrayProxyByValImpl<T>* MakeByVal(lua_State* L, Array<T>* object, LifetimeHolder const& lifetime)
		{
			static_assert(sizeof(ArrayProxyByValImpl<T>) <= sizeof(impl_), "ArrayProxy implementation object too large!");
			auto self = New(L, lifetime);
			return new (self->impl_) ArrayProxyByValImpl<T>(lifetime, object);
		}

		inline ArrayProxyImplBase* GetImpl()
		{
			return reinterpret_cast<ArrayProxyImplBase*>(impl_);
		}

		inline bool IsAlive() const
		{
			return lifetime_.IsAlive();
		}

		template <class T>
		T* Get()
		{
			if (!lifetime_.IsAlive()) {
				return nullptr;
			}

			if (GetImpl()->IsA(ArrayProxyHelpers<T>::TypeName)) {
				return reinterpret_cast<T*>(GetImpl()->GetRaw());
			} else {
				return nullptr;
			}
		}

	private:
		LifetimeReference lifetime_;
		uint8_t impl_[40];

		ArrayProxy(LifetimeHolder const& lifetime)
			: lifetime_(lifetime)
		{}

		~ArrayProxy()
		{
			GetImpl()->~ArrayProxyImplBase();
		}

	protected:
		friend Userdata<ArrayProxy>;

		int Index(lua_State* L);
		int NewIndex(lua_State* L);
		int Next(lua_State* L);
		int ToString(lua_State* L);
		int GC(lua_State* L);
	};

	template <class T>
	inline void push_array_ref_proxy(lua_State* L, LifetimeHolder const& lifetime, T* v)
	{
		ArrayProxy::MakeByRef<T>(L, v, lifetime);
	}

	template <class T>
	inline T* checked_get_array_proxy(lua_State* L, int index)
	{
		auto proxy = Userdata<ArrayProxy>::CheckUserData(L, index);
		auto const& typeName = ArrayProxyHelpers<T>::TypeName;
		if (proxy->GetImpl()->IsA(typeName)) {
			auto obj = proxy->Get<T>();
			if (obj == nullptr) {
				luaL_error(L, "Argument %d: got array of '%s' whose lifetime has expired", index, typeName);
				return nullptr;
			} else {
				return obj;
			}
		} else {
			luaL_error(L, "Argument %d: expected an array of '%s', got '%s'", index, typeName, proxy->GetImpl()->GetTypeName());
			return nullptr;
		}
	}
}
