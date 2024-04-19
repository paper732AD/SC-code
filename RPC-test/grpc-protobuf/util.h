#pragma once

#include "flat.pb.h"

extern void serialize_req_flat(test::FlatLogFileFlushChunkRequestPB &, void *);
extern void deserialize_req_flat(test::FlatLogFileFlushChunkRequestPB &, void *, size_t);
extern void serialize_resp_flat(test::FlatLogFileFlushChunkResponsePB &, void *);
extern void deserialize_resp_flat(test::FlatLogFileFlushChunkResponsePB &, void *, size_t);
extern test::FlatLogFileFlushChunkRequestPB get_flat_req(int);
extern test::FlatLogFileFlushChunkResponsePB get_flat_resp(int);