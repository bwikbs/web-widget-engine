
var system = require('system');
var args = system.args;

// console.log(args[1])

fs = require('fs')

var content = fs.read(args[1], "utf8")
// console.log(content);

var absPath = fs.absolute(args[1]);
// console.log(absPath)
var toolAbsPath = fs.absolute("./tool/html2xml/index.html")

var page = require('webpage').create();
page.onConsoleMessage = function(msg, lineNum, sourceId) {
	console.log('CONSOLE: ' + msg + '\n');
};

var resultXMLFolder=""
if (args[2] && args[2].indexOf("--result-folder=") == 0) {
	resultXMLFolder = args[2].slice("--result-folder=".length);
	console.log(resultXMLFolder)
}

var resultXMLFile=""
if (args[2] && args[2].indexOf("--result-file=") == 0) {
	resultXMLFile = args[2].slice("--result-file=".length);
	console.log(resultXMLFile)
}

page.open("tool/html2xml/index.html", function() {
	page.evaluate(function (data, absPath, point) {
		window.point = point
		var code = document.getElementById("code");
		code.value = data;
                code.url = absPath;
		onSubmit();
		// console.log(absPath);
		return "submit"
	}, content, absPath, absPath.substring(0,absPath.lastIndexOf("/")+1))

	setTimeout(function wait() {
		// page.render('result.png');
		var result = page.evaluate(function (absPath) {
			var result = document.getElementById("result");
			// console.log(result.value)
			return result.value
		});
		if (result.length == "") {
			window.setTimeout(wait, 100);
			return;
		}
		var resultXML;
        if (resultXMLFile != "")
            resultXML = resultXMLFile;
        else
            resultXML = resultXMLFolder + "./result.xml"
		fs.write(resultXML, result, "w");
		var wd = absPath.substring(0, absPath.lastIndexOf("/") + 1)

		var process = require("child_process")
		var spawn = process.spawn
		var execFile = process.execFile
		var args = [resultXML, "--working-directory="+wd];
		var i;
		for(i = 2; i < system.args.length; i ++) {
			args.push(system.args[i])
		}
		console.log(args);
		var child = spawn("./StarFish", args);

		child.stdout.on("data", function (data) {
			console.log("spawnSTDOUT:", JSON.stringify(data))
		})

		child.stderr.on("data", function (data) {
			console.log("STDERR:", JSON.stringify(data))
		})

		child.on("exit", function (code) {
			phantom.exit();
		})

	}, 100);
});

