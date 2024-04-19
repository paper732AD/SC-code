#include "util.h"

static std::string temp;

using namespace test;               

void serialize_req_flat(FlatLogFileFlushChunkRequestPB &ctrl_buf, void *buf)
{
   ctrl_buf.SerializeToArray(buf, ctrl_buf.ByteSize()); 
}
void deserialize_req_flat(FlatLogFileFlushChunkRequestPB &ctrl_buf, void *buf, size_t size)
{
   ctrl_buf.ParseFromArray(buf, size);
}
void serialize_resp_flat(FlatLogFileFlushChunkResponsePB &ctrl_buf, void *buf)
{
   ctrl_buf.SerializeToArray(buf, ctrl_buf.ByteSize());
}
void deserialize_resp_flat(FlatLogFileFlushChunkResponsePB &ctrl_buf, void *buf, size_t size)
{
   ctrl_buf.ParseFromArray(buf, size);
}

ChunkIdTypeCJPB chunkid;
IONicePB ionice;
AccelerateOpenOptionsPB OpenChunkOptions;
ChunkOptionsPB ChunkOptions;
MirrorArgsPB mirror;
ReplicaLocationPB *ChunkLocation;
PanguTraceSpanInfo trace_span;
CsChooseDiskStrategyCJPB Strategy;
CsChooseDiskStrategySingleCJPB *Strategies;
ChunkOrStreamIdInfoPB ChunkIdV2;
UUIDInfoPB uuid, uuid2, uuid3;
QosOptionsPB qosOptions;
FlushChecksumPB Checksum;
RootServerIdPB root_server_id;
static FlatLogFileFlushChunkRequestPB ctrl_req;

FlatLogFileFlushChunkRequestPB get_flat_req(int FLAGS_data_len)
{
   // temp = "";
   // for (int i = 0; i < FLAGS_data_len / 2; i++)
   //    temp += "as";
   temp = std::string(FLAGS_data_len,'h');
   chunkid.set_belongedfileid(123123);
   chunkid.set_chunkindex(123123);
   chunkid.set_padding(123123);

   ionice.set_ioclass(123123);
   ionice.set_priority(123123);

   OpenChunkOptions.set_chunksizeexponent(123123);
   OpenChunkOptions.set_checksumcategory(123123);

   ChunkOptions.set_ftttype(123123);
   ChunkOptions.set_placement(temp);
   OpenChunkOptions.set_allocated_chunkoptions(&ChunkOptions);

   mirror.set_copy(123123);
   mirror.set_ftt(123123);
   ChunkOptions.set_allocated_mirror(&mirror);
   ChunkLocation = OpenChunkOptions.add_chunklocation();
   ChunkLocation->set_disklocation(123123);
   ChunkLocation->set_disktype(123123);
   ChunkLocation->set_cslocation(temp);

   trace_span.set_spanid(123123);
   trace_span.set_traceidhigh(123123);
   trace_span.set_traceidlow(123123);

   Strategies = Strategy.add_strategies();
   Strategies->set_method(123123);
   Strategies->set_weight(123123);
   Strategies->set_parameter(123123);
   Strategy.set_diskstoragetype(123123);
   Strategy.set_targetstoragetype(123123);

   ChunkIdV2.set_partition_index(123123);
   ChunkIdV2.set_padding(123123);

   uuid.set_high(123123);
   uuid.set_low(123123);
   uuid2 = uuid;
   uuid3 = uuid;
   ChunkIdV2.set_allocated_uuid(&uuid);

   qosOptions.set_serviceid(123123);
   qosOptions.set_flowid(123123);
   qosOptions.set_adjust(123123);
   qosOptions.set_cachegroupid(123123);
   qosOptions.set_networkpriority(123123);
   // const google::protobuf::EnumDescriptor* ptr = qosOptions.AdjustType_descriptor();
   // qosOptions.DEFAULT;

   Checksum.set_category(123123);
   Checksum.set_value(123123);

   root_server_id.set_allocated_uuid(&uuid2);
   root_server_id.set_leader_term(123123);

   ctrl_req.set_allocated_chunkid(&chunkid);
   ctrl_req.set_offset(123123);
   ctrl_req.set_length(123123);
   ctrl_req.set_storageid(123123);
   ctrl_req.set_data(temp);
   ctrl_req.set_hint(123123);
   ctrl_req.set_isreplication(false);
   ctrl_req.set_trace_open(false);
   ctrl_req.set_allocated_ionice(&ionice);
   ctrl_req.set_allocated_openchunkoptions(&OpenChunkOptions);
   ctrl_req.set_openmode(123123);
   ctrl_req.set_handle(123123);
   ctrl_req.set_datasync(false);
   ctrl_req.set_allocated_trace_span(&trace_span);
   ctrl_req.set_createifnotexists(false);
   ctrl_req.set_allocated_strategy(&Strategy);
   ctrl_req.set_allocated_chunkidv2(&ChunkIdV2);
   ctrl_req.set_allocated_qosoptions(&qosOptions);
   ctrl_req.set_allocated_checksum(&Checksum);
   ctrl_req.set_allocated_volumeuuid(&uuid3);
   ctrl_req.set_moduleversion(123123);
   ctrl_req.set_hostname("https://192.168.40.139:10889\n");
   ctrl_req.set_diskid(123123);
   ctrl_req.set_requestid(123123);
   ctrl_req.set_userid("4f062dd5-05e5-48e3-821e-35e46ca028ce");
   ctrl_req.set_allocated_root_server_id(&root_server_id);
   ctrl_req.set_requiredmincsversion(123123);
   ctrl_req.set_fromtype(123123);
   ctrl_req.set_offset64(123123);
   ctrl_req.set_recordsize(123123);
   ctrl_req.set_poolid(123123);
   ctrl_req.set_storagepoolconfversion(123123);
   ctrl_req.set_a(123123);

   return ctrl_req;
}

FlatLogFileFlushChunkResponsePB get_flat_resp(int FLAGS_data_len)
{
   // if (temp == "")
   // {
   //    for (int i = 0; i < FLAGS_data_len / 2; i++)
   //       temp += "as";
   // }
      temp = std::string(FLAGS_data_len,'h');

   FlatLogFileFlushChunkResponsePB ctrl_resp;
   ctrl_resp.set_errorcode(123123);
   ctrl_resp.set_chunkstatus(123123);
   ctrl_resp.set_hint(123123);
   ctrl_resp.set_acklength(123123);
   ctrl_resp.set_trace_type(123123);
   ctrl_resp.set_trace_data(temp);
   ctrl_resp.set_handle(123123);
   ctrl_resp.set_storageid(123123);
   ctrl_resp.set_diskid(123123);
   ctrl_resp.set_requestid(123123);
   ctrl_resp.set_checksum(123123);
   ctrl_resp.set_leftreplicasize(123123);
   ctrl_resp.set_a(123123);

   return ctrl_resp;
}
