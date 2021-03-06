// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Enum.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_Enum_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_Enum_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3017000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3017001 < PROTOBUF_MIN_PROTOC_VERSION
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
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_Enum_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_Enum_2eproto {
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
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_Enum_2eproto;
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE
namespace Protocol {

enum PlayerType : int {
  PLAYER_TYPE_NONE = 0,
  PLAYER_TYPE_BEAR1 = 1,
  PLAYER_TYPE_BEAR2 = 2,
  PLAYER_TYPE_BEAR3 = 3,
  PlayerType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  PlayerType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool PlayerType_IsValid(int value);
constexpr PlayerType PlayerType_MIN = PLAYER_TYPE_NONE;
constexpr PlayerType PlayerType_MAX = PLAYER_TYPE_BEAR3;
constexpr int PlayerType_ARRAYSIZE = PlayerType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* PlayerType_descriptor();
template<typename T>
inline const std::string& PlayerType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, PlayerType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function PlayerType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    PlayerType_descriptor(), enum_t_value);
}
inline bool PlayerType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, PlayerType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<PlayerType>(
    PlayerType_descriptor(), name, value);
}
enum ObjectType : int {
  PLAYER = 0,
  WALL = 1,
  COMMONITEM = 2,
  UNIQEITEM = 3,
  TREASURE = 4,
  ObjectType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  ObjectType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool ObjectType_IsValid(int value);
constexpr ObjectType ObjectType_MIN = PLAYER;
constexpr ObjectType ObjectType_MAX = TREASURE;
constexpr int ObjectType_ARRAYSIZE = ObjectType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ObjectType_descriptor();
template<typename T>
inline const std::string& ObjectType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ObjectType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ObjectType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    ObjectType_descriptor(), enum_t_value);
}
inline bool ObjectType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, ObjectType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<ObjectType>(
    ObjectType_descriptor(), name, value);
}
enum MoveDir : int {
  FRONT = 0,
  BACK = 1,
  LEFT = 2,
  RIGHT = 3,
  MoveDir_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  MoveDir_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool MoveDir_IsValid(int value);
constexpr MoveDir MoveDir_MIN = FRONT;
constexpr MoveDir MoveDir_MAX = RIGHT;
constexpr int MoveDir_ARRAYSIZE = MoveDir_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* MoveDir_descriptor();
template<typename T>
inline const std::string& MoveDir_Name(T enum_t_value) {
  static_assert(::std::is_same<T, MoveDir>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function MoveDir_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    MoveDir_descriptor(), enum_t_value);
}
inline bool MoveDir_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, MoveDir* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<MoveDir>(
    MoveDir_descriptor(), name, value);
}
enum STATE : int {
  IDLE = 0,
  WALK = 1,
  DASH = 2,
  JUMP = 3,
  ATTACK = 4,
  STUN = 5,
  STUNEND = 6,
  DASHREST = 7,
  SLOW = 8,
  SHIELD = 9,
  STATE_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  STATE_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool STATE_IsValid(int value);
constexpr STATE STATE_MIN = IDLE;
constexpr STATE STATE_MAX = SHIELD;
constexpr int STATE_ARRAYSIZE = STATE_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* STATE_descriptor();
template<typename T>
inline const std::string& STATE_Name(T enum_t_value) {
  static_assert(::std::is_same<T, STATE>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function STATE_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    STATE_descriptor(), enum_t_value);
}
inline bool STATE_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, STATE* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<STATE>(
    STATE_descriptor(), name, value);
}
enum DEBUFITEMTYPE : int {
  DEBUF_SPEEDDOWN = 0,
  DEBUF_BLIND = 1,
  DEBUF_STUN = 2,
  DEBUFITEMTYPE_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  DEBUFITEMTYPE_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool DEBUFITEMTYPE_IsValid(int value);
constexpr DEBUFITEMTYPE DEBUFITEMTYPE_MIN = DEBUF_SPEEDDOWN;
constexpr DEBUFITEMTYPE DEBUFITEMTYPE_MAX = DEBUF_STUN;
constexpr int DEBUFITEMTYPE_ARRAYSIZE = DEBUFITEMTYPE_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* DEBUFITEMTYPE_descriptor();
template<typename T>
inline const std::string& DEBUFITEMTYPE_Name(T enum_t_value) {
  static_assert(::std::is_same<T, DEBUFITEMTYPE>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function DEBUFITEMTYPE_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    DEBUFITEMTYPE_descriptor(), enum_t_value);
}
inline bool DEBUFITEMTYPE_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, DEBUFITEMTYPE* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<DEBUFITEMTYPE>(
    DEBUFITEMTYPE_descriptor(), name, value);
}
enum PLAYER_TYPE : int {
  BROWN = 0,
  GRAY = 1,
  BLUE = 2,
  PLAYER_TYPE_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  PLAYER_TYPE_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool PLAYER_TYPE_IsValid(int value);
constexpr PLAYER_TYPE PLAYER_TYPE_MIN = BROWN;
constexpr PLAYER_TYPE PLAYER_TYPE_MAX = BLUE;
constexpr int PLAYER_TYPE_ARRAYSIZE = PLAYER_TYPE_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* PLAYER_TYPE_descriptor();
template<typename T>
inline const std::string& PLAYER_TYPE_Name(T enum_t_value) {
  static_assert(::std::is_same<T, PLAYER_TYPE>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function PLAYER_TYPE_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    PLAYER_TYPE_descriptor(), enum_t_value);
}
inline bool PLAYER_TYPE_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, PLAYER_TYPE* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<PLAYER_TYPE>(
    PLAYER_TYPE_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace Protocol

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::Protocol::PlayerType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::PlayerType>() {
  return ::Protocol::PlayerType_descriptor();
}
template <> struct is_proto_enum< ::Protocol::ObjectType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::ObjectType>() {
  return ::Protocol::ObjectType_descriptor();
}
template <> struct is_proto_enum< ::Protocol::MoveDir> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::MoveDir>() {
  return ::Protocol::MoveDir_descriptor();
}
template <> struct is_proto_enum< ::Protocol::STATE> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::STATE>() {
  return ::Protocol::STATE_descriptor();
}
template <> struct is_proto_enum< ::Protocol::DEBUFITEMTYPE> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::DEBUFITEMTYPE>() {
  return ::Protocol::DEBUFITEMTYPE_descriptor();
}
template <> struct is_proto_enum< ::Protocol::PLAYER_TYPE> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::PLAYER_TYPE>() {
  return ::Protocol::PLAYER_TYPE_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_Enum_2eproto
