# API Example Android

*English | [中文](README.zh.md)*

This project presents you a set of API examples to help you understand how to use Agora APIs.

## Prerequisites

- DevEco Studio 4.1+
- Physical HarmonyOS NEXT device

## Quick Start

This section shows you how to prepare, build, and run the sample application.

### Obtain an App Id

To build and run the sample application, get an App Id:

1. Create a developer account at [agora.io](https://dashboard.agora.io/signin/). Once you finish the signup process, you will be redirected to the Dashboard.
2. Navigate in the Dashboard tree on the left to **Projects** > **Project List**.
3. Save the **App Id** from the Dashboard for later use.
4. Save the **App Certificate** from the Dashboard for later use.

5. Open `HarmonyOS_NEXT/APIExample` and edit the `entry/src/main/ets/common/KeyCenter.ets` file. Update `YOUR APP ID` with your App Id, update `YOUR APP CERTIFICATE` with the main app certificate from dashboard. Note you can leave the certificate variable `null` if your project has not turned on security token.

    ```
    // Agora APP ID.
    export const AppID: String = YOUR APP ID
    // Agora APP Certificate. If the project does not have certificates enabled, leave this field blank.
    // PS：It is unsafe to place the App Certificate on the client side, it is recommended to place it on the server side to ensure that the App Certificate is not leaked.
    export const AppCertificate: String = YOUR APP CERTIFICATE
    ```

6. Download the har package from the [official website](https://download.agora.io/sdk/release/Agora_Native_SDK_for_HarmonyOS_v4.4.1_FULL.har) and rename`AgoraRtcSdk.har`,then copy it to `HarmonyOS_NEXT/APIExample/entry/libs/AgoraRtcSdk.har`
7. config the signature information in `build-profile.json5` under the root directory with your own

You are all set. Now connect your HarmonyOS NEXT device and run the project.

## Contact Us

- For potential issues, take a look at our [FAQ](https://docs.agora.io/en/faq) first
- Dive into [Agora SDK Samples](https://github.com/AgoraIO) to see more tutorials
- Take a look at [Agora Use Case](https://github.com/AgoraIO-usecase) for more complicated real use case
- Repositories managed by developer communities can be found at [Agora Community](https://github.com/AgoraIO-Community)
- You can find full API documentation at [Document Center](https://docs.agora.io/en/)
- If you encounter problems during integration, you can ask question in [Stack Overflow](https://stackoverflow.com/questions/tagged/agora.io)
- You can file bugs about this sample at [issue](https://github.com/AgoraIO/API-Examples/issues)

## License

The MIT License (MIT)
