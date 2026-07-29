import { rcp } from '@kit.RemoteCommunicationKit';
import { systemDateTime } from '@kit.BasicServicesKit';
import { AppCertificate, AppID } from '../common/KeyCenter';
import { JSON, util } from '@kit.ArkTS';
import Logger from './Logger';

export class TokenUtils {
  static async genRtcToken(channelName: string, uid: string) : Promise<string> {
    let token = ''
    try{
      const session = rcp.createSession({
        headers: { 'Content-Type': 'application/json' }
      });
      let body = JSON.stringify({
        "appId": AppID,
        "appCertificate": AppCertificate,
        "channelName": channelName,
        "expire": 900, //s
        "src": "HMOS",
        "ts": systemDateTime.getTime(false),
        "type": 1, // 1: RTC Token ; 2: RTM Token
        "uid": uid
      })
      let response = await session.post("https://toolbox.bj2.agoralab.co/v1/token/generate", body)
      Logger.info(`Response complete: statueCode=${response.statusCode}`);
      let decoder = util.TextDecoder.create('utf-8');
      let stringData = decoder.decodeWithStream(new Uint8Array(response.body));
      let respParsed = JSON.parse(stringData) as GenRtcTokenResp
      Logger.info(`Response succeeded: ${stringData}`);
      if(respParsed.data.token != undefined){
        token = respParsed.data.token
      }
    } catch (e) {
      Logger.error(`Response err: Code is ${e.code}, message is ${JSON.stringify(e)}`);
    }
    return token
  }
}

export interface GenRtcTokenData {
  token: string | undefined,
}

export interface GenRtcTokenResp {
  code: number,
  data: GenRtcTokenData,
  msg: string,
  tip: string
}

