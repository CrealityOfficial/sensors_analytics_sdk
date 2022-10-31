/*
 * Copyright 2015－2021 Sensors Data Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#ifndef SENSORS_ANALYTICS_SDK_H_
#define SENSORS_ANALYTICS_SDK_H_

#include <cstring>
#include <ctime>
#include <deque>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#define SA_SDK_VERSION "1.0.2"
#define SA_SDK_NAME "SensorsAnalytics CPP SDK"
#define SA_SDK_FULL_NAME SA_SDK_NAME " " SA_SDK_VERSION

#ifdef _MSC_VER
#if _MSC_VER >= 1600
#include <cstdint>
#else
typedef __int32 int32_t;
typedef __int64 int64_t;
#endif
#elif __GNUC__ >= 3

#include <cstdint>

#endif

namespace sensors_analytics {

using namespace std;

class Sdk;

class DefaultConsumer;

class PropertiesNode;
namespace utils {
class ObjectNode;
}

/// 用户变更观察者，当 distinct_id 发生变化时会触发 update() 函数
class UserAlterationObserver {
 public:
  virtual void Update() = 0;
};

class Sdk {
 public:
  static bool Init(const string &data_file_path, const string &server_url,
                   const string &distinct_id, bool is_login_id,
                   int max_staging_record_count);

  static void EnableLog(bool enable);

  static bool Flush();

  static bool FlushPart(size_t part_size, bool drop_failed_record);

  static void ClearQueue();

  static void Shutdown();

  static void RegisterSuperProperties(const PropertiesNode &properties);

  static void ClearSuperProperties();

  static void Track(const string &event_name);

  static void Track(const string &event_name, const PropertiesNode &properties);

  static void ItemSet(const string &item_type, const string &item_id,
                      PropertiesNode &properties);

  static void ItemDelete(const string &item_type, const string &item_id);

  static void TrackInstallation(const string &event_name,
                                const PropertiesNode &properties);

  static void Login(const string &login_id);

  static void Identify(const string &distinct_id, bool is_login_id);

  static void ProfileSet(const PropertiesNode &properties);

  static void ProfileSetString(const string &property_name,
                               const string &str_value);

  static void ProfileSetNumber(const string &property_name, int number_value);

  static void ProfileSetNumber(const string &property_name,
                               double number_value);

  static void ProfileSetBool(const string &property_name, bool bool_value);

  static void ProfileSetOnce(const PropertiesNode &properties);

  static void ProfileSetOnceString(const string &property_name,
                                   const string &str_value);

  static void ProfileSetOnceNumber(const string &property_name,
                                   int number_value);

  static void ProfileSetOnceNumber(const string &property_name,
                                   double number_value);

  static void ProfileSetOnceBool(const string &property_name, bool bool_value);

  static void ProfileIncrement(const PropertiesNode &properties);

  static void ProfileIncrement(const string &property_name, int number_value);

  static void ProfileAppend(const PropertiesNode &properties);

  static void ProfileAppend(const string &property_name,
                            const string &str_value);

  static void ProfileUnset(const string &property_name);

  static void ProfileDelete();

  static void AppendRecordsToDisk(bool enable);

  static void DumpAllRecordsToDisk();

  static void SetCookie(const string &cookie, bool encode);

  static string GetCookie(bool decode);

  static string DistinctID();
  static bool IsLoginID();
  static string StagingFilePath();
  static bool IsEnableLog();

  static void Attach(UserAlterationObserver *observer);

  static void Detach(UserAlterationObserver *observer);

  ~Sdk();

 private:
  Sdk(const string &server_url, const string &data_file_path,
      int max_staging_record_count, const string &distinct_id,
      bool is_login_id);

  Sdk(const Sdk &);

  Sdk &operator=(const Sdk &);

  bool AddEvent(const string &action_type, const string &event_name,
                const utils::ObjectNode &properties, const string &distinct_id,
                const string &original_id);

  bool AddItemEvent(const string &action_type, const string &item_type,
                    const string &item_id, const utils::ObjectNode &properties);

  void ResetSuperProperties();

  static bool AssertProperties(const utils::ObjectNode &properties);

  static bool AssertKey(const string &type, const string &key);

  static bool AssertId(const string &type, const string &key);

  static void Notify();

  static Sdk *instance_;
  PropertiesNode *super_properties_;
  string distinct_id_;
  bool is_login_id_;
  string staging_file_path_;
  vector<UserAlterationObserver *> observers;
  DefaultConsumer *consumer_;
};

namespace utils {

class ObjectNode {
 public:
  void SetNumber(const string &property_name, int32_t value);

  void SetNumber(const string &property_name, int64_t value);

  void SetNumber(const string &property_name, double value);

  void SetString(const string &property_name, const string &value);

  void SetString(const string &property_name, const char *value);

  void SetBool(const string &property_name, bool value);

  void SetList(const string &property_name, const std::vector<string> &value);

  void SetDateTime(const string &property_name, time_t seconds,
                   int milliseconds);

  void SetDateTime(const string &property_name, const string &value);

  void Clear();

  virtual void SetObject(const string &property_name, const ObjectNode &value);

  static string ToJson(const ObjectNode &node);

 protected:
  ObjectNode();

  class ValueNode;

  std::map<string, ValueNode> properties_map_;

 private:
  static void DumpNode(const ObjectNode &node, string *buffer);

  void MergeFrom(const ObjectNode &another_node);

  friend class ::sensors_analytics::Sdk;

  enum ValueNodeType {
    NUMBER,
    INT,
    STRING,
    LIST,
    DATETIME,
    BOOL,
    OBJECT,
    UNKNOWN,
  };
};

class ObjectNode::ValueNode {
 public:
  ValueNode() : node_type_(UNKNOWN) {}

  explicit ValueNode(double value);

  explicit ValueNode(int64_t value);

  explicit ValueNode(const string &value);

  explicit ValueNode(bool value);

  explicit ValueNode(const ObjectNode &value);

  explicit ValueNode(const std::vector<string> &value);

  ValueNode(time_t seconds, int milliseconds);

  static void ToStr(const ValueNode &node, string *buffer);

 private:
  static void DumpString(const string &value, string *buffer);

  static void DumpList(const std::vector<string> &value, string *buffer);

  static void DumpDateTime(const time_t &seconds, int milliseconds,
                           string *buffer);

  static void DumpNumber(double value, string *buffer);

  static void DumpNumber(int64_t value, string *buffer);

  friend class ::sensors_analytics::Sdk;

  ValueNodeType node_type_;

  union UnionValue {
    double number_value;
    bool bool_value;
    struct {
      std::time_t seconds;
      int milliseconds;
    } date_time_value;
    int64_t int_value;

    UnionValue() { memset(this, 0, sizeof(UnionValue)); }
  } value_;

  string string_data_;
  std::vector<string> list_data_;
  ObjectNode object_data_;
};
}  // namespace utils

class PropertiesNode : public utils::ObjectNode {
 private:
  friend class Sdk;

  void SetObject(const string &property_name, const ObjectNode &value);
};

}  // namespace sensors_analytics

#endif  // SENSORS_ANALYTICS_SDK_H_
