description("Tests that when multiple requests are waiting for permission, no callbacks are invoked until permission is denied.");

if (!window.testRunner || !window.internals)
    debug('This test can not run without testRunner or internals');

internals.setGeolocationClientMock(document);
internals.setGeolocationPosition(document, 51.478, -0.166, 100);

var permissionSet = false;

function denyPermission() {
    permissionSet = true;
    internals.setGeolocationPermission(document, false);
}

var watchCallbackInvoked = false;
var oneShotCallbackInvoked = false;
var error;

navigator.geolocation.watchPosition(function() {
    testFailed('Success callback invoked unexpectedly');
    finishJSTest();
}, function(e) {
    if (permissionSet) {
        error = e;
        shouldBe('error.code', 'error.PERMISSION_DENIED');
        shouldBe('error.message', '"User denied Geolocation"');
        watchCallbackInvoked = true;
        maybeFinishTest();
        return;
    }
    testFailed('Error callback invoked unexpectedly');
    finishJSTest();
});

navigator.geolocation.getCurrentPosition(function() {
    testFailed('Success callback invoked unexpectedly');
    finishJSTest();
}, function(e) {
    if (permissionSet) {
        error = e;
        shouldBe('error.code', 'error.PERMISSION_DENIED');
        shouldBe('error.message', '"User denied Geolocation"');
        oneShotCallbackInvoked = true;
        maybeFinishTest();        
        return;
    }
    testFailed('Error callback invoked unexpectedly');
    finishJSTest();
});
window.setTimeout(denyPermission, 100);

function maybeFinishTest() {
    if (watchCallbackInvoked && oneShotCallbackInvoked)
        finishJSTest();
}

window.jsTestIsAsync = true;
