// SharedArrayBuffer-based communication
const sharedMemory = new SharedArrayBuffer(10 * 1024 * 1024); // 10MB
const communicationBuffer = new DataView(sharedMemory);

// Stage communication protocol
class StageMessenger {
    writeStageResult(stageId: number, offset: number, data: Uint8Array) {
        communicationBuffer.setUint32(stageId * 8, offset, true);
        communicationBuffer.setUint32(stageId * 8 + 4, data.length, true);
        new Uint8Array(sharedMemory, offset, data.length).set(data);
    }
}
