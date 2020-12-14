// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: loan.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_loan_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_loan_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3014000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3014000 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_loan_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_loan_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxiliaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_loan_2eproto;
namespace loan {
class MsgLog;
class MsgLogDefaultTypeInternal;
extern MsgLogDefaultTypeInternal _MsgLog_default_instance_;
}  // namespace loan
PROTOBUF_NAMESPACE_OPEN
template<> ::loan::MsgLog* Arena::CreateMaybeMessage<::loan::MsgLog>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace loan {

enum MsgLog_Msg_Type : int {
  MsgLog_Msg_Type_SEND_LOG = 0,
  MsgLog_Msg_Type_SEND_COLLECT = 1,
  MsgLog_Msg_Type_MsgLog_Msg_Type_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  MsgLog_Msg_Type_MsgLog_Msg_Type_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool MsgLog_Msg_Type_IsValid(int value);
constexpr MsgLog_Msg_Type MsgLog_Msg_Type_Msg_Type_MIN = MsgLog_Msg_Type_SEND_LOG;
constexpr MsgLog_Msg_Type MsgLog_Msg_Type_Msg_Type_MAX = MsgLog_Msg_Type_SEND_COLLECT;
constexpr int MsgLog_Msg_Type_Msg_Type_ARRAYSIZE = MsgLog_Msg_Type_Msg_Type_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* MsgLog_Msg_Type_descriptor();
template<typename T>
inline const std::string& MsgLog_Msg_Type_Name(T enum_t_value) {
  static_assert(::std::is_same<T, MsgLog_Msg_Type>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function MsgLog_Msg_Type_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    MsgLog_Msg_Type_descriptor(), enum_t_value);
}
inline bool MsgLog_Msg_Type_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, MsgLog_Msg_Type* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<MsgLog_Msg_Type>(
    MsgLog_Msg_Type_descriptor(), name, value);
}
// ===================================================================

class MsgLog PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:loan.MsgLog) */ {
 public:
  inline MsgLog() : MsgLog(nullptr) {}
  virtual ~MsgLog();

  MsgLog(const MsgLog& from);
  MsgLog(MsgLog&& from) noexcept
    : MsgLog() {
    *this = ::std::move(from);
  }

  inline MsgLog& operator=(const MsgLog& from) {
    CopyFrom(from);
    return *this;
  }
  inline MsgLog& operator=(MsgLog&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const MsgLog& default_instance();

  static inline const MsgLog* internal_default_instance() {
    return reinterpret_cast<const MsgLog*>(
               &_MsgLog_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(MsgLog& a, MsgLog& b) {
    a.Swap(&b);
  }
  inline void Swap(MsgLog* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(MsgLog* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline MsgLog* New() const final {
    return CreateMaybeMessage<MsgLog>(nullptr);
  }

  MsgLog* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<MsgLog>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const MsgLog& from);
  void MergeFrom(const MsgLog& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(MsgLog* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "loan.MsgLog";
  }
  protected:
  explicit MsgLog(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_loan_2eproto);
    return ::descriptor_table_loan_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  typedef MsgLog_Msg_Type Msg_Type;
  static constexpr Msg_Type SEND_LOG =
    MsgLog_Msg_Type_SEND_LOG;
  static constexpr Msg_Type SEND_COLLECT =
    MsgLog_Msg_Type_SEND_COLLECT;
  static inline bool Msg_Type_IsValid(int value) {
    return MsgLog_Msg_Type_IsValid(value);
  }
  static constexpr Msg_Type Msg_Type_MIN =
    MsgLog_Msg_Type_Msg_Type_MIN;
  static constexpr Msg_Type Msg_Type_MAX =
    MsgLog_Msg_Type_Msg_Type_MAX;
  static constexpr int Msg_Type_ARRAYSIZE =
    MsgLog_Msg_Type_Msg_Type_ARRAYSIZE;
  static inline const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor*
  Msg_Type_descriptor() {
    return MsgLog_Msg_Type_descriptor();
  }
  template<typename T>
  static inline const std::string& Msg_Type_Name(T enum_t_value) {
    static_assert(::std::is_same<T, Msg_Type>::value ||
      ::std::is_integral<T>::value,
      "Incorrect type passed to function Msg_Type_Name.");
    return MsgLog_Msg_Type_Name(enum_t_value);
  }
  static inline bool Msg_Type_Parse(::PROTOBUF_NAMESPACE_ID::ConstStringParam name,
      Msg_Type* value) {
    return MsgLog_Msg_Type_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  enum : int {
    kServiceNameFieldNumber = 3,
    kLogContentsFieldNumber = 4,
    kMsgTypeFieldNumber = 1,
    kMsgCmdFieldNumber = 2,
  };
  // string service_name = 3;
  void clear_service_name();
  const std::string& service_name() const;
  void set_service_name(const std::string& value);
  void set_service_name(std::string&& value);
  void set_service_name(const char* value);
  void set_service_name(const char* value, size_t size);
  std::string* mutable_service_name();
  std::string* release_service_name();
  void set_allocated_service_name(std::string* service_name);
  private:
  const std::string& _internal_service_name() const;
  void _internal_set_service_name(const std::string& value);
  std::string* _internal_mutable_service_name();
  public:

  // string LogContents = 4;
  void clear_logcontents();
  const std::string& logcontents() const;
  void set_logcontents(const std::string& value);
  void set_logcontents(std::string&& value);
  void set_logcontents(const char* value);
  void set_logcontents(const char* value, size_t size);
  std::string* mutable_logcontents();
  std::string* release_logcontents();
  void set_allocated_logcontents(std::string* logcontents);
  private:
  const std::string& _internal_logcontents() const;
  void _internal_set_logcontents(const std::string& value);
  std::string* _internal_mutable_logcontents();
  public:

  // .loan.MsgLog.Msg_Type msg_type = 1;
  void clear_msg_type();
  ::loan::MsgLog_Msg_Type msg_type() const;
  void set_msg_type(::loan::MsgLog_Msg_Type value);
  private:
  ::loan::MsgLog_Msg_Type _internal_msg_type() const;
  void _internal_set_msg_type(::loan::MsgLog_Msg_Type value);
  public:

  // int32 msg_cmd = 2;
  void clear_msg_cmd();
  ::PROTOBUF_NAMESPACE_ID::int32 msg_cmd() const;
  void set_msg_cmd(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_msg_cmd() const;
  void _internal_set_msg_cmd(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // @@protoc_insertion_point(class_scope:loan.MsgLog)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr service_name_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr logcontents_;
  int msg_type_;
  ::PROTOBUF_NAMESPACE_ID::int32 msg_cmd_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_loan_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// MsgLog

// .loan.MsgLog.Msg_Type msg_type = 1;
inline void MsgLog::clear_msg_type() {
  msg_type_ = 0;
}
inline ::loan::MsgLog_Msg_Type MsgLog::_internal_msg_type() const {
  return static_cast< ::loan::MsgLog_Msg_Type >(msg_type_);
}
inline ::loan::MsgLog_Msg_Type MsgLog::msg_type() const {
  // @@protoc_insertion_point(field_get:loan.MsgLog.msg_type)
  return _internal_msg_type();
}
inline void MsgLog::_internal_set_msg_type(::loan::MsgLog_Msg_Type value) {
  
  msg_type_ = value;
}
inline void MsgLog::set_msg_type(::loan::MsgLog_Msg_Type value) {
  _internal_set_msg_type(value);
  // @@protoc_insertion_point(field_set:loan.MsgLog.msg_type)
}

// int32 msg_cmd = 2;
inline void MsgLog::clear_msg_cmd() {
  msg_cmd_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 MsgLog::_internal_msg_cmd() const {
  return msg_cmd_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 MsgLog::msg_cmd() const {
  // @@protoc_insertion_point(field_get:loan.MsgLog.msg_cmd)
  return _internal_msg_cmd();
}
inline void MsgLog::_internal_set_msg_cmd(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  msg_cmd_ = value;
}
inline void MsgLog::set_msg_cmd(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_msg_cmd(value);
  // @@protoc_insertion_point(field_set:loan.MsgLog.msg_cmd)
}

// string service_name = 3;
inline void MsgLog::clear_service_name() {
  service_name_.ClearToEmpty();
}
inline const std::string& MsgLog::service_name() const {
  // @@protoc_insertion_point(field_get:loan.MsgLog.service_name)
  return _internal_service_name();
}
inline void MsgLog::set_service_name(const std::string& value) {
  _internal_set_service_name(value);
  // @@protoc_insertion_point(field_set:loan.MsgLog.service_name)
}
inline std::string* MsgLog::mutable_service_name() {
  // @@protoc_insertion_point(field_mutable:loan.MsgLog.service_name)
  return _internal_mutable_service_name();
}
inline const std::string& MsgLog::_internal_service_name() const {
  return service_name_.Get();
}
inline void MsgLog::_internal_set_service_name(const std::string& value) {
  
  service_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArena());
}
inline void MsgLog::set_service_name(std::string&& value) {
  
  service_name_.Set(
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:loan.MsgLog.service_name)
}
inline void MsgLog::set_service_name(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  service_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(value), GetArena());
  // @@protoc_insertion_point(field_set_char:loan.MsgLog.service_name)
}
inline void MsgLog::set_service_name(const char* value,
    size_t size) {
  
  service_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:loan.MsgLog.service_name)
}
inline std::string* MsgLog::_internal_mutable_service_name() {
  
  return service_name_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArena());
}
inline std::string* MsgLog::release_service_name() {
  // @@protoc_insertion_point(field_release:loan.MsgLog.service_name)
  return service_name_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void MsgLog::set_allocated_service_name(std::string* service_name) {
  if (service_name != nullptr) {
    
  } else {
    
  }
  service_name_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), service_name,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:loan.MsgLog.service_name)
}

// string LogContents = 4;
inline void MsgLog::clear_logcontents() {
  logcontents_.ClearToEmpty();
}
inline const std::string& MsgLog::logcontents() const {
  // @@protoc_insertion_point(field_get:loan.MsgLog.LogContents)
  return _internal_logcontents();
}
inline void MsgLog::set_logcontents(const std::string& value) {
  _internal_set_logcontents(value);
  // @@protoc_insertion_point(field_set:loan.MsgLog.LogContents)
}
inline std::string* MsgLog::mutable_logcontents() {
  // @@protoc_insertion_point(field_mutable:loan.MsgLog.LogContents)
  return _internal_mutable_logcontents();
}
inline const std::string& MsgLog::_internal_logcontents() const {
  return logcontents_.Get();
}
inline void MsgLog::_internal_set_logcontents(const std::string& value) {
  
  logcontents_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArena());
}
inline void MsgLog::set_logcontents(std::string&& value) {
  
  logcontents_.Set(
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:loan.MsgLog.LogContents)
}
inline void MsgLog::set_logcontents(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  logcontents_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(value), GetArena());
  // @@protoc_insertion_point(field_set_char:loan.MsgLog.LogContents)
}
inline void MsgLog::set_logcontents(const char* value,
    size_t size) {
  
  logcontents_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:loan.MsgLog.LogContents)
}
inline std::string* MsgLog::_internal_mutable_logcontents() {
  
  return logcontents_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArena());
}
inline std::string* MsgLog::release_logcontents() {
  // @@protoc_insertion_point(field_release:loan.MsgLog.LogContents)
  return logcontents_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void MsgLog::set_allocated_logcontents(std::string* logcontents) {
  if (logcontents != nullptr) {
    
  } else {
    
  }
  logcontents_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), logcontents,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:loan.MsgLog.LogContents)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace loan

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::loan::MsgLog_Msg_Type> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::loan::MsgLog_Msg_Type>() {
  return ::loan::MsgLog_Msg_Type_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_loan_2eproto
