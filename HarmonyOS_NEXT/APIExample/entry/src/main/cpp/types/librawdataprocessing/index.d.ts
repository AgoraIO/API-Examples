export const setNativeEngineHandle: (handle:bigint) => number;
export const unsetNativeEngineHandle: () => number;
export const enableAudioFrameObserver:(enable:boolean) => void;
export const enableVideoFrameObserver:(enable:boolean) => void;

export class OriginAudioData {
  constructor(arg: bigint);

  enable: (enable:boolean) => number;
}

export class OriginVideoData {
  constructor(arg: bigint);

  enable: (enable:boolean) => number;

  takeSnapshot: () => number;
}