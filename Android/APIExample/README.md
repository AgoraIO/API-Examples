# API Example Android

*English | [中文](README.zh.md)*

This project presents you a set of API examples to help you understand how to use Agora APIs.

## Prerequisites

- Android Studio 3.0+
- Physical Android device
- Android simulator is supported

## Quick Start

This section shows you how to prepare, build, and run the sample application.

### Obtain an App Id

To build and run the sample application, get an App Id:

1. Create a developer account at [agora.io](https://dashboard.agora.io/signin/). Once you finish the signup process, you will be redirected to the Dashboard.
2. Navigate in the Dashboard tree on the left to **Projects** > **Project List**.
3. Save the **App Id** from the Dashboard for later use.
4. Generate a temp **Access Token** (valid for 24 hours) from dashboard page with given channel name, save for later use.

5. Open `Android/APIExample` and edit the `app/src/main/res/values/string_config.xml` file. Update `<#Your App Id#>` with your App Id, and change `<#Temp Access Token#>` with the temp Access Token generated from dashboard. Note you can leave the token variable `null` if your project has not turned on security token.

    ```
    <string name="agora_app_id" translatable="false">YOUR APP ID</string>
    // assign token to null if you have not enabled app certificate
    <string name="agora_access_token" translatable="false">YOUR ACCESS TOKEN</string>
    ```

You are all set. Now connect your Android device and run the project.



## Contact Us

- For bugs and problems, please email us(bd@lighthk.com)

## License

The MIT License (MIT)
