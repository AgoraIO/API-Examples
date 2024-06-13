import { abilityAccessCtrl, bundleManager, common, Permissions } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import Logger from './Logger';

const TAG = "PermissionHelper"

class PermissionHelper {
  async checkPermissions(context: common.UIAbilityContext, permissions: Array<Permissions>): Promise<boolean> {
    let atManager = abilityAccessCtrl.createAtManager();
    let grant: boolean = false;
    // let permissions: Array<Permissions> = [
    //   'ohos.permission.MICROPHONE',
    //   "ohos.permission.CAMERA"
    // ];

    // 获取应用程序的accessTokenID
    let tokenId: number = 0;
    try {
      let bundleInfo: bundleManager.BundleInfo = await bundleManager.getBundleInfoForSelf(bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION);
      let appInfo: bundleManager.ApplicationInfo = bundleInfo.appInfo;
      tokenId = appInfo.accessTokenId;
    } catch (error) {
      let err: BusinessError = error as BusinessError;
      Logger.error(TAG, `Failed to get bundle info for self. Code is ${err.code}, message is ${err.message}`);
    }

    let pending: Array<Permissions> = new Array<Permissions>();

    for (let i = 0; i < permissions.length; i++) {
      let permission = permissions[i];
      // 校验应用是否被授予权限
      try {
        grant = await atManager.checkAccessToken(tokenId, permission) == abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED;
      } catch (error) {
        let err: BusinessError = error as BusinessError;
        Logger.error(`Failed to check access token. Code is ${err.code}, message is ${err.message}`);
      }
      if (!grant) {
        pending.push(permission)
      }
    }

    if (pending.length > 0) {
      let data = await atManager.requestPermissionsFromUser(context, pending)
      let grantStatusList: Array<number> = data.authResults;
      let length: number = grantStatusList.length;
      for (let i = 0; i < length; i++) {
        if (grantStatusList[i] === abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
          // 用户授权，可以继续访问目标操作
          grant = true
        } else {
          // 用户拒绝授权，提示用户必须授权才能访问当前页面的功能，并引导用户到系统设置中打开相应的权限
          Logger.info(TAG, 'Permission denied');
          grant = false
          break
        }
      }
    }

    return grant;
  }
}

export default new PermissionHelper()