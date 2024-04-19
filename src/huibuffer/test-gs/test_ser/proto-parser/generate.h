struct P2P_Interface
{
	const static uint32_t IID = 0;
};
struct geete: public P2P_Interface {
	struct ChunkIdTypeCJPB {
			uint64_t BelongedFileId;
			uint32_t ChunkIndex;
			uint32_t Padding;
	};
	struct IONicePB {
			int32_t Priority;
			int32_t IOClass;
	};
	struct MirrorArgsPB {
			int32_t Copy;
			int32_t FTT;
	};
	struct ChunkOptionsPB {
			int32_t FTTType;
			MirrorArgsPB Mirror;
			RPC::string Placement;
	};
	struct ReplicaLocationPB {
			RPC::string CSLocation;
			int32_t diskLocation;
			uint32_t diskType;
	};
	struct AccelerateOpenOptionsPB {
			ChunkOptionsPB ChunkOptions;
			RPC::array<ReplicaLocationPB> ChunkLocation;
			uint32_t chunkSizeExponent;
			uint32_t checksumCategory;
	};
	struct PanguTraceSpanInfo {
			uint64_t TraceIdLow;
			uint64_t TraceIdHigh;
			uint64_t SpanId;
	};
	struct CsChooseDiskStrategySingleCJPB {
			uint32_t Method;
			uint32_t Weight;
			uint64_t Parameter;
	};
	struct CsChooseDiskStrategyCJPB {
			RPC::array<CsChooseDiskStrategySingleCJPB> Strategies;
			int32_t DiskStorageType;
			int32_t TargetStorageType;
	};
	struct UUIDInfoPB {
			uint64_t low;
			uint64_t high;
	};
	struct ChunkOrStreamIdInfoPB {
			UUIDInfoPB uuid;
			uint32_t partition_index;
			uint32_t padding;
	};
	struct QosOptionsPB {
			uint32_t serviceId;
			uint32_t flowId;
			int32_t adjust;
			uint32_t cacheGroupId;
			int32_t networkPriority;
	};
	struct FlushChecksumPB {
			uint32_t category;
			int64_t value;
	};
	struct RootServerIdPB {
			UUIDInfoPB uuid;
			int64_t leader_term;
	};
	struct SayHelloAll {
		const static uint32_t FID = 0;
		struct request: public RPC::message{
			ChunkIdTypeCJPB ChunkId;
			uint32_t Offset;
			uint32_t Length;
			uint32_t StorageId;
			RPC::string Data;
			uint32_t Hint;
			bool IsReplication;
			bool trace_open;
			IONicePB IONice;
			AccelerateOpenOptionsPB OpenChunkOptions;
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
			RPC::string HostName;
			uint32_t DiskId;
			uint64_t RequestId;
			RPC::string UserId;
			RootServerIdPB root_server_id;
			uint32_t RequiredMinCSVersion;
			int32_t FromType;
			uint64_t Offset64;
			uint32_t RecordSize;
			uint32_t PoolId;
			uint64_t StoragePoolConfVersion;
			PROCESS_FIELDS(ChunkId,Offset,Length,StorageId,Data,Hint,IsReplication,trace_open,IONice,OpenChunkOptions,OpenMode,Handle,DataSync,trace_span,CreateIfNotExists,Strategy,ChunkIdV2,qosOptions,Checksum,volumeUUID,ModuleVersion,HostName,DiskId,RequestId,UserId,root_server_id,RequiredMinCSVersion,FromType,Offset64,RecordSize,PoolId,StoragePoolConfVersion);
		};
		struct response: public RPC::message{
			int32_t Errorcode;
			uint32_t ChunkStatus;
			uint32_t Hint;
			uint32_t AckLength;
			uint32_t trace_type;
			RPC::string trace_data;
			int64_t Handle;
			uint32_t StorageId;
			uint32_t DiskId;
			uint64_t RequestId;
			int64_t Checksum;
			uint64_t LeftReplicaSize;
			PROCESS_FIELDS(Errorcode,ChunkStatus,Hint,AckLength,trace_type,trace_data,Handle,StorageId,DiskId,RequestId,Checksum,LeftReplicaSize);
		};
	};
}
