/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License.
 */

#include "meta/processors/schema/CreateTagProcessor.h"

#include "meta/processors/schema/SchemaUtil.h"

namespace nebula {
namespace meta {

void CreateTagProcessor::process(const cpp2::CreateTagReq& req) {
  GraphSpaceID spaceId = req.get_space_id();
  CHECK_SPACE_ID_AND_RETURN(spaceId);
  const auto& tagName = req.get_tag_name();
  folly::SharedMutex::WriteHolder holder(LockUtils::tagAndEdgeLock());

  // Check if the edge with same name exists
  auto conflictRet = getEdgeType(spaceId, tagName);
  if (nebula::ok(conflictRet)) {
    LOG(ERROR) << "Failed to create tag `" << tagName
               << "': some edge with the same name already exists.";
    resp_.id_ref() = to(nebula::value(conflictRet), EntryType::TAG);
    handleErrorCode(nebula::cpp2::ErrorCode::E_CONFLICT);
    onFinished();
    return;
  } else {
    auto retCode = nebula::error(conflictRet);
    if (retCode != nebula::cpp2::ErrorCode::E_EDGE_NOT_FOUND) {
      LOG(ERROR) << "Failed to create tag " << tagName << " error "
                 << apache::thrift::util::enumNameSafe(retCode);
      handleErrorCode(retCode);
      onFinished();
      return;
    }
  }

  auto columns = req.get_schema().get_columns();
  if (!SchemaUtil::checkType(columns)) {
    handleErrorCode(nebula::cpp2::ErrorCode::E_INVALID_PARM);
    onFinished();
    return;
  }

  cpp2::Schema schema;
  schema.columns_ref() = std::move(columns);
  schema.schema_prop_ref() = req.get_schema().get_schema_prop();

  auto ret = getTagId(spaceId, tagName);
  if (nebula::ok(ret)) {
    if (req.get_if_not_exists()) {
      handleErrorCode(nebula::cpp2::ErrorCode::SUCCEEDED);
    } else {
      LOG(ERROR) << "Create Tag Failed :" << tagName << " has existed";
      handleErrorCode(nebula::cpp2::ErrorCode::E_EXISTED);
    }
    resp_.id_ref() = to(nebula::value(ret), EntryType::TAG);
    onFinished();
    return;
  } else {
    auto retCode = nebula::error(ret);
    if (retCode != nebula::cpp2::ErrorCode::E_TAG_NOT_FOUND) {
      LOG(ERROR) << "Failed to create tag " << tagName << " error "
                 << apache::thrift::util::enumNameSafe(retCode);
      handleErrorCode(retCode);
      onFinished();
      return;
    }
  }

  auto tagRet = autoIncrementIdInSpace(spaceId);
  if (!nebula::ok(tagRet)) {
    LOG(ERROR) << "Create tag failed : Get tag id failed.";
    handleErrorCode(nebula::error(tagRet));
    onFinished();
    return;
  }

  auto tagId = nebula::value(tagRet);
  std::vector<kvstore::KV> data;
  data.emplace_back(MetaKeyUtils::indexTagKey(spaceId, tagName),
                    std::string(reinterpret_cast<const char*>(&tagId), sizeof(TagID)));
  data.emplace_back(MetaKeyUtils::schemaTagKey(spaceId, tagId, 0),
                    MetaKeyUtils::schemaVal(tagName, schema));

  LOG(INFO) << "Create Tag " << tagName << ", TagID " << tagId;

  resp_.id_ref() = to(tagId, EntryType::TAG);
  doSyncPutAndUpdate(std::move(data));
}

}  // namespace meta
}  // namespace nebula
