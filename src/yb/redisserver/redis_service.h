// Copyright (c) YugaByte, Inc.

#ifndef YB_REDISSERVER_REDIS_SERVICE_H_
#define YB_REDISSERVER_REDIS_SERVICE_H_

#include "yb/redisserver/redis_service.service.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

#include "yb/util/string_case.h"

using std::string;
using std::shared_ptr;

namespace yb {

namespace client {
class YBClient;
class YBTable;
class YBSession;
class YBRedisWriteOp;
class YBRedisReadOp;
}  // namespace client

namespace redisserver {

class RedisServer;

class RedisServiceImpl : public RedisServerServiceIf {
 public:
  RedisServiceImpl(RedisServer* server, string yb_tier_master_address);

  void Handle(yb::rpc::InboundCall* call) override;

  void GetCommand(yb::rpc::InboundCallPtr call, yb::rpc::RedisClientCommand* c);

  void HGetCommand(yb::rpc::InboundCallPtr call, yb::rpc::RedisClientCommand* c);

  void StrLenCommand(yb::rpc::InboundCallPtr call, yb::rpc::RedisClientCommand* c);

  void ExistsCommand(yb::rpc::InboundCallPtr call, yb::rpc::RedisClientCommand* c);

  void GetRangeCommand(yb::rpc::InboundCallPtr call, yb::rpc::RedisClientCommand* c);

  void SetCommand(yb::rpc::InboundCallPtr call, yb::rpc::RedisClientCommand* c);

  void HSetCommand(yb::rpc::InboundCallPtr call, yb::rpc::RedisClientCommand* c);

  void GetSetCommand(yb::rpc::InboundCallPtr call, yb::rpc::RedisClientCommand* c);

  void AppendCommand(yb::rpc::InboundCallPtr call, yb::rpc::RedisClientCommand* c);

  void DelCommand(yb::rpc::InboundCallPtr call, yb::rpc::RedisClientCommand* c);

  void SetRangeCommand(yb::rpc::InboundCallPtr call, yb::rpc::RedisClientCommand* c);

  void IncrCommand(yb::rpc::InboundCallPtr call, yb::rpc::RedisClientCommand* c);

  void EchoCommand(yb::rpc::InboundCallPtr call, yb::rpc::RedisClientCommand* c);

  void DummyCommand(yb::rpc::InboundCallPtr call, yb::rpc::RedisClientCommand* c);

 private:
  void ReadCommand(
      yb::rpc::InboundCallPtr call,
      yb::rpc::RedisClientCommand* c,
      const std::string& command_name,
      Status(*parse)(client::YBRedisReadOp*, const std::vector<Slice>&));

  void WriteCommand(
      yb::rpc::InboundCallPtr call,
      yb::rpc::RedisClientCommand* c,
      const std::string& command_name,
      Status(*parse)(client::YBRedisWriteOp*, const std::vector<Slice>&));

  typedef void (RedisServiceImpl::*RedisCommandFunctionPtr)(yb::rpc::InboundCallPtr call,
                                                            rpc::RedisClientCommand* c);

  // Information about RedisCommand(s) that we support.
  //
  // Based on "struct redisCommand" from redis/src/server.h
  //
  // The remaining fields in "struct redisCommand" from redis' implementation are
  // currently unused. They will be added and when we start using them.
  struct RedisCommandInfo {
    RedisCommandInfo(const string& name, const RedisCommandFunctionPtr& fptr, int arity)
        : name(name), function_ptr(fptr), arity(arity) {
      ToLowerCase(this->name, &this->name);
    }
    string name;
    RedisCommandFunctionPtr function_ptr;
    int arity;

   private:
    // Declare this private to ensure that we get a compiler error if kMethodCount is not the
    // size of kRedisCommandTable.
    RedisCommandInfo();
  };

  constexpr static int kRpcTimeoutSec = 5;
  constexpr static int kMethodCount = 13;

  void PopulateHandlers();
  // Fetches the appropriate handler for the command, nullptr if none exists.
  const RedisCommandInfo* FetchHandler(const std::vector<Slice>& cmd_args);
  CHECKED_STATUS SetUpYBClient(string yb_master_address);
  void RespondWithFailure(
      const string& error, yb::rpc::InboundCallPtr call, yb::rpc::RedisClientCommand* c);
  // Verify that the command has the required number of arguments, and if so, handle the call.
  void ValidateAndHandle(const RedisCommandInfo* cmd_info,
                         yb::rpc::InboundCallPtr call,
                         yb::rpc::RedisClientCommand* c);

  void ConfigureSession(client::YBSession* session);

  // Redis command table, for commands that we currently support.
  //
  // Based on "redisCommandTable[]" from redis/src/server.c
  // kMethodCount has to reflect the correct number of commands in the table.
  //
  // Every entry is composed of the following fields:
  //   name: a string representing the command name.
  //   function: pointer to the member function implementing the command.
  //   arity: number of arguments expected, it is possible to use -N to say >= N.
  const struct RedisCommandInfo kRedisCommandTable[kMethodCount] = {
      {"get", &RedisServiceImpl::GetCommand, 2},
      {"hget", &RedisServiceImpl::HGetCommand, 3},
      {"strlen", &RedisServiceImpl::StrLenCommand, 2},
      {"exists", &RedisServiceImpl::ExistsCommand, 2},
      {"getrange", &RedisServiceImpl::GetRangeCommand, 4},
      {"set", &RedisServiceImpl::SetCommand, -3},
      {"hset", &RedisServiceImpl::HSetCommand, 4},
      {"getset", &RedisServiceImpl::GetSetCommand, 3},
      {"append", &RedisServiceImpl::AppendCommand, 3},
      {"del", &RedisServiceImpl::DelCommand, 2},
      {"setrange", &RedisServiceImpl::SetRangeCommand, 4},
      {"incr", &RedisServiceImpl::IncrCommand, 2},
      {"echo", &RedisServiceImpl::EchoCommand, 2}};

  std::map<string, yb::rpc::RpcMethodMetrics> metrics_;
  std::map<string, const RedisCommandInfo*> command_name_to_info_map_;

  string yb_tier_master_addresses_;
  std::mutex yb_mutex_;  // Mutex that protects the creation of client_ and table_.
  std::atomic<bool> yb_client_initialized_;
  shared_ptr<client::YBClient> client_;
  shared_ptr<client::YBTable> table_;

  RedisServer* server_;
};

}  // namespace redisserver
}  // namespace yb

#endif  // YB_REDISSERVER_REDIS_SERVICE_H_
