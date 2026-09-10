fun main() {
    val errors = mutableListOf<String>()
    var joined = false
    val sut = ExampleCaseSession({ joined = it }, { errors.add(it) })
    fun request(channel: String) { check(sut.beginJoin(channel)); sut.onPermissionResult(true) }
    fun respond(index: Int, token: String? = "test-response") {
        TokenUtils.requests[index].callback(token)
        Handler.drain()
    }
    fun joins() = RtcEngine.instances.sumOf { it.joins.size }
    RtcEngine.failCreation = true
    sut.start(Context()); check(!sut.beginJoin("no-engine") && errors.size == 1)
    RtcEngine.failCreation = false
    sut.start(Context()); sut.start(Context()); check(RtcEngine.instances.size == 1)
    val first = RtcEngine.instances.last()
    request("old"); sut.dispose(); sut.dispose(); respond(0)
    check(joins() == 0 && RtcEngine.destroys == 1 && first.leaves == 1)
    sut.start(Context()); request("fresh"); respond(0); check(joins() == 0)
    respond(1); check(joins() == 1 && RtcEngine.instances.last().joins.last() == ("fresh" to 0))
    sut.leave(); request("leaving"); sut.leave(); respond(2); check(joins() == 1)
    request("superseded"); request("latest"); respond(4); respond(3)
    check(joins() == 2 && RtcEngine.instances.last().joins.last().first == "latest")
    sut.leave()
    check(sut.beginJoin("permission-old")); check(!sut.beginJoin("second-permission"))
    val count = TokenUtils.requests.size
    sut.dispose(); sut.start(Context())
    check(!sut.beginJoin("new-before-old-permission-returns"))
    sut.onPermissionResult(true); check(TokenUtils.requests.size == count)
    check(sut.beginJoin("denied")); sut.onPermissionResult(false); check(TokenUtils.requests.size == count)
    AgoraConfig.certificate = "enabled-in-test"
    request("missing-token"); respond(count, null); check(joins() == 2)
    AgoraConfig.certificate = ""
    request("no-certificate"); respond(count + 1, null); check(joins() == 3)
    sut.leave(); RtcEngine.joinResult = -1
    request("join-error"); respond(count + 2); check(errors.any { it.contains("joinChannel failed") })
    // Old SDK callback must not resurrect UI state after the same holder starts another engine.
    first.config.mEventHandler!!.onJoinChannelSuccess("fresh", 0, 0)
    Handler.drain(); check(!joined)
    sut.dispose()
    check(errors.none { it.contains("test-response") })
    println("PASS: initialization failure, pending exit, repeat cleanup/setup, reopen, leave, reordered responses, permission and token failures, join error")
}
