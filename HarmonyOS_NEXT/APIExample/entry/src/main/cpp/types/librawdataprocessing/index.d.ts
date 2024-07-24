export class OriginAudioData {
  constructor(arg: bigint);
  enable: (enable:boolean) => number;
}

export class OriginVideoData {
  constructor(arg: bigint);
  enable: (enable:boolean) => number;
  takeSnapshot: () => number;
}

export class MediaMetadata {
  constructor(arg: bigint);
  enable: (enable:boolean) => number;
  send: (data: string) => number;
}