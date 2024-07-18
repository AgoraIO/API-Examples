// Copyright (c) 2020 Agora.io. All rights reserved

// This program is confidential and proprietary to Agora.io.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Agora.io.

#pragma once  // NOLINT(build/header_guard)
#include "AgoraRefPtr.h"
#include "AgoraBase.h"
#include <functional>
#include <api/aosl_ref.h>

namespace agora {
namespace base {

enum SyncClientError {
  kOk = 0,
  kFail = -1,
  kConnectSyncFailed = -2,
  kConnectDatabaseFailed = -3,
  kDisconnectDatabaseFailed = -4,
  kDatabaseNotConnected = -5,
  kCreateCollectionFailed = -6,
  kCollectionNotCreated = -7,
  kCollectionExisted = -8,
  kInvalidParams = -9,
  kNotLoggedIn = -10,
  kQueryDocFailed = -11,
  kDocNotCreated = -12,
};

typedef void(*syncClientCallback)(SyncClientError, void*);
typedef void(*dataBaseOpCallback)(SyncClientError, const char*, void*);
typedef void(*collectionOpCallback)(SyncClientError, const char*, const char*, void*);
typedef void(*queryDocCallback)(SyncClientError error, const char* resultJson, size_t count, bool more, void* userData);

enum SyncEventType {
  kInserted = 0,
  kPut = 1,
  kDeleted = 2,

  kToBeInserted = 3,
  kToBePut = 4,
  kToBeDeleted = 5,

  kTransactionBegin = 6,
  kTransactionEnd = 7,
  kDocSyncEnd = 8,
};

/**
 * sync client observer
 */
class ISyncClientObserver {
 public:
  struct CollectionEvent {
    SyncEventType type;
    const char* path;
    const char* value;
  };
  virtual void onCollectionEvent(const char* previousJson, const char* curJson, const char* collection, const char* docName,
                                 const CollectionEvent* events, int eventSize) = 0;
  virtual void onDatabaseEvent(const char* databaseName, SyncClientError error) = 0;
  virtual void onDataException(const char* databaseName, const char* collectionName) = 0;
  virtual ~ISyncClientObserver() {};
};

/**
 * sync configuration
 */
struct SyncConfig {
  const char* appId;
  /* shakehand interval in seconds, 0 means enable manual shake hand */
  uint32_t shakehand_interval;
  /* connection timeout in seconds */
  uint32_t connection_timeout;
  /* compact interval in seconds */
  uint32_t compact_interval;
  SyncConfig() : shakehand_interval(1), connection_timeout(10), compact_interval(3600 * 1000) {}
};

class ISyncClient : public RefCountInterface {
protected:
  virtual ~ISyncClient() {}
public:

  virtual int32_t registerSyncClientObserver(ISyncClientObserver* observer, void(*safeDeleter)(ISyncClientObserver*) = OPTIONAL_NULLPTR, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int32_t unregisterSyncClientObserver(ISyncClientObserver* observer) = 0;
  // client operations
  virtual int32_t login(const char* token, const char* channelName, user_id_t userId, syncClientCallback callback, void* userData, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int32_t queryDoc(const char* database, const char* coll, const char* range_start, const char* range_end, int64_t limits, bool doc_only, bool count_only, queryDocCallback callback, void* userData, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int32_t logout(aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int32_t renewToken(const char* token, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  // database operations
  virtual int32_t connectDatabase(const char* database, dataBaseOpCallback callback, void* userData, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int32_t disconnectDatabase(const char* database,
                          dataBaseOpCallback callback, void* userData, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int32_t createCollection(const char* database, const char* collection,
                        const char** readable, int readSize,
                        collectionOpCallback callback, void* userData, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int32_t deleteCollection(const char* database, const char* collection,
                        collectionOpCallback callback, void* userData, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  // collection operations
  virtual int32_t subscribe(const char* database, const char* collection,
                    util::AString& snapshotJson) = 0;
  virtual int32_t unsubscribe(const char* database, const char* collection, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int32_t putDoc(const char* database, const char* collection,
                 const char* docName, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int32_t deleteDoc(const char* database, const char* collection,
                    const char* docName, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int32_t getDocs(const char* database, const char* collection,
                  util::AString* docNames, uint32_t docSize) = 0;

  // document operations
  virtual int32_t putDocValue(const char* database, const char* collection,
                      const char* docName, const char* jsonValue, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int32_t updateDocValue(const char* database, const char* collection,
                         const char* docName, const char* path,
                         const char* jsonValue, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int32_t deleteDocValue(const char* database, const char* collection,
                         const char* docName, const char* path, aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int32_t deleteDocValues(const char* database, const char* collection, const char* docName,
                          const char** path, uint32_t pathSize,
                          aosl_ref_t ares = AOSL_REF_INVALID) = 0;
  virtual int32_t getDocValue(const char* database, const char* collection,
                      const char* docName, util::AString& jsonValue) = 0;
  virtual int32_t hasPath(const char* database, const char* collection,
                  const char* docName, const char* path, bool& result) = 0;
  virtual int32_t keepAliveDoc(const char* database, const char* collection,
                       const char* docName, uint32_t ttl, aosl_ref_t ares = AOSL_REF_INVALID) = 0;

  // sync operations
  virtual int32_t shakehand(aosl_ref_t ares = AOSL_REF_INVALID) = 0;
};
}// namespace base
}// namespace agora
