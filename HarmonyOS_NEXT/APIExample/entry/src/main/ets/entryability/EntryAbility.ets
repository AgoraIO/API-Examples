import { AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';
import Logger from '../util/Logger';


const TAG: string = 'EntryAbility'

export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    Logger.info(TAG, 'onCreate')
  }

  onDestroy(): void {
    Logger.info(TAG, 'onDestroy')
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    // Main window is created, set main page for this ability
    Logger.info(TAG, 'onWindowStageCreate')

    try {
      let windowClass: window.Window;
      window.getLastWindow(this.context, (err, data) => {
        if (err.code) {
          Logger.error('Failed to obtain the top window. Cause: ' + JSON.stringify(err));
          return;
        }
        Logger.info('Succeeded in obtaining the top window. Data: ' + JSON.stringify(data));
        windowClass = data;
        let systemBarProperties: window.SystemBarProperties = {
          statusBarColor: "#f1f3f5"
        };
        windowClass.setWindowSystemBarProperties(systemBarProperties, (err) => {
          if (err.code) {
            Logger.error('Failed to set the system bar properties. Cause: ' + JSON.stringify(err));
            return;
          }
          Logger.info('Succeeded in setting the system bar properties.');
        })
      })
    } catch (exception) {
      Logger.error('Failed to obtain the top window. Cause: ' + JSON.stringify(exception));
    }

    windowStage.loadContent('pages/Index', (err, data) => {
      if (err) {
        Logger.error(TAG, `Failed to load the content. Cause: ${JSON.stringify(err)}`)
        return
      }
      Logger.info(TAG, `Succeeded in loading the content. Data: ${JSON.stringify(data)}`)
    })
  }

  onWindowStageDestroy(): void {
    // Main window is destroyed, release UI related resources
    Logger.info(TAG, 'onWindowStageDestroy')
  }

  onForeground(): void {
    // Ability has brought to foreground
    Logger.info(TAG, 'onForeground')
  }

  onBackground(): void {
    // Ability has back to background
    Logger.info(TAG, 'onBackground')
  }
}
