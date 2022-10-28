//
//  Agora Rtm SDK
//
//  Created by junhao Wang in 2018-05.
//  Copyright (c) 2018 Agora IO. All rights reserved.
//
#pragma once

#include <stdint.h>

namespace agora {

namespace rtm {

enum PEER_MESSAGE_STATE {
  PEER_MESSAGE_INIT = 0,
  PEER_MESSAGE_FAILURE = 1,
  PEER_MESSAGE_PEER_UNREACHABLE = 2,
  PEER_MESSAGE_RECEIVED_BY_PEER = 3,
  PEER_MESSAGE_SENT_TIMEOUT = 4,
};
/**
 * The login error code.
 */
enum LOGIN_ERR_CODE {
  /**
   * 0: Login succeeds. No error occurs.
   */
  LOGIN_ERR_OK = 0,
  /**
   * 1: Login fails for reasons unknown.
   */
  LOGIN_ERR_UNKNOWN = 1,
  /**
   * 2: The server rejects the login, either because the user has already logged in, or because
   * the RTM service is not initialized.
   */
  LOGIN_ERR_REJECTED = 2,  // ALREADY LOGIN OR NOT INITIALIZED, SERVER REJECT
  /**
   * 3: Invalid login arguments.
   */
  LOGIN_ERR_INVALID_ARGUMENT = 3,
  /**
   * 4: The App ID is invalid.
   */
  LOGIN_ERR_INVALID_APP_ID = 4,
  /**
   * 5: The token is invalid.
   */
  LOGIN_ERR_INVALID_TOKEN = 5,
  /**
   * 6: The login is rejected because the token has expired.
   */
  LOGIN_ERR_TOKEN_EXPIRED = 6,
  /**
   * 7: Authentication of the RTMP token fails.
   */
  LOGIN_ERR_NOT_AUTHORIZED = 7,
  /**
   * 8: The login times out. The current timeout is set as six seconds.
   */
  LOGIN_ERR_TIMEOUT = 8,
};
/**
 * The logout error code.
 */
enum LOGOUT_ERR_CODE {
  /**
   * 0: Logout succeeds. No error occurs.
   */
  LOGOUT_ERR_OK = 0,
  /**
   * 1: Logout fails.
   */
  LOGOUT_ERR_REJECTED = 1,
};
/**
 * The connection state.
 */
enum CONNECTION_STATE {
  /**
   * 1: The SDK has logged in the RTM service.
   */
  CONNECTION_STATE_CONNECTED = 1,
  /**
   * 2: The initial state. The SDK is disconnected from the RTM service.
   */
  CONNECTION_STATE_DISCONNECTED = 2,
  /**
   * 3: The SDK gives up logging in the RTM service, mainly because another instance has logged in the RTM
   * service with the same user ID.
   *
   * Call the logout() method before calling login to log in the RTM service again.
   */
  CONNECTION_STATE_ABORTED = 3,
};

/**
 * The state of the channel message.
 */
enum CHANNEL_MESSAGE_STATE {
  /**
   * 1: The channel message is received by the server.
   */
  CHANNEL_MESSAGE_RECEIVED_BY_SERVER = 1,
  /**
   * 3: The SDK has not received a response from the server in five seconds. The current timeout is set as
   * five seconds.
   */
  CHANNEL_MESSAGE_SENT_TIMEOUT = 3,
};
/**
 * The join channel error.
 */
enum JOIN_CHANNEL_ERR {
  /**
   0: The method call succeeds, or the user joins the channel successfully.
  */
  JOIN_CHANNEL_ERR_OK = 0,

  /**
   1: Common failure. The user fails to join the channel.
  */
  JOIN_CHANNEL_ERR_FAILURE = 1,

  /**
   2: **RESERVED FOR FUTURE USE**
  */
  JOIN_CHANNEL_ERR_REJECTED = 2, // Usually occurs when the user is already in the channel

  /**
   3: The user fails to join the channel because the argument is invalid.
  */
  JOIN_CHANNEL_ERR_INVALID_ARGUMENT = 3,

  /**
   4: A timeout occurs when joining the channel. The current timeout is set as five seconds. Possible reasons: The user is in the \ref agora::rtm::CONNECTION_STATE_ABORTED "CONNECTION_STATE_ABORTED" state.
  */
  JOIN_CHANNEL_TIMEOUT = 4,

  /**
   5: The number of the RTM channels you are in exceeds the limit of 20.
  */
  JOIN_CHANNEL_ERR_EXCEED_LIMIT = 5,

  /**
   6: The user is joining or has joined the channel.
  */
  JOIN_CHANNEL_ERR_ALREADY_JOINED = 6,

  /**
   7: The method call frequency exceeds 50 queries every three seconds.
  */
  JOIN_CHANNEL_ERR_TOO_OFTEN = 7,

  /**
   8: The frequency of joining the same channel exceeds two times every five seconds.
  */
  JOIN_CHANNEL_ERR_JOIN_SAME_CHANNEL_TOO_OFTEN = 8,

  /**
   101: \ref agora::rtm::IRtmService "IRtmService" is not initialized.
  */
  JOIN_CHANNEL_ERR_NOT_INITIALIZED = 101,

  /**
   102: The user does not call the \ref agora::rtm::IRtmService::login "login" method, or the method call of \ref agora::rtm::IRtmService::login "login" does not succeed before joining the channel.
  */
  JOIN_CHANNEL_ERR_USER_NOT_LOGGED_IN = 102,
};
/**
 @brief Error codes related to leaving a channel.
  */
enum LEAVE_CHANNEL_ERR {

  /**
   0: The method call succeeds, or the user leaves the channel successfully.
    */
  LEAVE_CHANNEL_ERR_OK = 0,

  /**
   1: Common failure. The user fails to leave the channel.
    */
  LEAVE_CHANNEL_ERR_FAILURE = 1,

  /**
   2: **RESERVED FOR FUTURE USE**
    */
  LEAVE_CHANNEL_ERR_REJECTED = 2,

  /**
   3: The user is not in the channel.
    */
  LEAVE_CHANNEL_ERR_NOT_IN_CHANNEL = 3,

  /**
   101: \ref agora::rtm::IRtmService "IRtmService" is not initialized.
    */
  LEAVE_CHANNEL_ERR_NOT_INITIALIZED = 101,

  /**
   102: The user does not call the \ref agora::rtm::IRtmService::login "login" method, or the method call of \ref agora::rtm::IRtmService::login "login" does not succeed before calling the \ref agora::rtm::IChannel::leave "leave" method.
    */
  LEAVE_CHANNEL_ERR_USER_NOT_LOGGED_IN = 102,
};

/**
 * The reason for a user to leave the channel.
 */
enum LEAVE_CHANNEL_REASON {
  /**
   * 1: The user quits the channel.
   */
  LEAVE_CHANNEL_REASON_QUIT = 1,
  /**
   * 2: The user is kicked off the channel.
   */
  LEAVE_CHANNEL_REASON_KICKED = 2,
};
/**
 @brief Error codes related to sending a channel message.
  */
enum CHANNEL_MESSAGE_ERR_CODE {

  /**
   0: The method call succeeds, or the server receives the channel message.
    */
  CHANNEL_MESSAGE_ERR_OK = 0,

  /**
   1: Common failure. The user fails to send the channel message.
    */
  CHANNEL_MESSAGE_ERR_FAILURE = 1,

  /**
   2: The SDK does not receive a response from the server in 10 seconds. The current timeout is set as 10 seconds. Possible reasons: The user is in the \ref agora::rtm::CONNECTION_STATE_ABORTED "CONNECTION_STATE_ABORTED" state.
    */
  CHANNEL_MESSAGE_ERR_SENT_TIMEOUT = 2,

  /**
   3: The method call frequency exceeds the limit of (RTM SDK for Windows C++) 180 calls every three seconds or (RTM SDK for Linux C++) 1500 calls every three seconds, with channel and peer messages taken together..
    */
  CHANNEL_MESSAGE_ERR_TOO_OFTEN = 3,

  /**
   4: The message is null or exceeds 32 KB in length.
    */
  CHANNEL_MESSAGE_ERR_INVALID_MESSAGE = 4,

  /**
   101: \ref agora::rtm::IRtmService "IRtmService" is not initialized.
    */
  CHANNEL_MESSAGE_ERR_NOT_INITIALIZED = 101,

  /**
   102: The user does not call the \ref agora::rtm::IRtmService::login "login" method, or the method call of \ref agora::rtm::IRtmService::login "login" does not succeed before sending out a channel message.
    */
  CHANNEL_MESSAGE_ERR_USER_NOT_LOGGED_IN = 102,
};

/**
 * The response code.
 */
enum RESPONSE_CODE {
  /**
   * 1: The response
   */
  RESPONSE_CODE_SUCCESS = 1,
  RESPONSE_CODE_FAILURE = 2,
};
/**
 * The message type.
 */
enum MESSAGE_TYPE {
  /**
   * 0: The message type is undefined.
   */
  MESSAGE_TYPE_UNDEFINED = 0,
  /**
   * 1: A text message.
   */
  MESSAGE_TYPE_TEXT = 1,
  /**
   * 2: A raw message in binary, for example, audio data, or video data.
   */
  MESSAGE_TYPE_BINARY = 2,
  /**
   * 4: A converge message.
   */
  MESSAGE_TYPE_CONVERGE = 4,
};
/**
 * The IMessage class.
 */
class IMessage {
 public:
  /**
   * Creates an IMessage instance.
   *
   * The created IMessage instance can be either for a channel message or for a peer-to-peer message.
   *
   * You can set the content of the text message later using the setText() method.
   * @return
   * - An empty text IMessage instance, if the method call succeeds.
   */
  static IMessage *createMessage();
  virtual ~IMessage() {}
  /**
   * Gets the ID of the message.
   * @return The ID of the current IMessage instance.
   */
  virtual int64_t getMessageId() const = 0;
  /**
   * Gets the message type.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getMessageType() const = 0;
  /**
   * Sets the content of the text message, or the text description of the raw message.
   * @param str The content of the text message, or the text description of the raw message. The maximum length
   * is 32 KB.
   */
  virtual void setText(const char *str) = 0;
  /**
   * Gets the content of the text messsage, or the text description of the raw message.
   * @return
   * - The content of the text message or the text description of the raw message.
   */
  virtual const char *getText() const = 0;
  /**
   * Get pointer of custom raw message
   * @return
   * - The content of binary raw message
   */
  virtual const unsigned char *getRawMessageData() const = 0;
  /**
   * Get length of custom raw message
   * @return
   * - The length of binary raw message in bytes
   */
  virtual int getRawMessageLength() const = 0;
  /**
   * Set message type
   */
  virtual void setMessageType(int32_t type) = 0;
  /**
   * Set raw binary message
   */
  virtual void setRawMessage(const uint8_t* data, int length) = 0;
  /**
   * Releases the IMessage instance.
   */
  virtual void release() = 0;
};

/**
 * The IChannelMember class.
 */
class IChannelMember {
 public:
  virtual ~IChannelMember() {}
  /**
   * Gets the ID of the channel member.
   * @return The ID of the channel member.
   */
  virtual const char *getMemberId() const = 0;
  /**
   * Gets the ID of the channel.
   * @return The ID of the channel.
   */
  virtual const char *getChannelId() const = 0;
  /**
   * Releases the IChannelMember class.
   */
  virtual void release() = 0;
};
/**
 * The IChannelAttributes class.
 */
class IChannelAttributes {
 public:
  /**
   * Creates an IChannelAttributes instance.
   * @return
   * - An IChannelAttributes instance, if the method call succeeds.
   */
  static IChannelAttributes *createChannelAttributes();
  /**
   * Adds an attribute to a specified channel.
   *
   * @param key The pointer to the attribute key.
   * @param value The pointer to the attribute value.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int addAttribute(const char *key, const char *value) = 0;
  /**
   * Removes an attribute from the channel.
   * @param key The pointer to the attribute key.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int removeAttribute(const char *key) = 0;
  /**
   * Gets the size of the attributes.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getAttributesSize() const = 0;
  /**
   * Gets the channel attributes.
   * @param size The size of the channel attributes.
   * @param key The pointer to the key of each channel attribute.
   * @param value The pointer to the value of each channel attribute.
   */
  virtual void getAttributes(int size, char **key,
                             char **value) const = 0;  // todo: discussion, how to traveral
  /**
   * Gets the value of a channel attribute using the attribute key.
   * @param key The pointer to the key of the channel attribute that you want to get.
   */
  virtual const char *getAttributeValue(const char *key) const = 0;
  /**
   * Releases the IChannelAttributes instance.
   * @param
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int release() = 0;
};
/**
 * The IChannelEventHandler class.
 */
class IChannelEventHandler {
 public:
  virtual ~IChannelEventHandler() {}
  /**
   * Occurs when the local user successfully joins a channel.
   */
  virtual void onJoinSuccess() {}
  /**
   * Occurs when the local user fails to join a channel.
   * @param errorCode The error code: #JOIN_CHANNEL_ERR.
   */
  virtual void onJoinFailure(JOIN_CHANNEL_ERR errorCode) {}
  /**
   * Occurs when the local user leaves a channel.
   * @param errorCode The error code. See #LEAVE_CHANNEL_ERR.
   */
  virtual void onLeave(LEAVE_CHANNEL_ERR errorCode) {}
  /**
   * Occurs when the local user receives a channel message.
   * @param message The pointer to the messsage: IMessage.
   */
  virtual void onMessageReceived(const char *userId, const IMessage *message) {}
  /**
   * Reports the state of the message sent by the local user.
   * @param messageId ID of the message.
   * @param state The state of the message: #CHANNEL_MESSAGE_STATE.
   */
  virtual void onSendMessageState(int64_t messageId, CHANNEL_MESSAGE_STATE state) {}
  /**
   Returns the result of the \ref agora::rtm::IChannel::sendMessage "sendMessage" method call.

    @param messageId The ID of the sent channel message.
    @param state The error codes. See #CHANNEL_MESSAGE_ERR_CODE.
    */
  virtual void onSendMessageResult(long long messageId, CHANNEL_MESSAGE_ERR_CODE state) {}
  /**
   * Occurs when another member joins the channel.
   * @param member The pointer to the member who joins the channel: IChannelMember.
   */
  virtual void onMemberJoined(IChannelMember *member) {}
  /**
   * Occurs when the other member leaves the channel.
   * @param member The pointer to the member who leaves the channel: IChannelMember.
   */
  virtual void onMemberLeft(IChannelMember *member) {}
  /**
   * Reports all the members in the channel.
   * @param members The pointer to each member in the channel: IChannelMember.
   * @param userCount The number of users in the channel.
   */
  virtual void onMembersGotten(IChannelMember **members, int userCount) {}
  /**
   * Occurs when the channel attributes are updated.
   * @param attributes The pointer to the current channel attributes: IChannelAttributes.
   */
  virtual void onAttributesUpdated(const IChannelAttributes *attributes) {}
  /**
   * Occurs when the local user calls updateAttributes().
   * @param requestId ID of the current attribute update request.
   * @param resCode The response code: #RESPONSE_CODE.
   */
  virtual void onUpdateAttributesResponse(int64_t requestId, RESPONSE_CODE resCode) {}
  /**
   * Occurs when the channel attributes are deleted.
   * @param attributes The pointer to the channel attributes that you want to remove: IChannelAttributes.
   */
  virtual void onAttributesDeleted(const IChannelAttributes *attributes) {}
  /**
   * Occurs when the local user calls deleteAttributes().
   * @param requestId ID of the current attribute delete request.
   * @param resCode The response code: #RESPONSE_CODE.
   */
  virtual void onDeleteAttributesResponse(int64_t requestId, RESPONSE_CODE resCode) {}
};
/**
 * The IChannel class.
 */
class IChannel {
 public:
  /**
   * Sets an event handler for IChannel.
   * @param eventHandler The pointer to the event handler of IChannel: IChannelEventHandler.
   */
  virtual void setEventHandler(IChannelEventHandler *eventHandler) = 0;
  /**
   * Joins the current channel.
   *
   * A successful method call triggers either onJoinSuccess() or onJoinFailure() on the local client, to report
   * the state of joining the channel.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int join() = 0;
  /**
   * Leaves the current channel.
   *
   * After the local user successfully leaves the channel, the SDK triggers the onLeave() on the local client.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int leave() = 0;
  /**
   * Sends a channel message.
   *
   * After you successfully send a channel message, all members in the channel receive the message in the
   * onMessageReceived() callback.
   * @param message The pointer to the channel message that you want to send: IMessage.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int sendMessage(const IMessage *message) = 0;
  /**
   * Updates the channel attributes.
   *
   * A successful method call triggers the onUpdateAttributesResponse() callback on the local client.
   * @param attributes The pointer to the channel attributes that you want to update: IChannelAttributes.
   * @param requestId ID of the current update request.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int updateAttributes(IChannelAttributes *attributes, int64_t &requestId) = 0;
  /**
   * Removes the channel attributes.
   *
   * A successful method call triggers the onDeleteAttributesResponse() callback on the local client.
   * @param attributes The pointer to the channel attributes that you want to remove: IChannelAttributes.
   * @param requestId ID of the current delete request.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int deleteAttributes(IChannelAttributes *attributes, int64_t &requestId) = 0;
  /**
   * Gets the current request ID.
   * @return
   * - The pointer to the request ID, if the method call succeeds.
   * - An empty pointer NULL, if the method call fails.
   */
  virtual const char *getId() const = 0;

  // sync_call
  /**
   * Releases the IChannel instance.
   *
   * This is a synchronous method call, which means that the SDK reports the result of this method call
   * after the IChannel instance is successfully released.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int release() = 0;
};
/**
 * The IRtmServiceEventHandler class.
 */
class IRtmServiceEventHandler {
 public:
  virtual ~IRtmServiceEventHandler() {}

  /**
   * Occurs when the user successfully logs in the RTM service.
   */
  virtual void onLoginSuccess() {}
  /**
   * Occurs when the user fails to log in the RTM service.
   * @param errorCode The reason for the login failure: #LOGIN_ERR_CODE.
   */
  virtual void onLoginFailure(LOGIN_ERR_CODE errorCode) {}
  /**
   * Occurs when the user successfully logs out of the RTM service.
   */
  virtual void onLogout() {}
  /**
   * Occurs when the connection state of the local user has changed.
   * @param state The current connection state: #CONNECTION_STATE.
   */
  virtual void onConnectionStateChanged(CONNECTION_STATE state) {}
  /**
   * Reports the state of sending a message.
   * @param messageId ID of the message.
   * @param state The current state of the message: #PEER_MESSAGE_STATE.
   */
  virtual void onSendMessageState(int64_t messageId, PEER_MESSAGE_STATE state) {}
  /**
   * Occurs when the local user receives a message from a remote user.
   * @param peerId ID of the remote user that sends the message.
   * @param message The pointer to the message: IMessage.
   */
  virtual void onMessageReceivedFromPeer(const char *peerId, const IMessage *message) {}
};
/**
 * The IRtmService class.
 */
class IRtmService {
 public:
  virtual ~IRtmService() {}
  /**
   * Creates and gets an IRtmService instance.
   * @param appId The pointer to the app ID.
   * @param eventHandler The pointer to the IRtmServiceEventHandler object.
   * @param eventSpace The connection specific ID, used during report to argus.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int initialize(const char *appId, IRtmServiceEventHandler *eventHandler) = 0;
  /**
   * Releases the IRtmServiceEventHandler object.
   * @param eventHandler The pointer to the IRtmServiceEventHandler object.
   */
  virtual void unregisterObserver(IRtmServiceEventHandler *eventHandler) = 0;
  /**
   * Releases the IRtmService instance.
   * @param sync Determines whether to report the result of this method call synchronously.
   * - true: Report the result of this method call after the IRtmService instance is released.
   * - false: (Default) Report the result of this method call immediately, even when the IRtmService is not
   * released.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int release(bool sync = false) = 0;
  /**
   * Logs in the RTM service.
   *
   * @note
   * - If you login with the same user ID from a different instance, your previous login will be kicked.
   * - The call frequency limit of this method is 2 queries per second.
   * @param token The token used to log in the RTM service.
   * @param userId ID of the user logging in the RTM service.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int login(const char *token, const char *userId) = 0;
  /**
   * Logs out of the RTM service.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int logout() = 0;
  /**
   * Sends a peer message to a specified remote user.
   *
   * @param peerId The pointer to the ID of the remote user.
   * @param message The pointer to message: IMessage.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int sendMessageToPeer(const char *peerId, const IMessage *message) = 0;
  /**
   * Creates an RTM channel.
   *
   * @param channelId The unique channel name for an RTM session. Supported character scopes are:
   * - All lowercase English letters: a to z.
   * - All uppercase English letters: A to Z.
   * - All numeric characters: 0 to 9.
   * - The space character.
   * - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=",
   * ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ","
   *
   * @param eventHandler The pointer to IChannelEventHandler.
   * @return
   * - The pointer to an IChannel instance, if the method call succeeds.
   * - An empty pointer NULL, if the method call fails.
   */
  virtual IChannel *createChannel(const char *channelId, IChannelEventHandler *eventHandler) = 0;
};

}  // namespace rtm
}  // namespace agora
