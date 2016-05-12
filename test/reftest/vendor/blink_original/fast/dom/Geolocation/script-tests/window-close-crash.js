description("Tests the assertion that the GeolocationClient should not be updating<br>" +
            "when the GeolocationController is destroyed.<br>" +
            "See https://bugs.webkit.org/show_bug.cgi?id=52216");

if (!window.testRunner || !window.internals)
    debug('This test can not run without testRunner or internals');

testRunner.waitUntilDone();
testRunner.setCanOpenWindows();
testRunner.setCloseRemainingWindowsWhenComplete(true);

var otherWindow;

function gotPosition(p)
{
    testPassed("Received Geoposition.");
    otherWindow.close();
    window.setTimeout(waitForWindowToClose, 0);
}

function waitForWindowToClose()
{
    if (!otherWindow.closed) {
        window.setTimeout(waitForWindowToClose, 0);
        return;
    }
    testPassed("Success - no crash!");
    finishJSTest();
}

debug("Main page opening resources/window-close-popup.html");
otherWindow = window.open("resources/window-close-popup.html");

window.jsTestIsAsync = true;
