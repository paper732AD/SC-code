// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: test_array.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_test_5farray_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_test_5farray_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_test_5farray_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_test_5farray_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_test_5farray_2eproto;
class test;
struct testDefaultTypeInternal;
extern testDefaultTypeInternal _test_default_instance_;
PROTOBUF_NAMESPACE_OPEN
template<> ::test* Arena::CreateMaybeMessage<::test>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

// ===================================================================

class test final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:test) */ {
 public:
  inline test() : test(nullptr) {}
  ~test() override;
  explicit PROTOBUF_CONSTEXPR test(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  test(const test& from);
  test(test&& from) noexcept
    : test() {
    *this = ::std::move(from);
  }

  inline test& operator=(const test& from) {
    CopyFrom(from);
    return *this;
  }
  inline test& operator=(test&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const test& default_instance() {
    return *internal_default_instance();
  }
  static inline const test* internal_default_instance() {
    return reinterpret_cast<const test*>(
               &_test_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(test& a, test& b) {
    a.Swap(&b);
  }
  inline void Swap(test* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(test* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  test* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<test>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const test& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const test& from) {
    test::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(test* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "test";
  }
  protected:
  explicit test(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kAFieldNumber = 1,
  };
  // repeated bytes a = 1;
  int a_size() const;
  private:
  int _internal_a_size() const;
  public:
  void clear_a();
  const std::string& a(int index) const;
  std::string* mutable_a(int index);
  void set_a(int index, const std::string& value);
  void set_a(int index, std::string&& value);
  void set_a(int index, const char* value);
  void set_a(int index, const void* value, size_t size);
  std::string* add_a();
  void add_a(const std::string& value);
  void add_a(std::string&& value);
  void add_a(const char* value);
  void add_a(const void* value, size_t size);
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>& a() const;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>* mutable_a();
  private:
  const std::string& _internal_a(int index) const;
  std::string* _internal_add_a();
  public:

  // @@protoc_insertion_point(class_scope:test)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string> a_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_test_5farray_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// test

// repeated bytes a = 1;
inline int test::_internal_a_size() const {
  return _impl_.a_.size();
}
inline int test::a_size() const {
  return _internal_a_size();
}
inline void test::clear_a() {
  _impl_.a_.Clear();
}
inline std::string* test::add_a() {
  std::string* _s = _internal_add_a();
  // @@protoc_insertion_point(field_add_mutable:test.a)
  return _s;
}
inline const std::string& test::_internal_a(int index) const {
  return _impl_.a_.Get(index);
}
inline const std::string& test::a(int index) const {
  // @@protoc_insertion_point(field_get:test.a)
  return _internal_a(index);
}
inline std::string* test::mutable_a(int index) {
  // @@protoc_insertion_point(field_mutable:test.a)
  return _impl_.a_.Mutable(index);
}
inline void test::set_a(int index, const std::string& value) {
  _impl_.a_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set:test.a)
}
inline void test::set_a(int index, std::string&& value) {
  _impl_.a_.Mutable(index)->assign(std::move(value));
  // @@protoc_insertion_point(field_set:test.a)
}
inline void test::set_a(int index, const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  _impl_.a_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set_char:test.a)
}
inline void test::set_a(int index, const void* value, size_t size) {
  _impl_.a_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:test.a)
}
inline std::string* test::_internal_add_a() {
  return _impl_.a_.Add();
}
inline void test::add_a(const std::string& value) {
  _impl_.a_.Add()->assign(value);
  // @@protoc_insertion_point(field_add:test.a)
}
inline void test::add_a(std::string&& value) {
  _impl_.a_.Add(std::move(value));
  // @@protoc_insertion_point(field_add:test.a)
}
inline void test::add_a(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  _impl_.a_.Add()->assign(value);
  // @@protoc_insertion_point(field_add_char:test.a)
}
inline void test::add_a(const void* value, size_t size) {
  _impl_.a_.Add()->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_add_pointer:test.a)
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>&
test::a() const {
  // @@protoc_insertion_point(field_list:test.a)
  return _impl_.a_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>*
test::mutable_a() {
  // @@protoc_insertion_point(field_mutable_list:test.a)
  return &_impl_.a_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_test_5farray_2eproto
