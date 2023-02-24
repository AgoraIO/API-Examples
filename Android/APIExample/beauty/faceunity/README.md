# Configuration Guide
*English | [中文](README.zh.md)*

This example provides two FaceUnity beauty access methods, single input (NV21 only) and dual input (NV21 and texture). Dual input has less NV21 to texture operation than single input, and the conversion performance is relatively high, but on low-end machines, the sticker may not follow the face, etc. It is recommended to choose the appropriate access method according to the customer.
> The plug-in cloud market provides a more convenient integration method. You can log in to the sound network [console](https://console.agora.io/) to view the [integration tutorial](https://console.agora.io/marketplace/license/introduce?serviceName=faceunity-ar)


## 3 License

Contact sales@agora.io and get a licence file `authpack.java`, then copy this file to project folder `beauty/faceunity/src/main/java/com/faceunity/nama/authpack.java`. Note this licence determines which FaceUnity functions/effects you are allowed to use.


## 4 Configure compilation

- Set the **beauty_faceunity** variable in **gradle.properties** in the project directory to true
- compile and run


