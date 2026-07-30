export class OriginAudioData {
  constructor(arg: bigint);
  enable: (enable:boolean) => number;
}

export class CustomAudioRender {
  constructor(arg: bigint);
  enable: (enable:boolean) => number;
}

export class OriginVideoData {
  constructor(arg: bigint);
  enable: (enable:boolean) => number;
  takeSnapshot: (callback: (frame: SnapshotFrame) => void) => number;
}

export interface SnapshotFrame {
  data: ArrayBuffer;
  width: number;
  height: number;
  rotation: number;
  errorCode: number;
}

export class MediaMetadata {
  constructor(arg: bigint);
  enable: (enable:boolean, onMetadataReceived?: (data: string) => void) => number;
  send: (data: string) => number;
}
