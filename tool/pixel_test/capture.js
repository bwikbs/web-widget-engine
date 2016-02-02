var fs = require('fs');
var system = require('system');
if (typeof String.prototype.endsWith !== 'function') {
    String.prototype.endsWith = function(suffix) {
        return this.indexOf(suffix, this.length - suffix.length) !== -1;
    };
}

var args = system.args;
var platformName = "linux";
var testPath = "";
var basePath = "../../test/";
var filelist = [];

function initialize() {
    if (args.length == 1) {
        testPath = "/unittest/css";
        basePath = "../../test/";
    }
    else if (args.length == 2) {
        if (args[1] == "css" || args[1] == "dom" || args[1] == "xhr") {
            testPath = "/unittest/" + args[1];
            return true;
        }
    }
    else if (args.length == 3) {
        if (args[1] == "-f" && fs.isFile(args[2]) && args[2].endsWith(".html")) {
            var last = args[2].lastIndexOf('/');
            filelist[0] = args[2].substring(last);

            var index = args[2].indexOf("test/");
            if (index >= 0)
                testPath = args[2].substring(index+5, last);
            else
                testPath = filelist[0];
            return true;
        }
    }
    return false;
}

if (!initialize()) {
    console.log("Usage: phantomjs capture.js [category=css|dom|xhr]       capture all html pages of category");
    console.log("   or: phantomjs capture.js -f [filepath=*.html]         capture only one page\n");

    phantom.exit();
}

var pathToSave = basePath + "/platform/" + platformName + "/" + testPath + "/";
var pathToTest = basePath + "/" + testPath + "/";
console.log("     * The captured images will be saved in " + pathToSave);

var page = require('webpage').create();
if (filelist.length == 0) {
    var list = fs.list(pathToTest);
    for (var i = 0; i < list.length; i++) {
        if (fs.isFile(pathToTest + list[i]) && list[i].endsWith(".html")) {
            filelist.push(list[i]);
        }
    }
}

var idx = 0;
var processing = false;

page.onLoadStarted = function() {
    processing = true;
    page.evaluate(function() {
        document.body.bgColor = 'white';
//        document.body.style.color = 'd';
        document.body.style.fontFamily = 'Ahem';
    });
    console.log(' [' + filelist[idx] + '] Loading Started...');
};

page.onLoadFinished = function() {
    var renderFileName = pathToSave + filelist[idx] + '.png';
    page.render(renderFileName);
    console.log('    ... Captured in '+ renderFileName);
    idx++;
    processing = false;
};

setInterval(function() {
  if (!processing && idx < filelist.length) {
    var file = pathToTest + filelist[idx];
    page.open(file);
  } else if (idx == filelist.length) {
    console.log("# " + filelist.length + " tests are captured");
    phantom.exit();
  }
}, 250);

