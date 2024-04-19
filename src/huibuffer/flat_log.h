#pragma once
#include <iostream>
#include "serialization.h"

namespace erpc
{

        struct ChunkIdTypeCJPB
        {
                uint64_t BelongedFileId;
                uint32_t ChunkIndex;
                uint32_t Padding;
        };
        struct IONicePB
        {
                int32_t Priority;
                int32_t IOClass;
        };
        struct MirrorArgsPB
        {
                int32_t Copy;
                int32_t FTT;
        };
        struct ChunkOptionsPB : public Message
        {
                int32_t FTTType;
                MirrorArgsPB Mirror;
                string Placement;
                template <typename AR>
                void serialize_fields(AR &ar)
                {
                        ar.process_field(Placement);
                }
        };
        struct ReplicaLocationPB : public Message
        {
                string CSLocation;
                int32_t diskLocation;
                uint32_t diskType;
                template <typename AR>
                void serialize_fields(AR &ar)
                {
                        ERPC_INFO("ar.process_field(CSLocation);\n");
                        
                        ar.process_field(CSLocation);
                }
        };
        struct AccelerateOpenOptionsPB : public Message
        {
                ChunkOptionsPB ChunkOptions;
                array<ReplicaLocationPB> ChunkLocation;
                uint32_t chunkSizeExponent;
                uint32_t checksumCategory;

                template <typename AR>
                void serialize_fields(AR &ar)
                {
                        ERPC_INFO("ChunkOptions.serialize_fields(ar);\n");
                        ChunkOptions.serialize_fields(ar);
                        ar.process_field((buffer &)ChunkLocation);
                        ERPC_INFO("finish\n");
                        ERPC_INFO("ChunkLocation.size() = %d, ChunkLocation._ptr = 0x%X, ChunkLocation._len = %d, sizeof(ReplicaLocationPB) = %d\n, ", 
                                ChunkLocation.size(), ChunkLocation._ptr, ChunkLocation._len, sizeof(ReplicaLocationPB));
                        for (int i=0; i<ChunkLocation.size(); ++i){
                                ERPC_INFO("rep.serialize_fields(ar);\n");
                                const_cast<ReplicaLocationPB*>(ChunkLocation.begin())->serialize_fields(ar);
                        }
                        ERPC_INFO("finish\n");
                        
                }
        };
        struct PanguTraceSpanInfo
        {
                uint64_t TraceIdLow;
                uint64_t TraceIdHigh;
                uint64_t SpanId;
        };
        struct CsChooseDiskStrategySingleCJPB
        {
                uint32_t Method;
                uint32_t Weight;
                uint64_t Parameter;
        };
        struct CsChooseDiskStrategyCJPB : public Message
        {
                array<CsChooseDiskStrategySingleCJPB> Strategies;
                int32_t DiskStorageType;
                int32_t TargetStorageType;
                template <typename AR>
                void serialize_fields(AR &ar)
                {
                        ERPC_INFO("ar.process_field((buffer &)Strategies);\n");
                        ar.process_field((buffer &)Strategies);
                        ERPC_INFO("finish\n");
                }
        };
        struct UUIDInfoPB
        {
                uint64_t low;
                uint64_t high;
        };
        struct ChunkOrStreamIdInfoPB
        {
                UUIDInfoPB uuid;
                uint32_t partition_index;
                uint32_t padding;
        };
        struct QosOptionsPB
        {
                uint32_t serviceId;
                uint32_t flowId;
                int32_t adjust;
                uint32_t cacheGroupId;
                int32_t networkPriority;
        };
        struct FlushChecksumPB
        {
                uint32_t category;
                int64_t value;
        };
        struct RootServerIdPB
        {
                UUIDInfoPB uuid;
                int64_t leader_term;
        };
        struct request : public Message
        {
                int32_t a;
                ChunkIdTypeCJPB ChunkId;
                uint32_t Offset;
                uint32_t Length;
                uint32_t StorageId;
                string Data;
                uint32_t Hint;
                bool IsReplication;
                bool trace_open;
                IONicePB IONice;
                AccelerateOpenOptionsPB OpenChunkOptions; // neated has string
                int32_t OpenMode;
                int64_t Handle;
                bool DataSync;
                PanguTraceSpanInfo trace_span;
                bool CreateIfNotExists;
                CsChooseDiskStrategyCJPB Strategy;
                ChunkOrStreamIdInfoPB ChunkIdV2;
                QosOptionsPB qosOptions;
                FlushChecksumPB Checksum;
                UUIDInfoPB volumeUUID;
                uint32_t ModuleVersion;
                string HostName;
                uint32_t DiskId;
                uint64_t RequestId;
                string UserId;
                RootServerIdPB root_server_id;
                uint32_t RequiredMinCSVersion;
                int32_t FromType;
                uint64_t Offset64;
                uint32_t RecordSize;
                uint32_t PoolId;
                uint64_t StoragePoolConfVersion;
                PROCESS_FIELDS(Data, OpenChunkOptions, Strategy, HostName, UserId)
        };
        struct response : public Message
        {
                int32_t a;
                int32_t Errorcode;
                uint32_t ChunkStatus;
                uint32_t Hint;
                uint32_t AckLength;
                uint32_t trace_type;
                string trace_data;
                int64_t Handle;
                uint32_t StorageId;
                uint32_t DiskId;
                uint64_t RequestId;
                int64_t Checksum;
                uint64_t LeftReplicaSize;
                PROCESS_FIELDS(trace_data)
        };
}